#include "network_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_device.h"
#include "event_logger.h"
#include "nvs.h"
#include <string.h>

static const char *TAG = "network_manager";

// Wi-Fi event group
static EventGroupHandle_t wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;

// BLE service handles
static uint16_t gatts_service_handle = 0;
static uint16_t gatts_char_handle_temp = 0;
static uint16_t gatts_char_handle_humidity = 0;
static uint16_t gatts_char_handle_light = 0;

// Forward declarations
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data);
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                              esp_ble_gatts_cb_param_t *param);

// Initialize network manager
esp_err_t network_manager_init(void) {
    ESP_LOGI(TAG, "Initializing network manager");
    
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Create event group
    wifi_event_group = xEventGroupCreate();
    
    // Initialize TCP/IP adapter
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                             &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                             &wifi_event_handler, NULL));
    
    // Initialize Bluetooth
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
    
    // Register BLE callbacks
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));
    ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_event_handler));
    
    event_logger_add("Network manager initialized", false);
    return ESP_OK;
}

// Start Wi-Fi with configuration
esp_err_t network_manager_wifi_start(rc_wifi_config_t *config) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {0};
    strncpy((char *)wifi_config.sta.ssid, config->ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, config->password, sizeof(wifi_config.sta.password));
    
    switch (config->mode) {
        case RC_WIFI_MODE_AP:
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
            break;
            
        case RC_WIFI_MODE_STA:
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
            break;
            
        case RC_WIFI_MODE_APSTA:
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
            break;
    }
    
    ESP_ERROR_CHECK(esp_wifi_start());
    event_logger_add("Wi-Fi started", false);
    return ESP_OK;
}

// Stop Wi-Fi
esp_err_t network_manager_wifi_stop(void) {
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    event_logger_add("Wi-Fi stopped", false);
    return ESP_OK;
}

// Check Wi-Fi connection status
bool network_manager_wifi_is_connected(void) {
    return (xEventGroupGetBits(wifi_event_group) & WIFI_CONNECTED_BIT) != 0;
}

// Start BLE services
esp_err_t network_manager_ble_start(void) {
    esp_ble_adv_params_t adv_params = {
        .adv_int_min = 0x20,
        .adv_int_max = 0x40,
        .adv_type = ADV_TYPE_IND,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .channel_map = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    
    ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
    event_logger_add("BLE services started", false);
    return ESP_OK;
}

// Stop BLE services
esp_err_t network_manager_ble_stop(void) {
    ESP_ERROR_CHECK(esp_ble_gap_stop_advertising());
    event_logger_add("BLE services stopped", false);
    return ESP_OK;
}

// Update BLE sensor values
esp_err_t network_manager_ble_update_sensors(float temperature, float humidity, float light) {
    if (gatts_service_handle == 0) {
        return ESP_FAIL;
    }
    
    // Update temperature characteristic
    esp_ble_gatts_set_attr_value(gatts_char_handle_temp,
                                sizeof(temperature), (uint8_t*)&temperature);
    
    // Update humidity characteristic
    esp_ble_gatts_set_attr_value(gatts_char_handle_humidity,
                                sizeof(humidity), (uint8_t*)&humidity);
    
    // Update light characteristic
    esp_ble_gatts_set_attr_value(gatts_char_handle_light,
                                sizeof(light), (uint8_t*)&light);
    
    return ESP_OK;
}

bool network_manager_load_wifi_credentials(char *ssid, size_t ssid_len,
                                           char *password, size_t pass_len,
                                           rc_wifi_mode_t *mode) {
    nvs_handle_t h;
    if (nvs_open("settings", NVS_READONLY, &h) != ESP_OK) {
        return false;
    }
    size_t s_len = ssid_len;
    size_t p_len = pass_len;
    if (nvs_get_str(h, "wifi_ssid", ssid, &s_len) != ESP_OK ||
        nvs_get_str(h, "wifi_password", password, &p_len) != ESP_OK) {
        nvs_close(h);
        return false;
    }
    int32_t mode_val;
    if (nvs_get_i32(h, "wifi_mode", &mode_val) != ESP_OK) {
        mode_val = WIFI_MODE_STA;
    }
    if (mode) *mode = (rc_wifi_mode_t)mode_val;
    nvs_close(h);
    return true;
}

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
                
            case WIFI_EVENT_STA_DISCONNECTED:
                xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
                esp_wifi_connect();
                event_logger_add("Wi-Fi disconnected, attempting reconnection", true);
                break;
                
            case WIFI_EVENT_AP_STACONNECTED:
                event_logger_add("Station connected to AP", false);
                break;
                
            case WIFI_EVENT_AP_STADISCONNECTED:
                event_logger_add("Station disconnected from AP", false);
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        event_logger_add("Wi-Fi connected", false);
    }
}

// BLE GAP event handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&param->adv_data_cmpl);
            break;
            
        default:
            break;
    }
}

// BLE GATTS event handler
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                              esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            // Service registered
            break;
            
        case ESP_GATTS_CONNECT_EVT:
            event_logger_add("BLE client connected", false);
            break;
            
        case ESP_GATTS_DISCONNECT_EVT:
            event_logger_add("BLE client disconnected", false);
            esp_ble_gap_start_advertising(NULL);
            break;
            
        default:
            break;
    }
}
