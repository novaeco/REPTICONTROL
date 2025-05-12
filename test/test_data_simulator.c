#include "unity.h"
#include "data_simulator.h"
#include <stdio.h>

void setUp(void) {
    data_simulator_init();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_temperature_simulation(void) {
    float initial_temp = data_simulator_get_temperature();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, initial_temp);
    
    // Test heating influence
    data_simulator_apply_heating();
    float heated_temp = data_simulator_get_temperature();
    TEST_ASSERT_TRUE(heated_temp > initial_temp);
    
    // Test cooling influence
    data_simulator_apply_cooling();
    float cooled_temp = data_simulator_get_temperature();
    TEST_ASSERT_TRUE(cooled_temp < heated_temp);
}

void test_humidity_simulation(void) {
    float initial_humidity = data_simulator_get_humidity();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 50.0f, initial_humidity);
    
    // Test humidifier influence
    data_simulator_apply_humidifier();
    float new_humidity = data_simulator_get_humidity();
    TEST_ASSERT_TRUE(new_humidity > initial_humidity);
}

void test_light_simulation(void) {
    float initial_light = data_simulator_get_light();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 75.0f, initial_light);
    
    // Test light target setting
    data_simulator_set_light_target(100.0f);
    data_simulator_update();
    float new_light = data_simulator_get_light();
    TEST_ASSERT_TRUE(new_light > initial_light);
}

void test_value_bounds(void) {
    // Run multiple updates to ensure values stay within bounds
    for (int i = 0; i < 100; i++) {
        data_simulator_update();
        
        float temp = data_simulator_get_temperature();
        float humidity = data_simulator_get_humidity();
        float light = data_simulator_get_light();
        
        TEST_ASSERT_TRUE(temp >= 10.0f && temp <= 45.0f);
        TEST_ASSERT_TRUE(humidity >= 10.0f && humidity <= 95.0f);
        TEST_ASSERT_TRUE(light >= 0.0f && light <= 100.0f);
    }
}

void app_main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_simulation);
    RUN_TEST(test_humidity_simulation);
    RUN_TEST(test_light_simulation);
    RUN_TEST(test_value_bounds);
    UNITY_END();
}