#include "event_logger.h"
#include "ui/ui.h"
#include "esp_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "event_logger";

// Log buffer
#define MAX_LOG_ENTRIES 100
#define MAX_LOG_MESSAGE_LEN 128

typedef struct {
    char message[MAX_LOG_MESSAGE_LEN];
    bool is_alert;
} log_entry_t;

static log_entry_t log_buffer[MAX_LOG_ENTRIES];
static int log_count = 0;
static int log_next_index = 0;

// Initialize the event logger
void event_logger_init(void) {
    ESP_LOGI(TAG, "Initializing event logger");
    
    // Clear log buffer
    memset(log_buffer, 0, sizeof(log_buffer));
    log_count = 0;
    log_next_index = 0;
}

// Add a log entry
void event_logger_add(const char* message, bool is_alert) {
    if (message == NULL || strlen(message) == 0) {
        return;
    }
    
    // Log to ESP console
    if (is_alert) {
        ESP_LOGW(TAG, "ALERT: %s", message);
    } else {
        ESP_LOGI(TAG, "%s", message);
    }
    
    // Store in circular buffer
    strncpy(log_buffer[log_next_index].message, message, MAX_LOG_MESSAGE_LEN - 1);
    log_buffer[log_next_index].message[MAX_LOG_MESSAGE_LEN - 1] = '\0';
    log_buffer[log_next_index].is_alert = is_alert;
    
    // Update indices
    log_next_index = (log_next_index + 1) % MAX_LOG_ENTRIES;
    if (log_count < MAX_LOG_ENTRIES) {
        log_count++;
    }
    
    // Update UI
    ui_add_log_entry(message, is_alert);
    
    // Show alert if needed
    if (is_alert) {
        ui_show_alert("Alert", message);
    }
}

// Add a formatted log entry
void event_logger_add_fmt(const char* format, bool is_alert, ...) {
    char buffer[MAX_LOG_MESSAGE_LEN];
    va_list args;
    
    va_start(args, is_alert);
    vsnprintf(buffer, MAX_LOG_MESSAGE_LEN, format, args);
    va_end(args);
    
    event_logger_add(buffer, is_alert);
}

// Get log entry count
int event_logger_get_count(void) {
    return log_count;
}

// Clear all log entries
void event_logger_clear(void) {
    memset(log_buffer, 0, sizeof(log_buffer));
    log_count = 0;
    log_next_index = 0;
    
    ESP_LOGI(TAG, "Event log cleared");
}
