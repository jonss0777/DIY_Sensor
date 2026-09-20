#ifndef DHT_H
#define DHT_H

#include "driver/gpio.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported DHT sensor models.
 */
typedef enum {
    DHT_TYPE_DHT11 = 11,   /*!< DHT11 sensor */
    DHT_TYPE_DHT22 = 22,   /*!< DHT22 (AM2302) sensor */
    DHT_TYPE_AM2301 = 21   /*!< AM2301 sensor */
} dht_type_t;

/**
 * @brief DHT Sensor Instance Structure.
 */
typedef struct {
    gpio_num_t pin;       /*!< GPIO pin connected to the DHT data line */
    dht_type_t type;      /*!< Type of the DHT sensor */
} dht_sensor_t;

/**
 * @brief Initialize the GPIO pin configured for the DHT sensor.
 * 
 * Sets the GPIO mode to open-drain with internal pull-up enabled.
 * 
 * @param[out] dev Pointer to the sensor structure handle.
 * @param[in]  pin GPIO pin number (e.g., GPIO_NUM_4). Avoid input-only pins (34-39).
 * @param[in]  type Sensor model (DHT_TYPE_DHT11, DHT_TYPE_DHT22, or DHT_TYPE_AM2301).
 * 
 * @return 
 *      - ESP_OK: Initialization successful
 *      - ESP_ERR_INVALID_ARG: Null handle provided
 */
esp_err_t dht_init(dht_sensor_t *dev, gpio_num_t pin, dht_type_t type);

/**
 * @brief Read relative humidity and temperature from the DHT sensor.
 * 
 * Performs start sequence, reads the 40-bit transmission payload, and computes CRC.
 * Must not be called more frequently than once every 1 to 2 seconds.
 * 
 * @param[in]  dev Pointer to initialized sensor handle.
 * @param[out] humidity Pointer to float variable where humidity (%) will be stored. Can be NULL.
 * @param[out] temperature Pointer to float variable where temperature (°C) will be stored. Can be NULL.
 * 
 * @return 
 *      - ESP_OK: Successful read and valid checksum
 *      - ESP_ERR_INVALID_ARG: Null sensor handle
 *      - ESP_ERR_TIMEOUT: Sensor failed to respond or timing out during transmission
 *      - ESP_ERR_INVALID_CRC: Checksum failed (data corruption)
 */
esp_err_t dht_read_data(const dht_sensor_t *dev, float *humidity, float *temperature);

/* --- Utility Helper Functions / Macros --- */

/**
 * @brief Convert Celsius to Fahrenheit.
 */
static inline float dht_c_to_f(float celsius) {
    return (celsius * 1.8f) + 32.0f;
}

/**
 * @brief Convert Fahrenheit to Celsius.
 */
static inline float dht_f_to_c(float fahrenheit) {
    return (fahrenheit - 32.0f) * 0.5555555556f;
}

/**
 * @brief Calculate Heat Index (Feels Like temperature in °C).
 * 
 * Uses Rothfusz regression equation.
 * 
 * @param[in] temperature_c Temperature in Celsius.
 * @param[in] humidity_percent Relative humidity percentage.
 * @return Heat index in Celsius.
 */
float dht_compute_heat_index(float temperature_c, float humidity_percent);

#ifdef __cplusplus
}
#endif

#endif // DHT_H