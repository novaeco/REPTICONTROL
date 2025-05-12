#include "app_main.h"
#include "drivers/display_driver.h"
#include "drivers/touch_driver.h"
#include "ui/ui.h"
#include "core/climate_controller.h"
#include "core/data_simulator.h"
#include "core/event_logger.h"
#include "core/system_monitor.h"
#include "core/settings_manager.h"
#include "utils/rtc_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "app_main";

// Forward declarations for task functions
static void ui_task(void *pvParameter);
static void sensor_simulator_task(void *pvParameter);
static void climate_control_task(void *pvParameter);
static void system_monitor_task(void *pvParameter);

// Initialization and main entry point
void repticontrol_main(void) {
    ESP_LOGI(TAG, "Initializing ReptiControl application");
    
    // Initialize components
    display_init();
    touch_init();
    rtc_init();
    settings_init();
    event_logger_init();
    climate_controller_init();
    data_simulator_init();
    system_monitor_init();
    
    // Initialize the UI (with splash screen)
    ui_init();
    
    // Create tasks
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096, NULL, 5, NULL, 1);
    xTaskCreate(sensor_simulator_task, "simulator_task", 2048, NULL, 3, NULL);
    xTaskCreate(climate_control_task, "climate_task", 2048, NULL, 4, NULL);
    xTaskCreate(system_monitor_task, "monitor_task", 2048, NULL, 2, NULL);
    
    ESP_LOGI(TAG, "ReptiControl started successfully");
}

// UI task - handles all GUI updates
static void ui_task(void *pvParameter) {
    ESP_LOGI(TAG, "UI task started");
    
    while (1) {
        // Process UI events
        ui_update();
        
        // Short delay to prevent CPU hogging
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Simulates sensor data
static void sensor_simulator_task(void *pvParameter) {
    ESP_LOGI(TAG, "Sensor simulator task started");
    
    while (1) {
        // Update simulated sensor readings
        data_simulator_update();
        
        // Delay between updates (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Climate control execution task
static void climate_control_task(void *pvParameter) {
    ESP_LOGI(TAG, "Climate control task started");
    
    while (1) {
        // Update climate controls based on current settings and schedule
        climate_controller_update();
        
        // Delay between control updates (500ms)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// System monitoring task
static void system_monitor_task(void *pvParameter) {
    ESP_LOGI(TAG, "System monitor task started");
    
    while (1) {
        // Update system status (battery, memory, etc.)
        system_monitor_update();
        
        // Delay between system updates (2 seconds)
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}