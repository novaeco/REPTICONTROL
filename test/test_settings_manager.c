#include "unity.h"
#include "settings_manager.h"
#include <stdio.h>

void setUp(void) {
    settings_init();
}

void tearDown(void) {
    // Cleanup after each test
}

void test_float_settings(void) {
    // Test float value storage and retrieval
    settings_set_float("test_float", 123.45f);
    float value = settings_get_float("test_float", 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(123.45f, value);

    // Test default value
    value = settings_get_float("nonexistent", 99.9f);
    TEST_ASSERT_EQUAL_FLOAT(99.9f, value);
}

void test_bool_settings(void) {
    // Test boolean value storage and retrieval
    settings_set_bool("test_bool", true);
    bool value = settings_get_bool("test_bool", false);
    TEST_ASSERT_TRUE(value);

    settings_set_bool("test_bool", false);
    value = settings_get_bool("test_bool", true);
    TEST_ASSERT_FALSE(value);

    // Test default value
    value = settings_get_bool("nonexistent", true);
    TEST_ASSERT_TRUE(value);
}

void test_int_settings(void) {
    // Test integer value storage and retrieval
    settings_set_int("test_int", 12345);
    int value = settings_get_int("test_int", 0);
    TEST_ASSERT_EQUAL_INT(12345, value);

    // Test default value
    value = settings_get_int("nonexistent", -1);
    TEST_ASSERT_EQUAL_INT(-1, value);
}

void test_persistence(void) {
    // Set some values
    settings_set_float("persist_float", 3.14f);
    settings_set_bool("persist_bool", true);
    settings_set_int("persist_int", 42);

    // Save settings
    settings_save();

    // Reinitialize settings (simulates reboot)
    settings_init();

    // Verify values persisted
    TEST_ASSERT_EQUAL_FLOAT(3.14f, settings_get_float("persist_float", 0.0f));
    TEST_ASSERT_TRUE(settings_get_bool("persist_bool", false));
    TEST_ASSERT_EQUAL_INT(42, settings_get_int("persist_int", 0));
}

void app_main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_float_settings);
    RUN_TEST(test_bool_settings);
    RUN_TEST(test_int_settings);
    RUN_TEST(test_persistence);
    UNITY_END();
}
