#include "ota_manager.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "event_logger.h"
#include <string.h>

static const char *TAG = "ota_manager";

// OTA configuration
#define OTA_FIRMWARE_URL "https://api.repticontrol.com/firmware/"
#define OTA_BUFFER_SIZE 1024
#define OTA_STACK_SIZE 8192

// OTA state
static ota_state_t current_state = OTA_STATE_IDLE;
static char error_message[256] = {0};
static ota_progress_cb_t progress_callback = NULL;
static esp_ota_handle_t update_handle = 0;
static const esp_partition_t *update_partition = NULL;

// Forward declarations
static void http_cleanup(esp_http_client_handle_t client);
static esp_err_t validate_image_header(esp_app_desc_t *new_app_info);
static void set_error(const char* message);

// Initialize OTA manager
esp_err_t ota_manager_init(void) {
    ESP_LOGI(TAG, "Initializing OTA manager");

    // Check if OTA support is enabled in partition table
    const esp_partition_t *running = esp_ota_get_running_partition();
    const esp_partition_t *update = esp_ota_get_next_update_partition(NULL);

    if (!running || !update) {
        ESP_LOGE(TAG, "OTA partitions not found");
        set_error("OTA partitions not found");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Running partition: %s", running->label);
    ESP_LOGI(TAG, "Update partition: %s", update->label);

    event_logger_add("OTA manager initialized", false);
    return ESP_OK;
}

// Check for updates
esp_err_t ota_manager_check_update(const char* current_version) {
    if (current_state != OTA_STATE_IDLE) {
        set_error("Update already in progress");
        return ESP_FAIL;
    }

    current_state = OTA_STATE_CHECKING;
    if (progress_callback) {
        progress_callback(0, current_state);
    }

    // Configure HTTP client
    esp_http_client_config_t config = {
        .url = OTA_FIRMWARE_URL "version.json",
        .timeout_ms = 5000,
        .keep_alive_enable = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        set_error("Failed to initialize HTTP client");
        current_state = OTA_STATE_ERROR;
        return ESP_FAIL;
    }

    // Get version info
    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        set_error("Failed to get version info");
        current_state = OTA_STATE_ERROR;
        http_cleanup(client);
        return ESP_FAIL;
    }

    // Parse response
    char *buffer = malloc(OTA_BUFFER_SIZE);
    int read_len = esp_http_client_read(client, buffer, OTA_BUFFER_SIZE);
    if (read_len <= 0) {
        set_error("Failed to read version info");
        current_state = OTA_STATE_ERROR;
        free(buffer);
        http_cleanup(client);
        return ESP_FAIL;
    }

    buffer[read_len] = 0;
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);

    if (!root) {
        set_error("Failed to parse version info");
        current_state = OTA_STATE_ERROR;
        http_cleanup(client);
        return ESP_FAIL;
    }

    // Compare versions
    const char *latest_version = cJSON_GetObjectItem(root, "version")->valuestring;
    bool update_available = strcmp(latest_version, current_version) > 0;

    cJSON_Delete(root);
    http_cleanup(client);

    current_state = update_available ? OTA_STATE_READY : OTA_STATE_IDLE;
    if (progress_callback) {
        progress_callback(0, current_state);
    }

    if (update_available) {
        event_logger_add_fmt("Update available: v%s", false, latest_version);
    }

    return ESP_OK;
}

// Start OTA update
esp_err_t ota_manager_start_update(ota_progress_cb_t progress_cb) {
    if (current_state != OTA_STATE_READY) {
        set_error("No update available");
        return ESP_FAIL;
    }

    progress_callback = progress_cb;
    current_state = OTA_STATE_DOWNLOADING;
    if (progress_callback) {
        progress_callback(0, current_state);
    }

    // Get running partition info
    const esp_partition_t *running = esp_ota_get_running_partition();
    char current_sha[65] = {0};
    esp_ota_get_app_elf_sha256(current_sha, sizeof(current_sha));

    // Configure HTTP client for firmware download
    esp_http_client_config_t config = {
        .url = OTA_FIRMWARE_URL "firmware.bin",
        .timeout_ms = 5000,
        .keep_alive_enable = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
        .bulk_flash_erase = false,
        .partial_http_download = true,
        .max_http_request_size = 8192,
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        set_error("Failed to begin OTA update");
        current_state = OTA_STATE_ERROR;
        return ESP_FAIL;
    }

    // Get image header and validate
    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        set_error("Failed to get firmware header");
        current_state = OTA_STATE_ERROR;
        esp_https_ota_abort(https_ota_handle);
        return ESP_FAIL;
    }

    err = validate_image_header(&app_desc);
    if (err != ESP_OK) {
        esp_https_ota_abort(https_ota_handle);
        return ESP_FAIL;
    }

    // Download firmware
    int binary_file_length = 0;
    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }

        // Get download progress
        binary_file_length = esp_https_ota_get_image_size(https_ota_handle);
        int downloaded = esp_https_ota_get_image_len_read(https_ota_handle);
        int progress = (downloaded * 100) / binary_file_length;

        if (progress_callback) {
            progress_callback(progress, current_state);
        }
    }

    if (err != ESP_OK) {
        set_error("Firmware download failed");
        current_state = OTA_STATE_ERROR;
        esp_https_ota_abort(https_ota_handle);
        return ESP_FAIL;
    }

    // Verify downloaded image
    current_state = OTA_STATE_VERIFYING;
    if (progress_callback) {
        progress_callback(100, current_state);
    }

    err = esp_https_ota_finish(https_ota_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            set_error("Firmware validation failed");
        } else {
            set_error("Failed to complete OTA update");
        }
        current_state = OTA_STATE_ERROR;
        return ESP_FAIL;
    }

    current_state = OTA_STATE_READY;
    if (progress_callback) {
        progress_callback(100, current_state);
    }

    event_logger_add("Firmware download complete", false);
    return ESP_OK;
}

// Get current OTA state
ota_state_t ota_manager_get_state(void) {
    return current_state;
}

// Get error message if state is OTA_STATE_ERROR
const char* ota_manager_get_error(void) {
    return error_message;
}

// Verify and apply update
esp_err_t ota_manager_verify_update(void) {
    if (current_state != OTA_STATE_READY) {
        set_error("No update ready to apply");
        return ESP_FAIL;
    }

    // Set boot partition
    esp_err_t err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        set_error("Failed to set boot partition");
        current_state = OTA_STATE_ERROR;
        return ESP_FAIL;
    }

    event_logger_add("Update verified, rebooting...", false);

    // Reset system to apply update
    esp_restart();
    return ESP_OK;
}

// Cancel ongoing update
esp_err_t ota_manager_cancel_update(void) {
    if (current_state == OTA_STATE_IDLE) {
        return ESP_OK;
    }

    if (update_handle) {
        esp_ota_abort(update_handle);
        update_handle = 0;
    }

    current_state = OTA_STATE_IDLE;
    if (progress_callback) {
        progress_callback(0, current_state);
    }

    event_logger_add("Update cancelled", false);
    return ESP_OK;
}

// Helper function to cleanup HTTP client
static void http_cleanup(esp_http_client_handle_t client) {
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
}

// Helper function to validate firmware header
static esp_err_t validate_image_header(esp_app_desc_t *new_app_info) {
    if (!new_app_info) {
        set_error("Invalid firmware header");
        current_state = OTA_STATE_ERROR;
        return ESP_FAIL;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;

    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
        ESP_LOGI(TAG, "New firmware version: %s", new_app_info->version);

        // Check version
        if (strcmp(new_app_info->version, running_app_info.version) <= 0) {
            set_error("New version is not newer than current version");
            current_state = OTA_STATE_ERROR;
            return ESP_FAIL;
        }
    }

    return ESP_OK;
}

// Helper function to set error message
static void set_error(const char* message) {
    strncpy(error_message, message, sizeof(error_message) - 1);
    error_message[sizeof(error_message) - 1] = '\0';
    ESP_LOGE(TAG, "%s", message);
    event_logger_add(message, true);
}
