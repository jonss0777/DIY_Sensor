#include "dht.h"

#include "esp_rom_sys.h"      // Required for esp_rom_delay_us()
#include "esp_timer.h"        // Required for esp_timer_get_time()
#include "driver/gpio.h"      // Required for gpio_config, gpio_set_level, etc.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DHT_TIMEOUT_US 100

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

static int32_t dht_wait_state(gpio_num_t pin, int level) {
    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(pin) == level) {
        if ((esp_timer_get_time() - start_time) > DHT_TIMEOUT_US) {
            return -1;
        }
    }
    return (int32_t)(esp_timer_get_time() - start_time);
}

esp_err_t dht_read_data(const dht_sensor_t *dev, float *humidity, float *temperature) {
    uint8_t data[5] = {0};

    // 1. Send start signal
    gpio_set_direction(dev->pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dev->pin, 0);

    if (dev->type == DHT_TYPE_DHT11) {
        vTaskDelay(pdMS_TO_TICKS(20)); // DHT11 needs at least 18ms LOW
    } else {
        vTaskDelay(pdMS_TO_TICKS(2));  // DHT22/AM2302 needs 1-2ms LOW
    }

    // Release line and switch back to input with pull-up
    gpio_set_level(dev->pin, 1);
    esp_rom_delay_us(30);
    gpio_set_direction(dev->pin, GPIO_MODE_INPUT);

    // 2. Critical Section for timing
    portENTER_CRITICAL(&dht_spinlock);

    // Sensor ACK response: Wait for line to go LOW (80us), then wait for line to go HIGH (80us)
    if (dht_wait_state(dev->pin, 1) < 0) { // Wait for initial HIGH to end (sensor pulls line LOW)
        portEXIT_CRITICAL(&dht_spinlock);
        return ESP_ERR_TIMEOUT;
    }
    if (dht_wait_state(dev->pin, 0) < 0) { // Wait for LOW ACK pulse to end
        portEXIT_CRITICAL(&dht_spinlock);
        return ESP_ERR_TIMEOUT;
    }
    if (dht_wait_state(dev->pin, 1) < 0) { // Wait for HIGH ACK pulse to end
        portEXIT_CRITICAL(&dht_spinlock);
        return ESP_ERR_TIMEOUT;
    }

    // 3. Read 40 bits of data
    for (int i = 0; i < 40; i++) {
        // Each bit starts with a 50us LOW pulse
        if (dht_wait_state(dev->pin, 0) < 0) {
            portEXIT_CRITICAL(&dht_spinlock);
            return ESP_ERR_TIMEOUT;
        }

        // Measure length of HIGH pulse (26-28us = 0, 70us = 1)
        int32_t high_duration = dht_wait_state(dev->pin, 1);
        if (high_duration < 0) {
            portEXIT_CRITICAL(&dht_spinlock);
            return ESP_ERR_TIMEOUT;
        }

        data[i / 8] <<= 1;
        if (high_duration > 40) { // If HIGH > 40us, bit is '1'
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