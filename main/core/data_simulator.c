#include "data_simulator.h"
#include "event_logger.h"
#include "esp_log.h"
#include "esp_random.h"
#include <math.h>
#include <time.h>

static const char *TAG = "data_simulator";

// Current simulated values
static float current_temp = 25.0f;
static float current_humidity = 50.0f;
static float current_light = 75.0f;

// Target for light system (set by controller)
static float light_target = 75.0f;

// Natural environment factors
static float ambient_temp = 22.0f;    // Ambient room temperature
static float ambient_humidity = 40.0f; // Ambient room humidity
static float day_night_cycle = 0.0f;   // 0.0 to 1.0 representing time of day

// Constants for simulation
static const float TEMP_CHANGE_RATE = 0.1f;     // Degrees per update
static const float HUMIDITY_CHANGE_RATE = 0.2f;  // Percent per update
static const float LIGHT_CHANGE_RATE = 0.5f;     // Percent per update
static const float HEATING_POWER = 0.3f;         // Heating effect per update
static const float COOLING_POWER = 0.3f;         // Cooling effect per update
static const float HUMIDIFIER_POWER = 0.5f;      // Humidifier effect per update
static const float TEMP_FLUCTUATION = 0.1f;      // Random fluctuation
static const float HUMIDITY_FLUCTUATION = 0.2f;  // Random fluctuation
static const float LIGHT_FLUCTUATION = 0.3f;     // Random fluctuation

// Initialize the data simulator
void data_simulator_init(void) {
    ESP_LOGI(TAG, "Initializing data simulator");
    
    // Set initial values
    current_temp = 25.0f;
    current_humidity = 50.0f;
    current_light = 75.0f;
    
    // Seed random number generator
    srand(time(NULL));
    
    event_logger_add("Data simulator initialized", false);
}

// Generate a random float between -range and +range
static float random_float(float range) {
    return ((float)rand() / RAND_MAX * 2.0f - 1.0f) * range;
}

// Update simulated sensor data
void data_simulator_update(void) {
    // Update day/night cycle (runs from 0.0 to 1.0 over a 2-minute period for simulation)
    static int update_count = 0;
    update_count++;
    day_night_cycle = fmodf((float)update_count / 120.0f, 1.0f);  // 2-minute cycle
    
    // Calculate ambient temperature based on day/night cycle
    // Warmest at midday (0.5), coolest at midnight (0.0)
    float time_factor = sinf(day_night_cycle * 2.0f * 3.1416f);
    ambient_temp = 22.0f + 3.0f * time_factor;  // Varies from 19-25°C
    
    // Natural drift toward ambient conditions
    // Temperature naturally moves toward ambient
    float temp_drift = (ambient_temp - current_temp) * TEMP_CHANGE_RATE;
    current_temp += temp_drift;
    
    // Humidity naturally decreases over time
    current_humidity -= HUMIDITY_CHANGE_RATE * 0.5f;
    
    // Light based on day/night cycle (plus any artificial light)
    float natural_light = 100.0f * sinf(day_night_cycle * 3.1416f); // Peaks at midday
    natural_light = natural_light < 0.0f ? 0.0f : natural_light;    // No negative light
    
    // Adjust light towards target (artificial) or natural light
    float light_drift;
    if (light_target > 0) {
        // If artificial lighting is on, move toward target
        light_drift = (light_target - current_light) * LIGHT_CHANGE_RATE;
    } else {
        // Otherwise, move toward natural light
        light_drift = (natural_light - current_light) * LIGHT_CHANGE_RATE;
    }
    current_light += light_drift;
    
    // Add random fluctuations
    current_temp += random_float(TEMP_FLUCTUATION);
    current_humidity += random_float(HUMIDITY_FLUCTUATION);
    current_light += random_float(LIGHT_FLUCTUATION);
    
    // Ensure values stay within realistic bounds
    if (current_temp < 10.0f) current_temp = 10.0f;
    if (current_temp > 45.0f) current_temp = 45.0f;
    
    if (current_humidity < 10.0f) current_humidity = 10.0f;
    if (current_humidity > 95.0f) current_humidity = 95.0f;
    
    if (current_light < 0.0f) current_light = 0.0f;
    if (current_light > 100.0f) current_light = 100.0f;
    
    // Generate random events
    if (rand() % 1000 == 0) {  // 0.1% chance per update
        if (current_temp > 35.0f) {
            event_logger_add("ALERT: High temperature detected!", true);
        } else if (current_temp < 15.0f) {
            event_logger_add("ALERT: Low temperature detected!", true);
        }
        
        if (current_humidity < 20.0f) {
            event_logger_add("ALERT: Low humidity detected!", true);
        } else if (current_humidity > 80.0f) {
            event_logger_add("ALERT: High humidity detected!", true);
        }
    }
}

// Get current simulated temperature
float data_simulator_get_temperature(void) {
    return current_temp;
}

// Get current simulated humidity
float data_simulator_get_humidity(void) {
    return current_humidity;
}

// Get current simulated light level
float data_simulator_get_light(void) {
    return current_light;
}

// Set the light target
void data_simulator_set_light_target(float target) {
    light_target = target;
}

// Apply heating influence
void data_simulator_apply_heating(void) {
    current_temp += HEATING_POWER;
}

// Apply cooling influence
void data_simulator_apply_cooling(void) {
    current_temp -= COOLING_POWER;
}

// Apply humidifier influence
void data_simulator_apply_humidifier(void) {
    current_humidity += HUMIDIFIER_POWER;
}