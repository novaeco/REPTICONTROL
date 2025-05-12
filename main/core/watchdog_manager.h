/**
 * @file watchdog_manager.h
 * @brief System watchdog management for fault detection and recovery
 */

#ifndef CORE_WATCHDOG_MANAGER_H
#define CORE_WATCHDOG_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

// Task watchdog states
typedef enum {
    WATCHDOG_OK,
    WATCHDOG_WARNING,
    WATCHDOG_TIMEOUT
} watchdog_state_t;

/**
 * @brief Initialize watchdog manager
 * @return ESP_OK on success
 */
esp_err_t watchdog_manager_init(void);

/**
 * @brief Register a task for watchdog monitoring
 * @param task_name Name of the task to monitor
 * @param timeout_ms Timeout in milliseconds
 * @return ESP_OK on success
 */
esp_err_t watchdog_manager_register_task(const char* task_name, uint32_t timeout_ms);

/**
 * @brief Feed the watchdog for a specific task
 * @param task_name Name of the task
 * @return ESP_OK on success
 */
esp_err_t watchdog_manager_feed(const char* task_name);

/**
 * @brief Get current watchdog state for a task
 * @param task_name Name of the task
 * @return Current watchdog state
 */
watchdog_state_t watchdog_manager_get_state(const char* task_name);

/**
 * @brief Enable/disable watchdog for a task
 * @param task_name Name of the task
 * @param enable true to enable, false to disable
 * @return ESP_OK on success
 */
esp_err_t watchdog_manager_set_enabled(const char* task_name, bool enable);

#endif /* CORE_WATCHDOG_MANAGER_H */