# PIR Occupancy Sensor Test

## Objective

Verify motion detection capability using the HC-SR501 PIR sensor.

## Hardware

- ESP32 Dev Board
- HC-SR501 PIR Sensor
- Breadboard
- Jumper Wires
- Screwdriver to adjust potentiometers

## Wiring

| PIR | ESP32 |
|------|------|
| VCC | VIN (5V) |
| GND | GND |
| OUT | GPIO4 |

## Testing Configuration
These settings were used to reduce false triggers and shorten the motion detection hold time during testing:

- Trigger: Repeatable Trigger Mode (H), position jumper on the upper two pins
- Delay: Minimum setting 
- Sensitivity: Minimum setting
  
## Procedure

1. Connect PIR sensor to ESP32.
2. Upload PIR test sketch.
3. Allow sensor to warm up for approximately 60 seconds.
4. Move in front of the sensor.
5. Verify motion detection output.
6. Adjust potentiometers for Delay and Sensitivity as needed.

## Results

The PIR sensor successfully detected motion and produced a digital HIGH output when movement occurred within the detection area. A piece of carboard was placed in front of the sensor to "stop" motion, resulting in a "No Movement" response.

## Notes

Initially the sensor was stuck giving a HIGH output since the Delay and Sensitivity were postitioned in the middle. The issue was resolved by:

- Decreasing the delay.
- Reducing sensitivity.

The sensor successfully transitioned between motion and no-motion states after stabilization.

## Reference Images

### HC-SR501 PIR Sensor Configuration

![PIR Sensor Mapping](/docs/diagrams/PIR-Sensor-Mapping.png)
