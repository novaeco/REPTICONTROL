/**
 * @file lv_conf.h
 * @brief LVGL configuration for ReptiControl
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

// Color depth
#define LV_COLOR_DEPTH 16

// Memory size settings
#define LV_MEM_CUSTOM 1
#define LV_MEM_SIZE (48U * 1024U)
#define LV_MEM_POOL_INCLUDE <stdlib.h>
#define LV_MEM_POOL_ALLOC malloc
#define LV_MEM_POOL_FREE free

// Screen resolution
#define LV_HOR_RES_MAX 1024
#define LV_VER_RES_MAX 600

// DPI settings
#define LV_DPI_DEF 130

// Enable features
#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR 1
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_INFO
#define LV_LOG_PRINTF 1

// Widget usage
#define LV_USE_ANIMIMG 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CALENDAR 1
#define LV_USE_CANVAS 1
#define LV_USE_CHART 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMG 1
#define LV_USE_LABEL 1
#define LV_USE_LED 1
#define LV_USE_LINE 1
#define LV_USE_LIST 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_ROLLER 1
#define LV_USE_SLIDER 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SWITCH 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1

// Theme configuration
#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 0
#define LV_THEME_DEFAULT_GROW 1
#define LV_THEME_DEFAULT_TRANSITION_TIME 80

// Font configuration
#define LV_FONT_MONTSERRAT_8 1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 1
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_38 1
#define LV_FONT_MONTSERRAT_40 1
#define LV_FONT_MONTSERRAT_42 1
#define LV_FONT_MONTSERRAT_44 1
#define LV_FONT_MONTSERRAT_46 1
#define LV_FONT_MONTSERRAT_48 1

// Default font
#define LV_FONT_DEFAULT &lv_font_montserrat_14

// Enable GPU
#define LV_USE_GPU_ESP32_DRAW_UNIT 1

// Input device settings
#define LV_INDEV_DEF_READ_PERIOD 30
#define LV_INDEV_DEF_DRAG_LIMIT 10
#define LV_INDEV_DEF_DRAG_THROW 10

// Feature configuration
#define LV_USE_USER_DATA 1
#define LV_USE_FILESYSTEM 1
#define LV_USE_FILE_EXPLORER 1
#define LV_USE_GRID 1
#define LV_USE_SNAPSHOT 1
#define LV_USE_MONKEY 1
#define LV_USE_ANIMATION 1
#define LV_USE_SHADOW 1
#define LV_USE_GROUP 1
#define LV_USE_TIMER 1
#define LV_SPRINTF_USE_FLOAT 1

// Color settings
#define LV_COLOR_16_SWAP 0
#define LV_COLOR_SCREEN_TRANSP 0
#define LV_COLOR_MIX_ROUND_OFS 128
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)

// Memory settings
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>
#define LV_MEM_CUSTOM_ALLOC malloc
#define LV_MEM_CUSTOM_FREE free
#define LV_MEM_CUSTOM_REALLOC realloc
#define LV_MEM_BUF_MAX_NUM 16

// HAL settings
#define LV_TICK_CUSTOM 0
#define LV_USE_LOG_LEVEL_TRACE 0

// Render settings
#define LV_USE_DRAW_MASKS 1
#define LV_USE_BLEND_MODES 1

#endif /* LV_CONF_H */