#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include "lvgl.h"

// Common styles
extern lv_style_t style_card;
extern lv_style_t style_title;
extern lv_style_t style_value;
extern lv_style_t style_alert_box;
extern lv_style_t style_panel;
extern lv_style_t style_slider;
extern lv_style_t style_toggle;
extern lv_style_t style_btn;
extern lv_style_t style_header;
extern lv_style_t style_text_muted;
extern lv_style_t style_badge;
extern lv_style_t style_card_dark;
extern lv_style_t style_nav_item;
extern lv_style_t style_nav_item_active;

// ReptiControl Design System Colors
#define COLOR_PRIMARY        lv_color_hex(0x2E7D32)  // Forest green
#define COLOR_PRIMARY_DARK   lv_color_hex(0x1B5E20)  // Dark green
#define COLOR_PRIMARY_LIGHT  lv_color_hex(0x4CAF50)  // Light green
#define COLOR_SECONDARY      lv_color_hex(0xD7CCC8)  // Warm gray
#define COLOR_ACCENT         lv_color_hex(0xA1887F)  // Terracotta
#define COLOR_BACKGROUND     lv_color_hex(0xF5F5F5)  // Light gray
#define COLOR_SURFACE       lv_color_hex(0xFFFFFF)  // White
#define COLOR_SUCCESS       lv_color_hex(0x4CAF50)  // Green
#define COLOR_WARNING       lv_color_hex(0xFF9800)  // Orange
#define COLOR_ERROR         lv_color_hex(0xF44336)  // Red
#define COLOR_INFO          lv_color_hex(0x2196F3)  // Blue
#define COLOR_NEUTRAL       lv_color_hex(0x757575)  // Gray
#define COLOR_TEXT          lv_color_hex(0x212121)  // Dark gray
#define COLOR_TEXT_SECONDARY lv_color_hex(0x757575)  // Medium gray

// Layout constants
#define GRID_UNIT           8  // Base unit for spacing
#define CARD_PADDING        (2 * GRID_UNIT)  // 16px
#define SCREEN_PADDING      (2 * GRID_UNIT)  // 16px
#define TOUCH_TARGET_MIN    48  // Minimum touch target size
#define BORDER_RADIUS       (GRID_UNIT)  // 8px
#define TRANSITION_TIME     300  // Animation duration in ms
#define NAV_BAR_HEIGHT     (TOUCH_TARGET_MIN * 1.5)  // Navigation bar height

// Initialize all styles
void init_styles(void);

// Create a card container with title
lv_obj_t *create_card(lv_obj_t *parent, const char *title, int width, int height);

// Create a gauge with label
lv_obj_t *create_gauge(lv_obj_t *parent, const char *title, const char *unit, 
                      int min, int max, lv_color_t color);

// Create a slider with label
lv_obj_t *create_slider(lv_obj_t *parent, const char *title, int min, int max, 
                       int value, lv_event_cb_t event_cb);

// Create a toggle switch with label
lv_obj_t *create_toggle(lv_obj_t *parent, const char *title, bool state, 
                       lv_event_cb_t event_cb);

// Create a badge indicator
lv_obj_t *create_badge(lv_obj_t *parent, const char *text, lv_color_t color);

// Create a navigation item
lv_obj_t *create_nav_item(lv_obj_t *parent, const char *icon, const char *text, 
                         bool active, lv_event_cb_t event_cb);

// Create a ripple animation on object
void add_ripple_effect(lv_obj_t *obj);

#endif /* UI_HELPERS_H */