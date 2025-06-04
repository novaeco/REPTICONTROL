#include "unity.h"
#include "climate_controller.h"
#include "data_simulator.h"
#include <stdio.h>

void setUp(void) {
    // Initialize before each test
    climate_controller_init();
    data_simulator_init();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_temperature_control(void) {
    // Test temperature target setting
    climate_controller_set_temp_target(25.0f);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, climate_controller_get_temp_target());

    // Test bounds checking
    climate_controller_set_temp_target(50.0f); // Above max
    TEST_ASSERT_EQUAL_FLOAT(40.0f, climate_controller_get_temp_target());

    climate_controller_set_temp_target(10.0f); // Below min
    TEST_ASSERT_EQUAL_FLOAT(15.0f, climate_controller_get_temp_target());
}

void test_humidity_control(void) {
    // Test humidity target setting
    climate_controller_set_humidity_target(50.0f);
    TEST_ASSERT_EQUAL_FLOAT(50.0f, climate_controller_get_humidity_target());

    // Test bounds checking
    climate_controller_set_humidity_target(100.0f); // Above max
    TEST_ASSERT_EQUAL_FLOAT(90.0f, climate_controller_get_humidity_target());

    climate_controller_set_humidity_target(10.0f); // Below min
    TEST_ASSERT_EQUAL_FLOAT(20.0f, climate_controller_get_humidity_target());
}

void test_light_control(void) {
    // Test light target setting
    climate_controller_set_light_target(75.0f);
    TEST_ASSERT_EQUAL_FLOAT(75.0f, climate_controller_get_light_target());

    // Test bounds checking
    climate_controller_set_light_target(150.0f); // Above max
    TEST_ASSERT_EQUAL_FLOAT(100.0f, climate_controller_get_light_target());

    climate_controller_set_light_target(-10.0f); // Below min
    TEST_ASSERT_EQUAL_FLOAT(0.0f, climate_controller_get_light_target());
}

void test_system_toggles(void) {
    // Test heating system toggle
    climate_controller_set_heating(true);
    TEST_ASSERT_TRUE(climate_controller_is_heating_on());
    climate_controller_set_heating(false);
    TEST_ASSERT_FALSE(climate_controller_is_heating_on());

    // Test cooling system toggle
    climate_controller_set_cooling(true);
    TEST_ASSERT_TRUE(climate_controller_is_cooling_on());
    climate_controller_set_cooling(false);
    TEST_ASSERT_FALSE(climate_controller_is_cooling_on());

    // Test humidifier toggle
    climate_controller_set_humidifier(true);
    TEST_ASSERT_TRUE(climate_controller_is_humidifier_on());
    climate_controller_set_humidifier(false);
    TEST_ASSERT_FALSE(climate_controller_is_humidifier_on());

    // Test lighting toggle
    climate_controller_set_lighting(true);
    TEST_ASSERT_TRUE(climate_controller_is_lighting_on());
    climate_controller_set_lighting(false);
    TEST_ASSERT_FALSE(climate_controller_is_lighting_on());
}

void app_main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_control);
    RUN_TEST(test_humidity_control);
    RUN_TEST(test_light_control);
    RUN_TEST(test_system_toggles);
    UNITY_END();
}
