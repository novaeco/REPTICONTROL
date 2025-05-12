cmake_minimum_required(VERSION 3.5)

set(COMPONENT_SRCS
    "main.c"
    "app_main.c"
    "ui/ui.c"
    "ui/ui_helpers.c"
    "ui/screens/ui_dashboard.c"
    "ui/screens/ui_climate.c"
    "ui/screens/ui_schedule.c"
    "ui/screens/ui_system.c"
    "ui/screens/ui_logs.c"
    "core/climate_controller.c"
    "core/data_simulator.c"
    "core/event_logger.c"
    "core/system_monitor.c"
    "core/settings_manager.c"
    "drivers/display_driver.c"
    "drivers/touch_driver.c"
    "utils/rtc_manager.c"
    "utils/animations.c"
)

set(COMPONENT_ADD_INCLUDEDIRS
    "."
    "ui"
    "ui/screens"
    "core"
    "drivers"
    "utils"
)

register_component()