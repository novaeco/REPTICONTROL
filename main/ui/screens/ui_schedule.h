#ifndef UI_SCHEDULE_H
#define UI_SCHEDULE_H

#include "lvgl.h"

// Create the schedule screen
lv_obj_t *ui_schedule_create(void);

// Add a scheduled event to the calendar
void ui_schedule_add_event(int day, int hour, int minute,
                        float temperature, float humidity, float light);

// Remove a scheduled event from the calendar
void ui_schedule_remove_event(int day, int hour, int minute);

#endif /* UI_SCHEDULE_H */
