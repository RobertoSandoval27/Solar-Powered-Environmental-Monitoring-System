# Temperature Controller

## Objective

Develop the primary environmental control logic for the monitoring system by automatically controlling the ventilation fan based on internal temperature.

## Features

- Reads temperature from the BME280 sensor
- Displays temperature in Fahrenheit
- User-adjustable fan ON threshold
- User-adjustable fan OFF threshold
- Serial interface for runtime configuration

## Hardware

- ESP32 Dev Board
- BME280 Environmental Sensor
- MOSFET Driver Module
- Arctic P12 Pro 120mm Fan

## Control Logic

The controller continuously monitors the internal doghouse temperature.

When the measured temperature exceeds the user-defined ON threshold, the fan is enabled.

The fan remains ON until the temperature falls below the user-defined OFF threshold.

This hysteresis prevents rapid fan cycling near the threshold temperature.

## User Commands
These commands modify the fan control thresholds during runtime. "ON" is used to set a threshold when the fan should turn on, and "OFF" is used to set a threshold when the fan should turn off.
Example:

ON=85    //The fan will turn on once the temperature reaches 85 degrees Fahrenheit

OFF=80  //The fan will turn off once the temperature drops below 80 degrees Fahrenheit

## Results

Successfully demonstrated:

- Automatic fan control
- Adjustable thresholds
- Stable hysteresis operation
- Real-time environmental monitoring

## Future Improvements

- Save user settings to ESP32 flash memory
- Configure thresholds through the web dashboard
- Display threshold values remotely

## Data Output
![Temperature Controller Serial Output](/docs/photos/temperature_controller_data_output.png)
