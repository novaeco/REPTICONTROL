#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "event_logger.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "cJSON.h"

static const char *TAG = "mqtt_manager";

// MQTT client handle
static esp_mqtt_client_handle_t mqtt_client = NULL;

// Connection status
static bool is_connected = false;

// Device unique identifier
static char device_id[32];

// Forward declarations
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                             int32_t event_id, void *event_data);
static esp_err_t publish_ha_discovery_sensor(const char* sensor_type, 
                                           const char* name,
                                           const char* unit,
                                           const char* state_topic,
                                           const char* device_class);
static esp_err_t publish_ha_discovery_switch(const char* component, 
                                           const char* name,
                                           const char* state_topic,
                                           const char* command_topic);

// Initialize MQTT manager
esp_err_t mqtt_manager_init(void) {
    ESP_LOGI(TAG, "Initializing MQTT manager");
    
    // Generate unique device ID
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    snprintf(device_id, sizeof(device_id), "repticontrol_%02x%02x%02x",
             mac[3], mac[4], mac[5]);
    
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
        .session.last_will.retain = true,
        .session.keepalive = 30
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

// Configure Home Assistant auto-discovery
esp_err_t mqtt_manager_configure_ha_discovery(void) {
    if (!is_connected) {
        return ESP_FAIL;
    }
    
    // Configure sensors
    publish_ha_discovery_sensor("temperature", "Temperature", "°C",
                              MQTT_TOPIC_TEMP, "temperature");
    
    publish_ha_discovery_sensor("humidity", "Humidity", "%",
                              MQTT_TOPIC_HUMIDITY, "humidity");
    
    publish_ha_discovery_sensor("light", "Light Level", "%",
                              MQTT_TOPIC_LIGHT, "illuminance");
    
    publish_ha_discovery_sensor("battery", "Battery Level", "%",
                              MQTT_TOPIC_BATTERY, "battery");
    
    // Configure switches
    publish_ha_discovery_switch("heating", "Heating System",
                              MQTT_TOPIC_HEATING,
                              MQTT_TOPIC_COMMANDS "/heating");
    
    publish_ha_discovery_switch("cooling", "Cooling System",
                              MQTT_TOPIC_COOLING,
                              MQTT_TOPIC_COMMANDS "/cooling");
    
    publish_ha_discovery_switch("humidifier", "Humidifier",
                              MQTT_TOPIC_HUMIDIFIER,
                              MQTT_TOPIC_COMMANDS "/humidifier");
    
    publish_ha_discovery_switch("lighting", "Lighting",
                              MQTT_TOPIC_LIGHTING,
                              MQTT_TOPIC_COMMANDS "/lighting");
    
    return ESP_OK;
}

// Helper function to publish Home Assistant sensor discovery config
static esp_err_t publish_ha_discovery_sensor(const char* sensor_type,
                                           const char* name,
                                           const char* unit,
                                           const char* state_topic,
                                           const char* device_class) {
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/sensor/%s/%s/config",
             HA_DISCOVERY_PREFIX, device_id, sensor_type);
    
    // Create discovery message
    cJSON *config = cJSON_CreateObject();
    cJSON_AddStringToObject(config, "name", name);
    cJSON_AddStringToObject(config, "unique_id", device_id);
    cJSON_AddStringToObject(config, "state_topic", state_topic);
    cJSON_AddStringToObject(config, "device_class", device_class);
    cJSON_AddStringToObject(config, "unit_of_measurement", unit);
    
    // Add device info
    cJSON *device = cJSON_CreateObject();
    cJSON_AddStringToObject(device, "identifiers", device_id);
    cJSON_AddStringToObject(device, "name", "ReptiControl");
    cJSON_AddStringToObject(device, "model", "ReptiControl v1.0");
    cJSON_AddStringToObject(device, "manufacturer", "ReptiControl");
    cJSON_AddItemToObject(config, "device", device);
    
    char *message = cJSON_PrintUnformatted(config);
    esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, 1);
    
    free(message);
    cJSON_Delete(config);
    
    return ESP_OK;
}

// Helper function to publish Home Assistant switch discovery config
static esp_err_t publish_ha_discovery_switch(const char* component,
                                           const char* name,
                                           const char* state_topic,
                                           const char* command_topic) {
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/switch/%s/%s/config",
             HA_DISCOVERY_PREFIX, device_id, component);
    
    // Create discovery message
    cJSON *config = cJSON_CreateObject();
    cJSON_AddStringToObject(config, "name", name);
    cJSON_AddStringToObject(config, "unique_id", device_id);
    cJSON_AddStringToObject(config, "state_topic", state_topic);
    cJSON_AddStringToObject(config, "command_topic", command_topic);
    cJSON_AddStringToObject(config, "payload_on", "on");
    cJSON_AddStringToObject(config, "payload_off", "off");
    cJSON_AddBoolToObject(config, "retain", true);
    
    // Add device info
    cJSON *device = cJSON_CreateObject();
    cJSON_AddStringToObject(device, "identifiers", device_id);
    cJSON_AddStringToObject(device, "name", "ReptiControl");
    cJSON_AddStringToObject(device, "model", "ReptiControl v1.0");
    cJSON_AddStringToObject(device, "manufacturer", "ReptiControl");
    cJSON_AddItemToObject(config, "device", device);
    
    char *message = cJSON_PrintUnformatted(config);
    esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, 1);
    
    free(message);
    cJSON_Delete(config);
    
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