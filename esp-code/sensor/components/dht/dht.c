#include "dht.h"
#include "esp_rom_sys.h"      // Required for esp_rom_delay_us()
#include "driver/gpio.h"      // Required for gpio_config, gpio_set_level, etc.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static portMUX_TYPE dht_spinlock = portMUX_INITIALIZER_UNLOCKED;

esp_err_t dht_init(dht_sensor_t *dev, gpio_num_t pin, dht_type_t type) {
    if (!dev) return ESP_ERR_INVALID_ARG;

    dev->pin = pin;
    dev->type = type;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << pin),
        .mode = GPIO_MODE_INPUT_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    return gpio_config(&io_conf);
}

// Fast timeout reader: uses microsecond delays rather than heavy 64-bit timer reads
static int32_t dht_wait_state_fast(gpio_num_t pin, int level, int32_t timeout_us) {
    int32_t micros = 0;
    while (gpio_get_level(pin) == level) {
        if (micros >= timeout_us) {
            return -1;
        }
        esp_rom_delay_us(1);
        micros++;
    }
    return micros;
}

esp_err_t dht_read_data(const dht_sensor_t *dev, float *humidity, float *temperature) {
    uint8_t data[5] = {0};

    // 1. Send start signal
    gpio_set_direction(dev->pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dev->pin, 0);

    if (dev->type == DHT_TYPE_DHT11) {
        vTaskDelay(pdMS_TO_TICKS(20)); // DHT11 needs at least 18ms LOW
    } else {
        vTaskDelay(pdMS_TO_TICKS(2));  // DHT22 needs 1-2ms LOW
    }

    // Release line and set to high-impedance input with pull-up
    gpio_set_level(dev->pin, 1);
    gpio_set_direction(dev->pin, GPIO_MODE_INPUT);
    esp_rom_delay_us(40); // Allow line to float back HIGH and sensor to pull LOW

    // 2. Critical Section for precise bit-bang timing
    portENTER_CRITICAL(&dht_spinlock);

    // Sensor ACK: Expect 80us LOW followed by 80us HIGH
    if (dht_wait_state_fast(dev->pin, 0, 100) < 0) { // Wait for LOW ACK to finish
        portEXIT_CRITICAL(&dht_spinlock);
        return ESP_ERR_TIMEOUT;
    }
    if (dht_wait_state_fast(dev->pin, 1, 100) < 0) { // Wait for HIGH ACK to finish
        portEXIT_CRITICAL(&dht_spinlock);
        return ESP_ERR_TIMEOUT;
    }

    // 3. Read 40 bits of data
    for (int i = 0; i < 40; i++) {
        // Each bit starts with a 50us LOW pulse
        if (dht_wait_state_fast(dev->pin, 0, 100) < 0) {
            portEXIT_CRITICAL(&dht_spinlock);
            return ESP_ERR_TIMEOUT;
        }

        // Measure duration of the HIGH pulse (26-28us = '0', 70us = '1')
        int32_t high_duration = dht_wait_state_fast(dev->pin, 1, 100);
        if (high_duration < 0) {
            portEXIT_CRITICAL(&dht_spinlock);
            return ESP_ERR_TIMEOUT;
        }

        data[i / 8] <<= 1;
        if (high_duration > 40) { // If HIGH duration > 40us, it's a '1'
            data[i / 8] |= 1;
        }
    }

    portEXIT_CRITICAL(&dht_spinlock);

    // 4. Checksum Verification
    uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if (data[4] != checksum) {
        return ESP_ERR_INVALID_CRC;
    }

    // 5. Decode Values
    if (dev->type == DHT_TYPE_DHT11) {
        if (humidity) *humidity = (float)data[0];
        if (temperature) *temperature = (float)data[2];
    } else {
        if (humidity) {
            *humidity = (float)((data[0] << 8) | data[1]) / 10.0f;
        }
        if (temperature) {
            int16_t temp_raw = (data[2] & 0x7F) << 8 | data[3];
            float temp = (float)temp_raw / 10.0f;
            if (data[2] & 0x80) {
                temp = -temp;
            }
            *temperature = temp;
        }
    }

    return ESP_OK;
}