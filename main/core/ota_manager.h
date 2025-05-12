```c
#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

// OTA update states
typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_CHECKING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_VERIFYING,
    OTA_STATE_READY,
    OTA_STATE_ERROR
} ota_state_t;

// OTA update progress callback
typedef void (*ota_progress_cb_t)(int progress, ota_state_t state);

// Initialize OTA manager
esp_err_t ota_manager_init(void);

// Check for updates
esp_err_t ota_manager_check_update(const char* current_version);

// Start OTA update
esp_err_t ota_manager_start_update(ota_progress_cb_t progress_cb);

// Get current OTA state
ota_state_t ota_manager_get_state(void);

// Get error message if state is OTA_STATE_ERROR
const char* ota_manager_get_error(void);

// Verify and apply update
esp_err_t ota_manager_verify_update(void);

// Cancel ongoing update
esp_err_t ota_manager_cancel_update(void);

#endif /* OTA_MANAGER_H */
```