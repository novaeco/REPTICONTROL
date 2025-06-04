#include "display_config.h"
#include "settings_manager.h"

// Default settings for each display
#define H_RES_5_INCH 1024
#define V_RES_5_INCH 600
#define CLK_5_INCH   (18 * 1000 * 1000)

#define H_RES_7_INCH 800
#define V_RES_7_INCH 480
#define CLK_7_INCH   (16 * 1000 * 1000)

// Extern variables used by display_driver
int LCD_H_RES = H_RES_5_INCH;
int LCD_V_RES = V_RES_5_INCH;
int LCD_PIXEL_CLOCK_HZ = CLK_5_INCH;

static display_type_t current_type = DISPLAY_5_INCH;

__attribute__((constructor)) static void load_display_type(void) {
    int stored = settings_get_int("display_type", (int)DISPLAY_5_INCH);
    display_config_apply((display_type_t)stored);
}

display_type_t display_config_get_type(void) {
    return current_type;
}

void display_config_set_type(display_type_t type) {
    current_type = type;
    settings_set_int("display_type", (int)type);
    settings_save();
}

void display_config_apply(display_type_t type) {
    current_type = type;
    if (type == DISPLAY_7_INCH) {
        LCD_H_RES = H_RES_7_INCH;
        LCD_V_RES = V_RES_7_INCH;
        LCD_PIXEL_CLOCK_HZ = CLK_7_INCH;
    } else {
        LCD_H_RES = H_RES_5_INCH;
        LCD_V_RES = V_RES_5_INCH;
        LCD_PIXEL_CLOCK_HZ = CLK_5_INCH;
    }
}
