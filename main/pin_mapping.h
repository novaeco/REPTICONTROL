#ifndef PIN_MAPPING_H
#define PIN_MAPPING_H

// LCD Interface
#define LCD_DISP_EN_GPIO   4
#define LCD_PCLK_GPIO      5
#define LCD_VSYNC_GPIO     6
#define LCD_HSYNC_GPIO     7
#define LCD_DE_GPIO        8

// RGB Data Bus
#define LCD_D0_GPIO        9
#define LCD_D1_GPIO        10
#define LCD_D2_GPIO        11
#define LCD_D3_GPIO        12
#define LCD_D4_GPIO        13
#define LCD_D5_GPIO        14
#define LCD_D6_GPIO        15
#define LCD_D7_GPIO        16
#define LCD_D8_GPIO        17
#define LCD_D9_GPIO        18
#define LCD_D10_GPIO       19
#define LCD_D11_GPIO       20
#define LCD_D12_GPIO       21
#define LCD_D13_GPIO       22
#define LCD_D14_GPIO       23
#define LCD_D15_GPIO       24

// Touch Panel Interface (FT5x06)
#define TOUCH_I2C_SCL      41
#define TOUCH_I2C_SDA      42
#define TOUCH_IRQ_PIN      40
#define TOUCH_RST_PIN      39

// Battery Management
#define BAT_VOLTAGE_PIN    1   // ADC channel for battery voltage
#define BAT_CHARGE_PIN     2   // Battery charging status input
#define BAT_STDBY_PIN      3   // Battery standby status input

// Status LEDs
#define LED_POWER          35  // Power indicator LED
#define LED_STATUS         36  // Status indicator LED
#define LED_ERROR          37  // Error indicator LED

// Communication Interfaces
#define UART_TX_PIN        43  // UART TX for RS-485
#define UART_RX_PIN        44  // UART RX for RS-485
#define RS485_DE_PIN       45  // RS-485 Driver Enable
#define RS485_RE_PIN       46  // RS-485 Receiver Enable

#define CAN_TX_PIN         47  // CAN TX
#define CAN_RX_PIN         48  // CAN RX

// I2C Sensors
#define SENSOR_I2C_SCL     33  // I2C Clock for sensors
#define SENSOR_I2C_SDA     34  // I2C Data for sensors

// LCD Backlight Control
#define LCD_BK_LIGHT_GPIO  25  // Backlight control pin

#endif /* PIN_MAPPING_H */