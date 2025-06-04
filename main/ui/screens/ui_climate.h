#ifndef UI_CLIMATE_H
#define UI_CLIMATE_H

#include "lvgl.h"

// Create the climate control screen
lv_obj_t *ui_climate_create(void);

// Update the climate screen with current values
void ui_climate_update_values(float temp_target, float humidity_target, float light_target,
                            bool heating_on, bool cooling_on, bool humidifier_on, bool lighting_on);

// Get the current target values from the climate screen
void ui_climate_get_targets(float *temp_target, float *humidity_target, float *light_target,
                          bool *heating_on, bool *cooling_on, bool *humidifier_on, bool *lighting_on);

#endif /* UI_CLIMATE_H */
