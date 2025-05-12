#ifndef CLIMATE_CONTROLLER_H
#define CLIMATE_CONTROLLER_H

#include <stdbool.h>

// Initialize the climate controller
void climate_controller_init(void);

// Update climate control logic
void climate_controller_update(void);

// Set target temperature
void climate_controller_set_temp_target(float temp);

// Set target humidity
void climate_controller_set_humidity_target(float humidity);

// Set target light level
void climate_controller_set_light_target(float light);

// Toggle heating system
void climate_controller_set_heating(bool enable);

// Toggle cooling system
void climate_controller_set_cooling(bool enable);

// Toggle humidifier
void climate_controller_set_humidifier(bool enable);

// Toggle lighting
void climate_controller_set_lighting(bool enable);

// Get current target temperature
float climate_controller_get_temp_target(void);

// Get current target humidity
float climate_controller_get_humidity_target(void);

// Get current target light level
float climate_controller_get_light_target(void);

// Get heating system status
bool climate_controller_is_heating_on(void);

// Get cooling system status
bool climate_controller_is_cooling_on(void);

// Get humidifier status
bool climate_controller_is_humidifier_on(void);

// Get lighting status
bool climate_controller_is_lighting_on(void);

#endif /* CLIMATE_CONTROLLER_H */