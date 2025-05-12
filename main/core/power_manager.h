```c
/**
 * @file power_manager.h
 * @brief Power management and battery monitoring system
 */

#ifndef CORE_POWER_MANAGER_H
#define CORE_POWER_MANAGER_H

#include <stdbool.h>
#include "esp_err.h"

// Battery states
typedef enum {
    BATTERY_STATE_NORMAL,
    BATTERY_STATE_LOW,
    BATTERY_STATE_CRITICAL,
    BATTERY_STATE_CHARGING,
    BATTERY_STATE_FULL
} battery_state_t;

// Power modes
typedef enum {
    POWER_MODE_NORMAL,
    POWER_MODE_ECO,
    POWER_MODE_SLEEP
} power_mode_t;

/**
 * @brief Initialize power management system
 * @return ESP_OK on success
 */
esp_err_t power_manager_init(void);

/**
 * @brief Get current battery percentage
 * @return Battery level (0-100)
 */
int power_manager_get_battery_level(void);

/**
 * @brief Get current battery state
 * @return Current battery state
 */
battery_state_t power_manager_get_battery_state(void);

/**
 * @brief Get battery voltage in millivolts
 * @return Battery voltage
 */
uint32_t power_manager_get_battery_voltage(void);

/**
 * @brief Check if device is currently charging
 * @return true if charging
 */
bool power_manager_is_charging(void);

/**
 * @brief Set power mode
 * @param mode Power mode to set
 * @return ESP_OK on success
 */
esp_err_t power_manager_set_mode(power_mode_t mode);

/**
 * @brief Enter light sleep mode
 * @param sleep_ms Time to sleep in milliseconds
 * @return ESP_OK on success
 */
esp_err_t power_manager_light_sleep(uint32_t sleep_ms);

/**
 * @brief Update power management status
 * Should be called periodically
 */
void power_manager_update(void);

#endif /* CORE_POWER_MANAGER_H */
```