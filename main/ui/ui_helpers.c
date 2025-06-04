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
lv_style_t style_nav_item;
lv_style_t style_nav_item_active;

// Initialize all common styles
void init_styles(void) {
    ESP_LOGI(TAG, "Initializing UI styles");

    // Card style - for container panels
    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, BORDER_RADIUS);
    lv_style_set_bg_color(&style_card, COLOR_SURFACE);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 1);
    lv_style_set_border_color(&style_card, COLOR_SECONDARY);
    lv_style_set_shadow_width(&style_card, 10);
    lv_style_set_shadow_ofs_y(&style_card, 5);
    lv_style_set_shadow_opa(&style_card, LV_OPA_20);
    lv_style_set_pad_all(&style_card, CARD_PADDING);

    // Dark card style
    lv_style_init(&style_card_dark);
    lv_style_set_radius(&style_card_dark, BORDER_RADIUS);
    lv_style_set_bg_color(&style_card_dark, COLOR_PRIMARY_DARK);
    lv_style_set_bg_opa(&style_card_dark, LV_OPA_COVER);
    lv_style_set_text_color(&style_card_dark, COLOR_SURFACE);
    lv_style_set_border_width(&style_card_dark, 0);
    lv_style_set_shadow_width(&style_card_dark, 10);
    lv_style_set_shadow_ofs_y(&style_card_dark, 5);
    lv_style_set_shadow_opa(&style_card_dark, LV_OPA_30);
    lv_style_set_pad_all(&style_card_dark, CARD_PADDING);
    
    // Title style - for headings
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_PRIMARY);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_18);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);
    
    // Value style - for gauge/sensor readings
    lv_style_init(&style_value);
    lv_style_set_text_color(&style_value, COLOR_TEXT);
    lv_style_set_text_font(&style_value, &lv_font_montserrat_22);
    lv_style_set_text_align(&style_value, LV_TEXT_ALIGN_CENTER);
    
    // Alert box style
    lv_style_init(&style_alert_box);
    lv_style_set_bg_color(&style_alert_box, COLOR_SURFACE);
    lv_style_set_border_color(&style_alert_box, COLOR_PRIMARY);
    lv_style_set_border_width(&style_alert_box, 2);
    lv_style_set_radius(&style_alert_box, BORDER_RADIUS);
    lv_style_set_shadow_width(&style_alert_box, 20);
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
    lv_style_set_bg_color(&style_slider, COLOR_PRIMARY);
    lv_style_set_bg_opa(&style_slider, LV_OPA_60);
    lv_style_set_radius(&style_slider, BORDER_RADIUS / 2);
    
    // Toggle style
    lv_style_init(&style_toggle);
    lv_style_set_border_width(&style_toggle, 0);
    lv_style_set_radius(&style_toggle, BORDER_RADIUS / 2);
    
    // Button style
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, BORDER_RADIUS / 2);
    lv_style_set_bg_color(&style_btn, COLOR_PRIMARY);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn, COLOR_SURFACE);
    lv_style_set_border_width(&style_btn, 0);
    lv_style_set_pad_all(&style_btn, GRID_UNIT * 1.5);
    
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
    lv_style_set_radius(&style_badge, BORDER_RADIUS / 2);
    lv_style_set_bg_opa(&style_badge, LV_OPA_50);
    lv_style_set_text_color(&style_badge, COLOR_SURFACE);
    lv_style_set_pad_all(&style_badge, GRID_UNIT / 2);
    
    // Navigation item style
    lv_style_init(&style_nav_item);
    lv_style_set_radius(&style_nav_item, BORDER_RADIUS / 2);
    lv_style_set_bg_color(&style_nav_item, COLOR_PRIMARY);
    lv_style_set_text_color(&style_nav_item, COLOR_SURFACE);
    lv_style_set_pad_all(&style_nav_item, GRID_UNIT);
    
    // Active navigation item style
    lv_style_init(&style_nav_item_active);
    lv_style_set_bg_color(&style_nav_item_active, COLOR_PRIMARY_DARK);
    lv_style_set_text_color(&style_nav_item_active, COLOR_SURFACE);
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

// Create a badge indicator
lv_obj_t *create_badge(lv_obj_t *parent, const char *text, lv_color_t color) {
    lv_obj_t *badge = lv_obj_create(parent);
    lv_obj_set_size(badge, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_add_style(badge, &style_badge, 0);
    lv_obj_set_style_bg_color(badge, color, 0);
    
    lv_obj_t *label = lv_label_create(badge);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    
    return badge;
}

// Create a navigation item
lv_obj_t *create_nav_item(lv_obj_t *parent, const char *icon, const char *text, 
                         bool active, lv_event_cb_t event_cb) {
    lv_obj_t *nav_item = lv_btn_create(parent);
    lv_obj_set_size(nav_item, NAV_BAR_HEIGHT, NAV_BAR_HEIGHT);
    lv_obj_add_style(nav_item, active ? &style_nav_item_active : &style_nav_item, 0);
    lv_obj_add_event_cb(nav_item, event_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(nav_item);
    
    lv_obj_t *col = lv_obj_create(nav_item);
    lv_obj_set_size(col, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(col, 0, 0);
    
    lv_obj_t *icon_label = lv_label_create(col);
    lv_label_set_text(icon_label, icon);
    
    lv_obj_t *text_label = lv_label_create(col);
    lv_label_set_text(text_label, text);
    lv_obj_set_style_text_font(text_label, &lv_font_montserrat_12, 0);
    
    return nav_item;
}

// Add ripple effect to object
void add_ripple_effect(lv_obj_t *obj) {
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(obj, lv_color_white(), LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(obj, LV_OPA_20, LV_STATE_PRESSED);
    lv_obj_set_style_transition_time(obj, 300, LV_STATE_PRESSED);
    lv_obj_set_style_transition_prop_1(obj, LV_STYLE_BG_OPA, LV_STATE_PRESSED);
}
