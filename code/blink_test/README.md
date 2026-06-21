# ESP32 Dev-Kit Blink Test

## Objective

Verify that the ESP32 development board can be programmed successfully using Arduino IDE. 

## Hardware
- ESP32 Dev Board
- USB-C Data Cable

## Procedure
1. Install ESP32 board package by Espressif Systems in Arduino IDE.
2. Install CP210x USB-UART driver from Silicon Labs.
3. Upload "Blink" sketch.
4. Verify onboard LED flashes

## Result 
The Blink sketch was successfully uploaded to the ESP32. The onboard LED flashed at a one-second interval, confirming successful programming and execution.

## Notes
* The microcontroller works, allowing us to advance
* Make sure the CP210x driver is updated or else your ESP32 won't show up within the "PORT" list. 
