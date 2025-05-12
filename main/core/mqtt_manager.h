#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

// MQTT Topics
#define MQTT_TOPIC_TEMP        "repticontrol/sensors/temperature"
#define MQTT_TOPIC_HUMIDITY    "repticontrol/sensors/humidity"
#define MQTT_TOPIC_LIGHT       "repticontrol/sensors/light"
#define MQTT_TOPIC_HEATING     "repticontrol/status/heating"
#define MQTT_TOPIC_COOLING     "repticontrol/status/cooling"
#define MQTT_TOPIC_HUMIDIFIER  "repticontrol/status/humidifier"
#define MQTT_TOPIC_LIGHTING    "repticontrol/status/lighting"
#define MQTT_TOPIC_BATTERY     "repticontrol/status/battery"
#define MQTT_TOPIC_ALERTS      "repticontrol/alerts"
#define MQTT_TOPIC_COMMANDS    "repticontrol/commands"

// Initialize MQTT manager
esp_err_t mqtt_manager_init(void);

// Connect to MQTT broker
esp_err_t mqtt_manager_connect(const char* broker_url, int port, 
                             const char* username, const char* password);

// Disconnect from MQTT broker
esp_err_t mqtt_manager_disconnect(void);

// Publish sensor data
esp_err_t mqtt_manager_publish_sensors(float temperature, float humidity, float light);

// Publish system status
esp_err_t mqtt_manager_publish_status(bool heating_on, bool cooling_on,
                                    bool humidifier_on, bool lighting_on,
                                    int battery_level);

// Publish alert
esp_err_t mqtt_manager_publish_alert(const char* message, bool is_critical);

// Check connection status
bool mqtt_manager_is_connected(void);

#endif /* MQTT_MANAGER_H */