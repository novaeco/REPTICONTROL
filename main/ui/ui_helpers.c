#include "ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_helpers";

// Declare styles
lv_style_t style_card;
lv_style_t style_title;
lv_style_t style_value;
lv_style_t style_alert_box;
lv_style_t style_panel;
lv_style_t style_slider;
lv_style_t style_toggle;
lv_style_t style_btn;
lv_style_t style_header;
lv_style_t style_text_muted;
lv_style_t style_badge;
lv_style_t style_card_dark;

// Initialize all common styles
void init_styles(void) {
    ESP_LOGI(TAG, "Initializing UI styles");

    // Card style - for container panels
    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, BORDER_RADIUS);
    lv_style_set_bg_color(&style_card, COLOR_SURFACE);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_ofs_y(&style_card, 4);
    lv_style_set_shadow_opa(&style_card, LV_OPA_20);
    lv_style_set_pad_all(&style_card, CARD_PADDING);
    
    // Dark card style
    lv_style_init(&style_card_dark);
    lv_style_copy(&style_card_dark, &style_card);
    lv_style_set_bg_color(&style_card_dark, COLOR_PRIMARY_DARK);
    lv_style_set_text_color(&style_card_dark, COLOR_SURFACE);
    
    // Title style - for headings
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_TEXT);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_18);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);
    lv_style_set_pad_bottom(&style_title, GRID_UNIT * 2);
    
    // Value style - for gauge/sensor readings
    lv_style_init(&style_value);
    lv_style_set_text_color(&style_value, COLOR_PRIMARY);
    lv_style_set_text_font(&style_value, &lv_font_montserrat_22);
    lv_style_set_text_align(&style_value, LV_TEXT_ALIGN_CENTER);
    
    // Alert box style
    lv_style_init(&style_alert_box);
    lv_style_set_bg_color(&style_alert_box, COLOR_SURFACE);
    lv_style_set_border_color(&style_alert_box, COLOR_PRIMARY);
    lv_style_set_border_width(&style_alert_box, 2);
    lv_style_set_radius(&style_alert_box, BORDER_RADIUS);
    lv_style_set_shadow_width(&style_alert_box, 30);
    lv_style_set_shadow_opa(&style_alert_box, LV_OPA_30);
    lv_style_set_pad_all(&style_alert_box, CARD_PADDING);
    
    // Panel style - for sections
    lv_style_init(&style_panel);
    lv_style_set_radius(&style_panel, BORDER_RADIUS / 2);
    lv_style_set_bg_color(&style_panel, COLOR_SURFACE);
    lv_style_set_bg_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_border_color(&style_panel, COLOR_SECONDARY);
    lv_style_set_pad_all(&style_panel, GRID_UNIT);
    
    // Slider style
    lv_style_init(&style_slider);
    lv_style_set_bg_color(&style_slider, COLOR_PRIMARY_LIGHT);
    lv_style_set_bg_opa(&style_slider, LV_OPA_40);
    lv_style_set_transition_time(&style_slider, TRANSITION_TIME);
    
    // Toggle style
    lv_style_init(&style_toggle);
    lv_style_set_border_width(&style_toggle, 0);
    lv_style_set_bg_color(&style_toggle, COLOR_PRIMARY);
    lv_style_set_transition_time(&style_toggle, TRANSITION_TIME);
    
    // Button style
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, BORDER_RADIUS / 2);
    lv_style_set_bg_color(&style_btn, COLOR_PRIMARY);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn, COLOR_SURFACE);
    lv_style_set_border_width(&style_btn, 0);
    lv_style_set_pad_all(&style_btn, GRID_UNIT * 1.5);
    lv_style_set_shadow_width(&style_btn, 10);
    lv_style_set_shadow_opa(&style_btn, LV_OPA_20);
    lv_style_set_transition_time(&style_btn, TRANSITION_TIME);
    
    // Header style
    lv_style_init(&style_header);
    lv_style_set_bg_color(&style_header, COLOR_PRIMARY);
    lv_style_set_text_color(&style_header, COLOR_SURFACE);
    lv_style_set_pad_all(&style_header, GRID_UNIT);
    lv_style_set_radius(&style_header, 0);
    
    // Muted text style
    lv_style_init(&style_text_muted);
    lv_style_set_text_color(&style_text_muted, COLOR_TEXT_SECONDARY);
    lv_style_set_text_font(&style_text_muted, &lv_font_montserrat_14);
    
    // Badge style
    lv_style_init(&style_badge);
    lv_style_set_radius(&style_badge, BORDER_RADIUS * 2);
    lv_style_set_bg_opa(&style_badge, LV_OPA_COVER);
    lv_style_set_text_color(&style_badge, COLOR_SURFACE);
    lv_style_set_pad_hor(&style_badge, GRID_UNIT);
    lv_style_set_pad_ver(&style_badge, GRID_UNIT / 2);
}

// Create a card container with title
lv_obj_t *create_card(lv_obj_t *parent, const char *title, int width, int height) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, width, height);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    
    if (title && strlen(title) > 0) {
        lv_obj_t *title_label = lv_label_create(card);
        lv_label_set_text(title_label, title);
        lv_obj_add_style(title_label, &style_title, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, GRID_UNIT);
    }
    
    // Add ripple effect
    add_ripple_effect(card);
    
    return card;
}

// Create a gauge with label
lv_obj_t *create_gauge(lv_obj_t *parent, const char *title, const char *unit, 
                      int min, int max, lv_color_t color) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_size(panel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t *title_label = lv_label_create(panel);
    lv_label_set_text(title_label, title);
    lv_obj_add_style(title_label, &style_title, 0);
    
    lv_obj_t *gauge = lv_meter_create(panel);
    lv_obj_set_size(gauge, TOUCH_TARGET_MIN * 2.5, TOUCH_TARGET_MIN * 2.5);
    
    lv_meter_scale_t *scale = lv_meter_add_scale(gauge);
    lv_meter_set_scale_ticks(gauge, scale, 41, 2, 10, color);
    lv_meter_set_scale_range(gauge, scale, min, max, 270, 135);
    
    lv_meter_indicator_t *indic = lv_meter_add_arc(gauge, scale, 10, color, 0);
    lv_meter_set_indicator_value(gauge, indic, min);
    
    lv_obj_t *value_label = lv_label_create(panel);
    lv_label_set_text_fmt(value_label, "%d %s", min, unit);
    lv_obj_add_style(value_label, &style_value, 0);
    
    return gauge;
}

// Create a slider with label
lv_obj_t *create_slider(lv_obj_t *parent, const char *title, int min, int max, 
                       int value, lv_event_cb_t event_cb) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(panel, GRID_UNIT, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    
    lv_obj_t *label = lv_label_create(panel);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t *value_label = lv_label_create(panel);
    lv_label_set_text_fmt(value_label, "%d", value);
    lv_obj_align(value_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(value_label, &style_value, 0);
    
    lv_obj_t *slider = lv_slider_create(panel);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    lv_obj_set_width(slider, LV_PCT(100));
    lv_obj_add_style(slider, &style_slider, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_slider, LV_PART_INDICATOR);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN);
    
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, value_label);
    
    return slider;
}

// Create a toggle switch with label
lv_obj_t *create_toggle(lv_obj_t *parent, const char *title, bool state, 
                       lv_event_cb_t event_cb) {
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_size(panel, LV_PCT(100), TOUCH_TARGET_MIN);
    lv_obj_set_style_pad_all(panel, GRID_UNIT, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    
    lv_obj_t *label = lv_label_create(panel);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
    
    lv_obj_t *toggle = lv_switch_create(panel);
    lv_obj_add_style(toggle, &style_toggle, 0);
    lv_obj_set_size(toggle, TOUCH_TARGET_MIN, TOUCH_TARGET_MIN / 2);
    lv_obj_align(toggle, LV_ALIGN_RIGHT_MID, 0, 0);
    
    if (state) {
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle, LV_STATE_CHECKED);
    }
    
    lv_obj_add_event_cb(toggle, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    return toggle;
}

// Create a badge indicator
lv_obj_t *create_badge(lv_obj_t *parent, const char *text, lv_color_t color) {
    lv_obj_t *badge = lv_obj_create(parent);
    lv_obj_add_style(badge, &style_badge, 0);
    lv_obj_set_style_bg_color(badge, color, 0);
    
    lv_obj_t *label = lv_label_create(badge);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    
    return badge;
}

// Add ripple effect to object
void add_ripple_effect(lv_obj_t *obj) {
    static lv_style_t style_ripple;
    static bool ripple_style_initialized = false;
    
    if (!ripple_style_initialized) {
        lv_style_init(&style_ripple);
        lv_style_set_bg_opa(&style_ripple, LV_OPA_COVER);
        lv_style_set_bg_color(&style_ripple, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&style_ripple, LV_OPA_20);
        ripple_style_initialized = true;
    }
    
    lv_obj_add_style(obj, &style_ripple, LV_STATE_PRESSED);
}