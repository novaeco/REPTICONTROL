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

// Initialize all common styles
void init_styles(void) {
    ESP_LOGI(TAG, "Initializing UI styles");

    // Card style - for container panels
    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, 8);
    lv_style_set_bg_color(&style_card, COLOR_SECONDARY);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_shadow_width(&style_card, 10);
    lv_style_set_shadow_ofs_y(&style_card, 5);
    lv_style_set_shadow_opa(&style_card, LV_OPA_20);
    lv_style_set_pad_all(&style_card, 16);
    
    // Title style - for headings
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_PRIMARY);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_18);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);
    
    // Value style - for gauge/sensor readings
    lv_style_init(&style_value);
    lv_style_set_text_color(&style_value, COLOR_NEUTRAL);
    lv_style_set_text_font(&style_value, &lv_font_montserrat_22);
    lv_style_set_text_align(&style_value, LV_TEXT_ALIGN_CENTER);
    
    // Alert box style
    lv_style_init(&style_alert_box);
    lv_style_set_bg_color(&style_alert_box, lv_color_white());
    lv_style_set_border_color(&style_alert_box, COLOR_PRIMARY);
    lv_style_set_border_width(&style_alert_box, 2);
    lv_style_set_radius(&style_alert_box, 8);
    lv_style_set_shadow_width(&style_alert_box, 20);
    lv_style_set_shadow_opa(&style_alert_box, LV_OPA_30);
    
    // Panel style - for sections
    lv_style_init(&style_panel);
    lv_style_set_radius(&style_panel, 4);
    lv_style_set_bg_color(&style_panel, lv_color_white());
    lv_style_set_bg_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_border_color(&style_panel, lv_color_hex(0xE0E0E0)); // Light gray
    lv_style_set_pad_all(&style_panel, 8);
    
    // Slider style
    lv_style_init(&style_slider);
    lv_style_set_bg_color(&style_slider, COLOR_SECONDARY);
    lv_style_set_bg_opa(&style_slider, LV_OPA_60);
    
    // Toggle style
    lv_style_init(&style_toggle);
    lv_style_set_border_width(&style_toggle, 0);
    
    // Button style
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 4);
    lv_style_set_bg_color(&style_btn, COLOR_PRIMARY);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn, lv_color_white());
    lv_style_set_border_width(&style_btn, 0);
    lv_style_set_pad_all(&style_btn, 12);
    
    // Header style
    lv_style_init(&style_header);
    lv_style_set_bg_color(&style_header, COLOR_PRIMARY);
    lv_style_set_text_color(&style_header, lv_color_white());
    lv_style_set_pad_all(&style_header, 8);
    lv_style_set_radius(&style_header, 0);
}

// Create a card container with title
lv_obj_t *create_card(lv_obj_t *parent, const char *title, int width, int height) {
    // Create the container
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_size(card, width, height);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add title at the top if provided
    if (title && strlen(title) > 0) {
        lv_obj_t *title_label = lv_label_create(card);
        lv_label_set_text(title_label, title);
        lv_obj_add_style(title_label, &style_title, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);
    }
    
    return card;
}

// Create a gauge with label
lv_obj_t *create_gauge(lv_obj_t *parent, const char *title, const char *unit, 
                      int min, int max, lv_color_t color) {
    // Create containing panel
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_size(panel, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create title label
    lv_obj_t *title_label = lv_label_create(panel);
    lv_label_set_text(title_label, title);
    lv_obj_add_style(title_label, &style_title, 0);
    
    // Create gauge
    lv_obj_t *gauge = lv_meter_create(panel);
    lv_obj_set_size(gauge, 120, 120);
    
    // Add scale and arcs to gauge
    lv_meter_scale_t *scale = lv_meter_add_scale(gauge);
    lv_meter_set_scale_ticks(gauge, scale, 41, 2, 10, color);
    lv_meter_set_scale_range(gauge, scale, min, max, 270, 135);
    
    // Add a special arc
    lv_meter_indicator_t *indic = lv_meter_add_arc(gauge, scale, 10, color, 0);
    lv_meter_set_indicator_value(gauge, indic, min);
    
    // Create value label with unit
    lv_obj_t *value_label = lv_label_create(panel);
    lv_label_set_text_fmt(value_label, "%d %s", min, unit);
    lv_obj_add_style(value_label, &style_value, 0);
    
    return gauge;
}

// Create a slider with label
lv_obj_t *create_slider(lv_obj_t *parent, const char *title, int min, int max, 
                       int value, lv_event_cb_t event_cb) {
    // Create panel to group slider with labels
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(panel, 8, 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    
    // Create title label
    lv_obj_t *label = lv_label_create(panel);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // Create value label (will be updated by slider)
    lv_obj_t *value_label = lv_label_create(panel);
    lv_label_set_text_fmt(value_label, "%d", value);
    lv_obj_align(value_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    // Create slider
    lv_obj_t *slider = lv_slider_create(panel);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    lv_obj_set_width(slider, LV_PCT(100));
    lv_obj_add_style(slider, &style_slider, LV_PART_MAIN);
    lv_obj_add_style(slider, &style_slider, LV_PART_INDICATOR);
    lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 30);
    
    // Set up value change event
    lv_obj_add_event_cb(slider, event_cb, LV_EVENT_VALUE_CHANGED, value_label);
    
    return slider;
}

// Create a toggle switch with label
lv_obj_t *create_toggle(lv_obj_t *parent, const char *title, bool state, 
                       lv_event_cb_t event_cb) {
    // Create containing panel
    lv_obj_t *panel = lv_obj_create(parent);
    lv_obj_set_size(panel, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(panel, 8, 0);
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    
    // Create the label
    lv_obj_t *label = lv_label_create(panel);
    lv_label_set_text(label, title);
    lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
    
    // Create the switch
    lv_obj_t *toggle = lv_switch_create(panel);
    lv_obj_add_style(toggle, &style_toggle, 0);
    lv_obj_align(toggle, LV_ALIGN_RIGHT_MID, 0, 0);
    
    // Set initial state
    if (state) {
        lv_obj_add_state(toggle, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle, LV_STATE_CHECKED);
    }
    
    // Add event handler
    lv_obj_add_event_cb(toggle, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    return toggle;
}