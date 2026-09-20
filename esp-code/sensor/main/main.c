#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "mqtt_client.h"
#include "dht.h"
#include <string.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "sdkconfig.h"


#define WIFI_SSID       CONFIG_MY_WIFI_SSID
#define WIFI_PASS       CONFIG_MY_WIFI_PASSWORD
#define MQTT_BROKER_URL CONFIG_MY_MQTT_BROKER_URL

typedef struct {
    float temperature;
    float humidity;
} sensor_data_t;


static const char *TAG = "MAIN";
static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool wifi_connected = false;
static QueueHandle_t sensor_queue = NULL;


// Event Handler for Wi-Fi events and IP events
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_connected = false;
        ESP_LOGI(TAG, "Disconnected from Wi-Fi, retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_connected = true;
        if(mqtt_client) {
            esp_mqtt_client_start(mqtt_client);
        }
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Successfully connected to MQTT Broker");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected from MQTT Broker");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Event Error encountered");
            break;
        default:
            break;
    }
}


void init_network_and_mqtt(void) {
    // 1. Initialize NVS (Required for Wi-Fi storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize Netif and Default Event Loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 3. Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register Wi-Fi Handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.rssi = -127,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // 4. Initialize MQTT Config (ESP-IDF v5.x structural style)
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URL,
    };
    
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);}



// Sensor reading task
void dht11_task(void *pvParameters) {
    dht_sensor_t dht_sensor;
    gpio_num_t dht_pin = GPIO_NUM_4;

    if (dht_init(&dht_sensor, dht_pin, DHT_TYPE_DHT11) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize DHT GPIO pin");
        vTaskDelete(NULL);
    }

    while (1) {
        float humidity = 0.0f;
        float temp_c = 0.0f;

        esp_err_t res = dht_read_data(&dht_sensor, &humidity, &temp_c);
        if (res == ESP_OK) {
            sensor_data_t data = { .temperature = temp_c, .humidity = humidity };
            xQueueSend(sensor_queue, &data, portMAX_DELAY);
        } else if (res == ESP_ERR_TIMEOUT) {
            ESP_LOGW(TAG, "Sensor read timeout (check wiring)");
        } else if (res == ESP_ERR_INVALID_CRC) {
            ESP_LOGE(TAG, "CRC validation failed");
        }

        // Delay at least 5 mins between reads
        vTaskDelay(pdMS_TO_TICKS(5*60000));
    }
}

void mqtt_publish_task(void *pvParameters) {
    sensor_data_t received_data;

    while (1) {
        if (xQueueReceive(sensor_queue, &received_data, portMAX_DELAY)) {
            char payload[100];
            snprintf(payload, sizeof(payload), "{\"temperature\": %.1f, \"humidity\": %.1f}", received_data.temperature, received_data.humidity);
            esp_mqtt_client_publish(mqtt_client, "sensor/dht11", payload, 0, 1, 0);
            ESP_LOGI(TAG, "Published: %s", payload);
        }
    }
}

void app_main(void) {
    init_network_and_mqtt();
    sensor_queue = xQueueCreate(5, sizeof(sensor_data_t));

    if (sensor_queue != NULL) {
        xTaskCreate(dht11_task, "dht11_task", 4096, NULL, 5, NULL);
        xTaskCreate(mqtt_publish_task, "mqtt_publish_task", 4096, NULL, 5, NULL);
    }
    else{
        ESP_LOGE(TAG, "Failed to create sensor queue");
    }
}