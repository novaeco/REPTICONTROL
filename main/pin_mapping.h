/**
 * @file pin_mapping.h
 * @brief Pin mapping configuration for ReptiControl hardware
 *
 * This file defines all GPIO assignments for the Waveshare ESP32-S3-Touch-LCD-5B
 * hardware platform. All pin definitions should be centralized here.
 */

#ifndef PIN_MAPPING_H
#define PIN_MAPPING_H

// LCD Interface
#define LCD_RESET_PIN       4
#define LCD_CS_PIN         5
#define LCD_DC_PIN         6
#define LCD_WR_PIN         7
#define LCD_RD_PIN         8

// RGB Interface
#define LCD_VSYNC_PIN      9
#define LCD_HSYNC_PIN      10
#define LCD_DE_PIN         11
#define LCD_PCLK_PIN       12

// RGB Data Bus
#define LCD_B0_PIN         13
#define LCD_B1_PIN         14
#define LCD_B2_PIN         15
#define LCD_B3_PIN         16
#define LCD_B4_PIN         17
#define LCD_G0_PIN         18
#define LCD_G1_PIN         19
#define LCD_G2_PIN         20
#define LCD_G3_PIN         21
#define LCD_G4_PIN         22
#define LCD_G5_PIN         23
#define LCD_R0_PIN         24
#define LCD_R1_PIN         25
#define LCD_R2_PIN         26
#define LCD_R3_PIN         27
#define LCD_R4_PIN         28

// Touch Panel Interface
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
#define UART_TX_PIN       43   // UART TX for RS-485
#define UART_RX_PIN       44   // UART RX for RS-485
#define RS485_DE_PIN      45   // RS-485 Driver Enable
#define RS485_RE_PIN      46   // RS-485 Receiver Enable

#define CAN_TX_PIN        47   // CAN TX
#define CAN_RX_PIN        48   // CAN RX

// I2C Sensors
#define SENSOR_I2C_SCL    33   // I2C Clock for sensors
#define SENSOR_I2C_SDA    34   // I2C Data for sensors

#endif /* PIN_MAPPING_H */