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

Phase 1: Hardware bring-up and sensor testing.

## Development Log

### ESP32 Bring-Up (6/20/26)

- Installed CP210x USB-UART driver
- Configured Arduino IDE for ESP32 development
- Resolved COM port detection issues
- Successfully uploaded and executed Blink test
  
## Planned Milestones

- [x] Verify ESP32 programming
- [ ] Read temperature sensor data
- [ ] Read PIR sensor data
- [ ] Control fan using MOSFET
- [ ] Create local web dashboard
- [ ] Integrate battery monitoring
- [ ] Integrate solar charging system
- [ ] Design enclosure
- [ ] Complete final installation

## Project Photos

Project photos and test results will be uploaded throughout development.
