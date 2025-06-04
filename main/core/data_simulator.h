#ifndef DATA_SIMULATOR_H
#define DATA_SIMULATOR_H

// Initialize the data simulator
void data_simulator_init(void);

// Update simulated sensor data
void data_simulator_update(void);

// Get current simulated temperature
float data_simulator_get_temperature(void);

// Get current simulated humidity
float data_simulator_get_humidity(void);

// Get current simulated light level
float data_simulator_get_light(void);

// Set the light target (for simulation)
void data_simulator_set_light_target(float target);

// Apply heating influence
void data_simulator_apply_heating(void);

// Apply cooling influence
void data_simulator_apply_cooling(void);

// Apply humidifier influence
void data_simulator_apply_humidifier(void);

#endif /* DATA_SIMULATOR_H */
