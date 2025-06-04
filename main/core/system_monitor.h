#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

// Initialize the system monitor
void system_monitor_init(void);

// Update system status
void system_monitor_update(void);

// Get current battery level (percentage)
int system_monitor_get_battery_level(void);

// Get current CPU usage (percentage)
int system_monitor_get_cpu_usage(void);

// Get current memory usage (percentage)
int system_monitor_get_memory_usage(void);

// Simulate a system alert
void system_monitor_trigger_alert(const char* message);

#endif /* SYSTEM_MONITOR_H */
