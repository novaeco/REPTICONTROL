#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "lvgl.h"

// Animate screen change with slide effect
void animate_screen_change(lv_obj_t *scr_from, lv_obj_t *scr_to, bool slide_left);

// Animate value change on gauge
void animate_gauge_value(lv_obj_t *gauge, int start_value, int end_value, int duration_ms);

// Pulse animation for alert indicators
void animate_pulse(lv_obj_t *obj, int duration_ms);

#endif /* ANIMATIONS_H */
