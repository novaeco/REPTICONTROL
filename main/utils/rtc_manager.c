#include "rtc_manager.h"
#include "esp_log.h"
#include "esp_system.h"
#include <sys/time.h>
#include <time.h>

static const char *TAG = "rtc_manager";

// Initialize RTC
void rtc_init(void) {
    ESP_LOGI(TAG, "Initializing RTC manager");
    
    // Set system time to a default value (2025-01-01 12:00:00)
    struct tm default_time;
    default_time.tm_year = 2025 - 1900;
    default_time.tm_mon = 0;      // January
    default_time.tm_mday = 1;     // 1st
    default_time.tm_hour = 12;    // 12:00
    default_time.tm_min = 0;
    default_time.tm_sec = 0;
    
    rtc_set_time(&default_time);
    
    ESP_LOGI(TAG, "RTC initialized and set to default time");
}

// Get current time
void rtc_get_time(struct tm *time_info) {
    time_t now;
    time(&now);
    localtime_r(&now, time_info);
}

// Set current time
void rtc_set_time(const struct tm *time_info) {
    struct timeval tv;
    tv.tv_sec = mktime((struct tm *)time_info);
    tv.tv_usec = 0;
    
    settimeofday(&tv, NULL);
    
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);
    ESP_LOGI(TAG, "System time set to: %s", time_str);
}
