#include "watchdog_manager.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "event_logger.h"
#include <string.h>

static const char *TAG = "watchdog_manager";

// Maximum number of monitored tasks
#define MAX_MONITORED_TASKS 10

// Task monitoring structure
typedef struct {
    char name[32];
    uint32_t timeout_ms;
    uint64_t last_feed;
    bool enabled;
    watchdog_state_t state;
} monitored_task_t;

// Task monitoring array
static monitored_task_t monitored_tasks[MAX_MONITORED_TASKS];
static int task_count = 0;

// Timer handle for watchdog checks
static esp_timer_handle_t watchdog_timer;

// Forward declarations
static void watchdog_timer_callback(void* arg);

// Initialize watchdog manager
esp_err_t watchdog_manager_init(void) {
    ESP_LOGI(TAG, "Initializing watchdog manager");
    
    // Initialize task watchdog
    ESP_ERROR_CHECK(esp_task_wdt_init(5, true));
    
    // Create periodic timer for watchdog checks
    const esp_timer_create_args_t timer_args = {
        .callback = &watchdog_timer_callback,
        .name = "watchdog_timer"
    };
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &watchdog_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(watchdog_timer, 1000000)); // 1 second interval
    
    // Clear task array
    memset(monitored_tasks, 0, sizeof(monitored_tasks));
    task_count = 0;
    
    event_logger_add("Watchdog manager initialized", false);
    return ESP_OK;
}

// Register a task for monitoring
esp_err_t watchdog_manager_register_task(const char* task_name, uint32_t timeout_ms) {
    if (task_count >= MAX_MONITORED_TASKS) {
        ESP_LOGE(TAG, "Maximum number of monitored tasks reached");
        return ESP_FAIL;
    }
    
    // Check for duplicate task
    for (int i = 0; i < task_count; i++) {
        if (strcmp(monitored_tasks[i].name, task_name) == 0) {
            ESP_LOGE(TAG, "Task %s already registered", task_name);
            return ESP_FAIL;
        }
    }
    
    // Add new task
    monitored_task_t *task = &monitored_tasks[task_count];
    strncpy(task->name, task_name, sizeof(task->name) - 1);
    task->timeout_ms = timeout_ms;
    task->last_feed = esp_timer_get_time() / 1000;
    task->enabled = true;
    task->state = WATCHDOG_OK;
    
    task_count++;
    
    ESP_LOGI(TAG, "Registered task %s with timeout %d ms", task_name, timeout_ms);
    return ESP_OK;
}

// Feed watchdog for a task
esp_err_t watchdog_manager_feed(const char* task_name) {
    for (int i = 0; i < task_count; i++) {
        if (strcmp(monitored_tasks[i].name, task_name) == 0) {
            if (!monitored_tasks[i].enabled) {
                return ESP_OK;
            }
            
            monitored_tasks[i].last_feed = esp_timer_get_time() / 1000;
            monitored_tasks[i].state = WATCHDOG_OK;
            return ESP_OK;
        }
    }
    
    ESP_LOGW(TAG, "Task %s not found", task_name);
    return ESP_FAIL;
}

// Get watchdog state for a task
watchdog_state_t watchdog_manager_get_state(const char* task_name) {
    for (int i = 0; i < task_count; i++) {
        if (strcmp(monitored_tasks[i].name, task_name) == 0) {
            return monitored_tasks[i].state;
        }
    }
    
    return WATCHDOG_OK;
}

// Enable/disable watchdog for a task
esp_err_t watchdog_manager_set_enabled(const char* task_name, bool enable) {
    for (int i = 0; i < task_count; i++) {
        if (strcmp(monitored_tasks[i].name, task_name) == 0) {
            monitored_tasks[i].enabled = enable;
            if (enable) {
                monitored_tasks[i].last_feed = esp_timer_get_time() / 1000;
                monitored_tasks[i].state = WATCHDOG_OK;
            }
            return ESP_OK;
        }
    }
    
    ESP_LOGW(TAG, "Task %s not found", task_name);
    return ESP_FAIL;
}

// Timer callback for watchdog checks
static void watchdog_timer_callback(void* arg) {
    uint64_t current_time = esp_timer_get_time() / 1000;
    
    for (int i = 0; i < task_count; i++) {
        monitored_task_t *task = &monitored_tasks[i];
        
        if (!task->enabled) {
            continue;
        }
        
        uint64_t elapsed = current_time - task->last_feed;
        
        // Check for warning threshold (80% of timeout)
        if (elapsed >= (task->timeout_ms * 8 / 10)) {
            if (task->state == WATCHDOG_OK) {
                task->state = WATCHDOG_WARNING;
                event_logger_add_fmt("WARNING: Task %s approaching watchdog timeout", 
                                   true, task->name);
            }
        }
        
        // Check for timeout
        if (elapsed >= task->timeout_ms) {
            if (task->state != WATCHDOG_TIMEOUT) {
                task->state = WATCHDOG_TIMEOUT;
                event_logger_add_fmt("ALERT: Task %s watchdog timeout!", true, task->name);
                
                // In a real system, this would trigger a recovery action
                // For simulation, we just log the event
            }
        }
    }
}
