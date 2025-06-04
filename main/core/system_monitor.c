#include "system_monitor.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "event_logger.h"
#include "ui/ui.h"
#include "climate_controller.h"
#include <stdlib.h>
#include <time.h>

static const char *TAG = "system_monitor";

// Simulated values
static int battery_level = 100;
static int cpu_usage = 0;
static int memory_usage = 0;
static time_t last_update_time = 0;

// Discharge rate (percent per hour)
static const float BATTERY_DISCHARGE_RATE = 5.0f;

// Initialize the system monitor
void system_monitor_init(void) {
    ESP_LOGI(TAG, "Initializing system monitor");
    
    // Set initial values
    battery_level = 100;
    cpu_usage = 0;
    memory_usage = 0;
    last_update_time = time(NULL);
    
    // Seed random for simulation
    srand(time(NULL));
    
    event_logger_add("System monitor initialized", false);
}

// Update system status
void system_monitor_update(void) {
    // Get actual memory usage
    size_t free_heap = esp_get_free_heap_size();
    size_t total_heap = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    memory_usage = (int)((total_heap - free_heap) * 100 / total_heap);
    
    // Simulate CPU usage
    cpu_usage = 15 + rand() % 20;  // Base load of 15-35%
    
    // Add additional CPU load if climate systems are active
    if (climate_controller_is_heating_on()) cpu_usage += 10;
    if (climate_controller_is_cooling_on()) cpu_usage += 10;
    if (climate_controller_is_humidifier_on()) cpu_usage += 5;
    if (climate_controller_is_lighting_on()) cpu_usage += 5;
    
    // Add minor random fluctuations
    cpu_usage += (rand() % 5) - 2;  // -2 to +2 range
    
    // Ensure valid range
    if (cpu_usage < 0) cpu_usage = 0;
    if (cpu_usage > 100) cpu_usage = 100;
    
    // Simulate battery discharge
    time_t now = time(NULL);
    float hours_elapsed = difftime(now, last_update_time) / 3600.0f;
    last_update_time = now;
    
    // For simulation purposes, drain faster
    float discharge = BATTERY_DISCHARGE_RATE * hours_elapsed;
    battery_level -= (int)discharge;
    
    // Add small random variations
    battery_level += (rand() % 3) - 1;  // -1 to +1 range
    
    // Ensure valid range
    if (battery_level < 0) battery_level = 0;
    if (battery_level > 100) battery_level = 100;
    
    // Update UI with system stats
    ui_update_system_status(battery_level, 
                          climate_controller_is_heating_on(),
                          climate_controller_is_cooling_on(),
                          climate_controller_is_humidifier_on(),
                          climate_controller_is_lighting_on());
    
    ui_system_update_stats(cpu_usage, memory_usage);
    
    // Generate alerts for low battery
    if (battery_level == 20 || battery_level == 10 || battery_level == 5) {
        char alert_msg[64];
        snprintf(alert_msg, sizeof(alert_msg), "Low battery: %d%% remaining", battery_level);
        system_monitor_trigger_alert(alert_msg);
    }
    
    // Randomly generate system events for demonstration
    if (rand() % 300 == 0) {  // Random occasional events
        const char* random_events[] = {
            "WiFi connection established",
            "Network time synchronized",
            "Daily data backup complete",
            "Memory optimization performed",
            "Background calibration complete"
        };
        int event_idx = rand() % 5;
        event_logger_add(random_events[event_idx], false);
    }
}

// Get current battery level (percentage)
int system_monitor_get_battery_level(void) {
    return battery_level;
}

// Get current CPU usage (percentage)
int system_monitor_get_cpu_usage(void) {
    return cpu_usage;
}

// Get current memory usage (percentage)
int system_monitor_get_memory_usage(void) {
    return memory_usage;
}

// Simulate a system alert
void system_monitor_trigger_alert(const char* message) {
    event_logger_add(message, true);
    
    // Log to ESP console
    ESP_LOGW(TAG, "SYSTEM ALERT: %s", message);
}
