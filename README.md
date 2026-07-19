# Solar-Powered-Environmental-Monitoring-System

## Overview

This project is a solar-powered environmental monitoring and control system designed for outdoor enclosure applications. The system monitors environmental conditions, occupancy, and power system status while automatically controlling ventilation and providing remote telemetry through a web-based interface.

## Features

- Detects occupancy using passive infrared sensing
- Monitors enclosure temperature and humidity
- Automatically activates ventilation when temperature exceeds a configurable threshold
- Operates from a solar-powered energy system
- Monitors battery charging and discharging status
- Provides real-time environmental data through a web dashboard
- Supports remote monitoring over Wi-Fi

## Hardware

- ESP32 Dev Board
- HC-SR501 PIR Motion Sensor
- BME280 Temprature/Humidity Sensor
- 12v 120mm PWM Fan
- MOSFET Driver Module
- Solar Panel and Charge Controller
- 12V 10Ah LiFePO4 Battery
- LM2596 Buck Converter

## Current Status

- [x] Phase 1: Hardware bring-up and sensor testing.

- [x] Phase 2: System Integration and Control Logic
- [ ] Phase 3: Power System Integration

## Development Log

### ESP32 Bring-Up (6/20/26)

- Installed CP210x USB-UART driver
- Configured Arduino IDE for ESP32 development
- Resolved COM port detection issues
- Successfully uploaded and executed Blink test

### BME280 Environmental Sensor Integration (6/22/26)

- Established I2C communication with the BME280 sensor.
- Verified sensor address at 0x76 using an I2C scanner.
- Confirmed chip identity using the sensor ID register (0x60).
- Successfully measured temperature, humidity, and pressure.
- Validated environmental sensing subsystem for future fan control logic.

### PIR Occupancy Sensor Integration (6/23/26)

- Successfully interfaced HC-SR501 PIR sensor with the ESP32.
- Verified digital motion detection functionality.
- Adjusted sensitivity and delay settings for accurate measurements.
- Validated occupancy detection subsystem for future ventilation control logic.

### MOSFET and Fan Control Verification (7/10/26)

- Verified MOSFET switching using ESP32 GPIO control.
- Successfully controlled the Arctic P12 12V fan using the MOSFET module.
- Verified fan current draw of approximately 0.33 A using a DC power supply.
- Diagnosed a faulty/discharged LiFePO₄ battery during testing and isolated the issue from the control circuitry.

### Temperature Controller (7/11/26)

- First version of the environmental control firmware.
- Implemented automatic fan control using BME280 temperature data.
- Added hysteresis to prevent rapid fan cycling.
- Implemented runtime configuration of fan ON/OFF thresholds through the Serial Monitor.
- Verified automatic fan operation using a regulated DC bench power supply.

### Occupancy Estimation (7/16/26)

- Integrated PIR motion sensing into the primary firmware.
- Implemented occupancy estimation using a configurable inactivity timeout.
- Added runtime configuration through the Serial Monitor.

### Power System Architecture (7/16/26)

- Power architecture
- Power budget
- Runtime analysis
- Design decisions

### Battery Integration (7/17/26)

- LiFePO₄ battery integration
- LM2596 buck converter configuration
- Standalone battery-powered operation

### Solar Integration (7/19/26)

- Integrated a 20W monocrystalline solar panel.
- Connected and validated the PWM solar charge controller.
- Verified battery charging operation.
  
## Planned Milestones
### Phase 1 – Hardware Bring-up
- [x] Verify ESP32 programming
- [x] Read temperature sensor data
- [x] Read PIR sensor data
- [x] Control fan using MOSFET

### Phase 2 – System Integration

- [x] Implement temperature-controlled fan logic
- [x] Integrate PIR motion detection into occupancy estimation
- [x] Implement persistent user settings

### Phase 3 – Power System Integration

- [x] Power system architecture
- [x] Integrate LiFePO₄ battery
- [x] Integrate solar charging system

### Phase 4 – System Monitoring and User Interface

#### Battery Monitoring

- [ ] Display battery voltage
- [ ] Display estimated battery state of charge
- [ ] Display charging status

#### Web Dashboard

- [ ] Create local web dashboard
- [ ] Display environmental data
- [ ] Display occupancy estimation
- [ ] Display battery information
- [ ] Manual fan override
- [ ] Adjust controller parameters

### Phase 5 – Final System Assembly

- [ ] Design enclosure
- [ ] Final wiring and cable management
- [ ] Install complete system
- [ ] Tune occupancy estimation
- [ ] Outdoor validation testing
- [ ] Final documentation
