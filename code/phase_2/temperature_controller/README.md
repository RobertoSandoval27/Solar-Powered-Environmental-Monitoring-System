# Temperature Controller and Occupancy Estimation

## Objective

Develop the primary embedded firmware responsible for monitoring the doghouse environment and automatically controlling ventilation while estimating occupancy.

This firmware serves as the foundation of the final smart doghouse system.

---

## Features

- Automatic temperature-controlled fan operation
- User-adjustable fan ON temperature
- User-adjustable fan OFF temperature
- Hysteresis-based fan control
- Temperature displayed in Fahrenheit
- Humidity monitoring
- PIR motion sensing
- Occupancy estimation using configurable timeout
- Persistent user settings stored in ESP32 flash memory
- Runtime configuration through the Serial Monitor

---

## Hardware

- ESP32 Dev Board
- BME280 Environmental Sensor
- HC-SR501 PIR Sensor
- Logic-Level MOSFET Driver Module
- Arctic P12 Pro 120 mm PWM Fan

---

## Control Logic

### Temperature Controller

The controller continuously monitors the internal doghouse temperature.

When the temperature reaches or exceeds the user-defined ON threshold, the ventilation fan is enabled.

The fan remains ON until the temperature falls below the user-defined OFF threshold.

Using separate ON and OFF thresholds introduces hysteresis, preventing rapid cycling near the threshold temperature.

---

### Occupancy Estimation

The PIR sensor is used independently from the fan controller.

Whenever motion is detected:

- The current system time is stored.

If no motion has been detected within the user-defined timeout period:

- Occupancy Status = No Recent Activity

Otherwise:

- Occupancy Status = Likely Present

This approach avoids incorrectly assuming the dog has left the enclosure simply because it is resting.

---

## Runtime Commands

| Command | Description |
|----------|-------------|
| ON=85 | Set fan ON temperature (°F) |
| OFF=80 | Set fan OFF temperature (°F) |
| TIMEOUT=5 | Set occupancy timeout (minutes) |
| RESET | Restore factory default settings |
| HELP | Display available commands |

---

## Persistent Settings

User-defined parameters are stored in the ESP32 flash memory using the Preferences library.

The following settings are automatically restored after power loss:

- Fan ON temperature
- Fan OFF temperature
- Occupancy timeout

---

## Results

Successfully demonstrated:

- Automatic fan control
- Adjustable fan thresholds
- Stable hysteresis operation
- PIR-based occupancy estimation
- Persistent user settings
- Real-time environmental monitoring

---

## Sample Serial Monitor Output

![Temperature Controller Output](/docs/photos/temperature_controller_data_output.png)

---

## Future Improvements

- Web-based configuration dashboard
- Manual fan override
- Battery monitoring
- Solar charging status
- Remote telemetry
