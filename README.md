# ReptiControl - Advanced Reptile Habitat Controller

## Overview
ReptiControl is an embedded climate control system designed for the Waveshare ESP32-S3-Touch-LCD-5B, providing precise environmental management for reptile habitats. The system features a fully touch-enabled interface with real-time monitoring, scheduling, and automated climate control.

## Features
- Real-time temperature, humidity, and light monitoring
- Touch-enabled climate control interface
- Automated scheduling system
- System status monitoring and alerts
- Event logging and data visualization
- Battery management and power optimization
- Intuitive 5" or 7" touchscreen interface
- First-run wizard to choose screen size and connect to Wi-Fi

## Hardware Requirements
- Waveshare ESP32-S3-Touch-LCD-5B or ESP32-S3-Touch-LCD-7
- ESP32-S3 with 8MB PSRAM
- 5" 1024x600 or 7" 800x480 IPS LCD Display
- Capacitive touch panel (5-point)
- Built-in RTC
- Battery management system

On first boot, ReptiControl lets you choose between the 5" and 7" display and
enter Wi-Fi credentials using the French virtual keyboard.

## Software Requirements
- ESP-IDF v5.0.1 or later
- LVGL v9.2.2
- CMake 3.5 or later
- ESP-IDF compatible toolchain

## Project Structure
```
repticontrol/
├── main/
│   ├── core/           # Business logic
│   ├── drivers/        # Hardware abstraction
│   ├── ui/            # User interface
│   │   └── screens/   # Screen implementations
│   └── utils/         # Utility functions
└── test/              # Unit tests
```
External components are retrieved using the IDF component manager.

## Architecture
The application follows a layered architecture:
- UI Layer: LVGL-based touch interface
- Core Layer: Climate control logic and data management
- Driver Layer: Hardware abstraction for display, touch, and sensors
- Utility Layer: Common functions and helpers

## Building and Flashing

1. Set up ESP-IDF environment:
```bash
. $HOME/esp/esp-idf/export.sh
```

2. Configure the project:
```bash
idf.py menuconfig
# Set target to ESP32-S3
# Enable PSRAM
# Configure partition table
```

3. Fetch required components:
```bash
idf.py add-dependency espressif/bt
idf.py add-dependency lvgl/lvgl
# The following components are included with ESP-IDF:
# - mqtt
# - esp_https_ota
# - cjson
```

4. Build the project:
```bash
idf.py build
```

5. Flash to device:
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Development Guidelines
- Code follows ESP-IDF style guide
- All functions are documented using Doxygen format
- Unit tests required for core functionality
- Memory allocations must use appropriate capabilities
- Error handling required for all API calls

## Power Management
- Automatic sleep mode during inactivity
- Battery voltage monitoring
- Graceful shutdown on low battery
- RTC backup for time keeping

## Security Features
- Protected flash storage for settings
- Input validation on all interfaces
- Hardware watchdog implementation
- Secure communication protocols

## License
Copyright (c) 2025 ReptiControl. All rights reserved.
