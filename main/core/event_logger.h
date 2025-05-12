#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <stdbool.h>

// Initialize the event logger
void event_logger_init(void);

// Add a log entry
void event_logger_add(const char* message, bool is_alert);

// Add a formatted log entry
void event_logger_add_fmt(const char* format, bool is_alert, ...);

// Get log entry count
int event_logger_get_count(void);

// Clear all log entries
void event_logger_clear(void);

#endif /* EVENT_LOGGER_H */