#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "event_logger.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "mqtt_manager";

// MQTT client handle
static esp_mqtt_client_handle_t mqtt_client = NULL;

// Connection status
static bool is_connected = false;

// Forward declarations
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                             int32_t event_id, void *event_data);

// Initialize MQTT manager
esp_err_t mqtt_manager_init(void) {
    ESP_LOGI(TAG, "Initializing MQTT manager");
    
    // Configuration will be done when connecting
    event_logger_add("MQTT manager initialized", false);
    return ESP_OK;
}

// Connect to MQTT broker
esp_err_t mqtt_manager_connect(const char* broker_url, int port, 
                             const char* username, const char* password) {
    if (mqtt_client) {
        ESP_LOGW(TAG, "MQTT client already initialized");
        return ESP_FAIL;
    }
    
    // Create MQTT client configuration
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
        .broker.address.port = port,
        .credentials.username = username,
        .credentials.authentication.password = password,
        .session.last_will.topic = MQTT_TOPIC_STATUS,
        .session.last_will.msg = "offline",
        .session.last_will.qos = 1,
        .session.last_will.retain = true
    };
    
    // Initialize MQTT client
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (!mqtt_client) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }
    
    // Register event handler
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_client, 
                                                 ESP_EVENT_ANY_ID,
                                                 mqtt_event_handler,
                                                 NULL));
    
    // Start MQTT client
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));
    
    event_logger_add("MQTT client started", false);
    return ESP_OK;
}

// Disconnect from MQTT broker
esp_err_t mqtt_manager_disconnect(void) {
    if (!mqtt_client) {
        return ESP_OK;
    }
    
    // Stop MQTT client
    ESP_ERROR_CHECK(esp_mqtt_client_stop(mqtt_client));
    ESP_ERROR_CHECK(esp_mqtt_client_destroy(mqtt_client));
    mqtt_client = NULL;
    is_connected = false;
    
    event_logger_add("MQTT client disconnected", false);
    return ESP_OK;
}

// Publish sensor data
esp_err_t mqtt_manager_publish_sensors(float temperature, float humidity, float light) {
    if (!is_connected) {
        return ESP_FAIL;
    }
    
    char data[32];
    
    // Publish temperature
    snprintf(data, sizeof(data), "%.1f", temperature);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_TEMP, data, 0, 1, 0);
    
    // Publish humidity
    snprintf(data, sizeof(data), "%.1f", humidity);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_HUMIDITY, data, 0, 1, 0);
    
    // Publish light
    snprintf(data, sizeof(data), "%.1f", light);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_LIGHT, data, 0, 1, 0);
    
    return ESP_OK;
}

// Publish system status
esp_err_t mqtt_manager_publish_status(bool heating_on, bool cooling_on,
                                    bool humidifier_on, bool lighting_on,
                                    int battery_level) {
    if (!is_connected) {
        return ESP_FAIL;
    }
    
    // Publish individual status
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_HEATING,
                          heating_on ? "on" : "off", 0, 1, 1);
    
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_COOLING,
                          cooling_on ? "on" : "off", 0, 1, 1);
    
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_HUMIDIFIER,
                          humidifier_on ? "on" : "off", 0, 1, 1);
    
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_LIGHTING,
                          lighting_on ? "on" : "off", 0, 1, 1);
    
    // Publish battery level
    char battery_str[8];
    snprintf(battery_str, sizeof(battery_str), "%d", battery_level);
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_BATTERY,
                          battery_str, 0, 1, 1);
    
    return ESP_OK;
}

// Publish alert
esp_err_t mqtt_manager_publish_alert(const char* message, bool is_critical) {
    if (!is_connected) {
        return ESP_FAIL;
    }
    
    // Create JSON alert message
    char alert[256];
    snprintf(alert, sizeof(alert),
             "{\"message\":\"%s\",\"level\":\"%s\",\"timestamp\":%lld}",
             message,
             is_critical ? "critical" : "warning",
             (long long)time(NULL));
    
    esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_ALERTS,
                          alert, 0, 1, 0);
    
    return ESP_OK;
}

// Check connection status
bool mqtt_manager_is_connected(void) {
    return is_connected;
}

// MQTT event handler
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                             int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected to broker");
            is_connected = true;
            
            // Subscribe to command topic
            esp_mqtt_client_subscribe(mqtt_client, MQTT_TOPIC_COMMANDS, 1);
            
            // Publish online status
            esp_mqtt_client_publish(mqtt_client, MQTT_TOPIC_STATUS,
                                  "online", 0, 1, 1);
            
            event_logger_add("Connected to MQTT broker", false);
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected from broker");
            is_connected = false;
            event_logger_add("Disconnected from MQTT broker", true);
            break;
            
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Subscribed to topic");
            break;
            
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Unsubscribed from topic");
            break;
            
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT Message published");
            break;
            
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT Message received");
            // Handle incoming commands
            if (strncmp(event->topic, MQTT_TOPIC_COMMANDS,
                       strlen(MQTT_TOPIC_COMMANDS)) == 0) {
                // Process command
                // TODO: Implement command handling
            }
            break;
            
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Error occurred");
            event_logger_add("MQTT error occurred", true);
            break;
            
        default:
            ESP_LOGI(TAG, "MQTT Other event id: %d", event->event_id);
            break;
    }
}