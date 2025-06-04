/**
 * @file network_manager.h
 * @brief Network communication management (Wi-Fi and BLE)
 */

#ifndef CORE_NETWORK_MANAGER_H
#define CORE_NETWORK_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

// Wi-Fi modes
typedef enum {
    WIFI_MODE_AP,      // Access Point mode
    WIFI_MODE_STA,     // Station mode
    WIFI_MODE_APSTA    // AP + Station mode
} wifi_mode_t;

// Wi-Fi configuration
typedef struct {
    char ssid[33];
    char password[65];
    wifi_mode_t mode;
} wifi_config_t;

// BLE service UUIDs
#define BLE_SERVICE_UUID        "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define BLE_TEMP_CHAR_UUID     "cba1d466-344c-4be3-ab3f-189f80dd7518"
#define BLE_HUMIDITY_CHAR_UUID  "ca73b3ba-39f6-4ab3-91ae-186dc9577d99"
#define BLE_LIGHT_CHAR_UUID    "f27b53ad-c63d-49a0-8c0f-9f297e6cc520"

/**
 * @brief Initialize network manager
 * @return ESP_OK on success
 */
esp_err_t network_manager_init(void);

/**
 * @brief Configure and start Wi-Fi
 * @param config Wi-Fi configuration
 * @return ESP_OK on success
 */
esp_err_t network_manager_wifi_start(wifi_config_t *config);

/**
 * @brief Stop Wi-Fi
 * @return ESP_OK on success
 */
esp_err_t network_manager_wifi_stop(void);

/**
 * @brief Check if Wi-Fi is connected
 * @return true if connected
 */
bool network_manager_wifi_is_connected(void);

/**
 * @brief Start BLE services
 * @return ESP_OK on success
 */
esp_err_t network_manager_ble_start(void);

/**
 * @brief Stop BLE services
 * @return ESP_OK on success
 */
esp_err_t network_manager_ble_stop(void);

/**
 * @brief Update BLE sensor values
 * @param temperature Current temperature
 * @param humidity Current humidity
 * @param light Current light level
 * @return ESP_OK on success
 */
esp_err_t network_manager_ble_update_sensors(float temperature, float humidity, float light);

// Load Wi-Fi credentials from NVS
bool network_manager_load_wifi_credentials(char *ssid, size_t ssid_len,
                                           char *password, size_t pass_len,
                                           wifi_mode_t *mode);

 vlhdas-codex/rendre-le-projet-compatible-avec-waveshare-esp32-s3-touch-lc
#endif /* CORE_NETWORK_MANAGER_H */

#endif /* CORE_NETWORK_MANAGER_H */
 main
