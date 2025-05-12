#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <time.h>

// Initialize RTC
void rtc_init(void);

// Get current time
void rtc_get_time(struct tm *time_info);

// Set current time
void rtc_set_time(const struct tm *time_info);

#endif /* RTC_MANAGER_H */