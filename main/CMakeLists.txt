cmake_minimum_required(VERSION 3.5)

# Project declaration

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Automatically collect all source files
file(GLOB_RECURSE COMPONENT_SRCS
    "*.c"
)

# Define include directories
set(COMPONENT_ADD_INCLUDEDIRS
    "."
    "core"
    "drivers"
    "ui"
    "ui/screens"
    "utils"
)

# Register component with required ESP-IDF components
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_ADD_INCLUDEDIRS}

    REQUIRES driver esp_lcd esp_timer esp_wifi esp_event nvs_flash esp_pm esp_adc bt esp_system freertos lvgl mqtt esp_https_ota cjson

    REQUIRES driver esp_lcd esp_timer esp_wifi esp_event nvs_flash esp_pm esp_adc bt esp_system freertos lvgl
 main
)

# Configure PSRAM
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    -DCONFIG_SPIRAM_SUPPORT
    -DCONFIG_SPIRAM_SPEED_80M
)

# Configure compiler warnings
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
)

# Configure LVGL
target_compile_definitions(${COMPONENT_LIB} PUBLIC
    -DLV_CONF_INCLUDE_SIMPLE
    -DLV_CONF_PATH=${CMAKE_CURRENT_SOURCE_DIR}/ui/lv_conf.h
)

# Configure optimization flags for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${COMPONENT_LIB} PRIVATE -O2)
endif()
