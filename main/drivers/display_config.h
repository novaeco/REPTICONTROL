#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <stdint.h>

typedef enum {
    DISPLAY_5_INCH,
    DISPLAY_7_INCH
} display_type_t;

void display_config_apply(display_type_t type);
void display_config_init(void);

display_type_t display_config_get_type(void);
void display_config_set_type(display_type_t type);

#endif /* DISPLAY_CONFIG_H */
