#include "animations.h"
#include "esp_log.h"

static const char *TAG = "animations";

// Screen transition callback for slide animation
static void screen_anim_cb(void *var, int32_t value) {
    lv_obj_t *scr = (lv_obj_t *)var;
    lv_obj_set_x(scr, value);
}

// Screen transition ready callback
static void screen_anim_ready_cb(lv_anim_t *a) {
    lv_obj_t *scr_to = (lv_obj_t *)a->var;
    lv_obj_t *scr_from = (lv_obj_t *)lv_obj_get_user_data(scr_to);
    
    // Load the new screen
    lv_scr_load(scr_to);
    
    // Reset position of both screens
    lv_obj_set_x(scr_to, 0);
    lv_obj_set_x(scr_from, 0);
    
    // Clear temporary user data
    lv_obj_set_user_data(scr_to, NULL);
}

// Animate screen change with slide effect
void animate_screen_change(lv_obj_t *scr_from, lv_obj_t *scr_to, bool slide_left) {
    // Get screen width
    lv_coord_t width = lv_obj_get_width(lv_scr_act());
    
    // Set initial positions
    lv_obj_set_x(scr_to, slide_left ? width : -width);
    lv_obj_set_x(scr_from, 0);
    
    // Make destination screen visible without loading it
    lv_obj_set_style_opa(scr_to, LV_OPA_COVER, 0);
    
    // Store 'from' screen pointer in 'to' screen's user data for cleanup
    lv_obj_set_user_data(scr_to, scr_from);
    
    // Create animation for outgoing screen
    lv_anim_t a_out;
    lv_anim_init(&a_out);
    lv_anim_set_var(&a_out, scr_from);
    lv_anim_set_time(&a_out, 300);
    lv_anim_set_path_cb(&a_out, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a_out, screen_anim_cb);
    lv_anim_set_values(&a_out, 0, slide_left ? -width : width);
    
    // Create animation for incoming screen
    lv_anim_t a_in;
    lv_anim_init(&a_in);
    lv_anim_set_var(&a_in, scr_to);
    lv_anim_set_time(&a_in, 300);
    lv_anim_set_path_cb(&a_in, lv_anim_path_ease_out);
    lv_anim_set_exec_cb(&a_in, screen_anim_cb);
    lv_anim_set_values(&a_in, slide_left ? width : -width, 0);
    lv_anim_set_ready_cb(&a_in, screen_anim_ready_cb);
    
    // Start animations
    lv_anim_start(&a_out);
    lv_anim_start(&a_in);
}

// Gauge value animation callback
static void gauge_anim_cb(void *var, int32_t value) {
    lv_obj_t *gauge = (lv_obj_t *)var;
    lv_meter_set_indicator_value(gauge, lv_meter_get_indicator(gauge, 0), value);
}

// Animate value change on gauge
void animate_gauge_value(lv_obj_t *gauge, int start_value, int end_value, int duration_ms) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, gauge);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_set_values(&a, start_value, end_value);
    lv_anim_set_exec_cb(&a, gauge_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

// Pulse animation callback
static void pulse_anim_cb(void *var, int32_t value) {
    lv_obj_t *obj = (lv_obj_t *)var;
    lv_obj_set_style_opa(obj, value, 0);
}

// Pulse animation for alert indicators
void animate_pulse(lv_obj_t *obj, int duration_ms) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_set_values(&a, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, pulse_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_playback_time(&a, duration_ms);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}
