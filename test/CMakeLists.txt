cmake_minimum_required(VERSION 3.5)

# Unity test framework component
set(COMPONENT_SRCS
    "test_climate_controller.c"
    "test_data_simulator.c"
    "test_settings_manager.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "."
    "../main/core"
    "../main/utils"
)

# Register test component
register_component()

# Link Unity test framework
target_link_libraries(${COMPONENT_LIB} PRIVATE unity)