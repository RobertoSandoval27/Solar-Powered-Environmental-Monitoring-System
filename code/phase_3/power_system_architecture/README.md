# Phase 3.1 – Power System Architecture

## Objective

Design and document the complete electrical power architecture for the Solar-Powered Environmental Monitoring System prior to hardware integration.

This phase establishes how electrical energy flows throughout the system, verifies that the selected hardware can support the anticipated electrical load, and documents the engineering decisions that will guide the remaining development of the project.

---

## System Architecture

The figure below illustrates the complete system power architecture.

- The solar panel serves as the renewable energy source.
- The MPPT charge controller regulates battery charging.
- A 12V LiFePO₄ battery stores energy for continuous operation.
- The ventilation fan operates directly from the 12V battery.
- A buck converter provides a regulated 5V supply for the ESP32 and sensors.
- The ESP32 monitors environmental conditions and controls the ventilation fan through a MOSFET driver.

![Power Block Diagram](/docs/diagrams/power_block_diagram.png)

---

## Hardware

| Component | Purpose |
|-----------|---------|
| 20W Monocrystalline Solar Panel | Renewable power generation |
| MPPT Solar Charge Controller | Battery charging and regulation |
| 12V 10Ah LiFePO₄ Battery | Energy storage |
| 12V → 5V Buck Converter | Supplies regulated power to electronics |
| ESP32 Development Board | Main embedded controller |
| BME280 Environmental Sensor | Temperature and humidity monitoring |
| HC-SR501 PIR Sensor | Occupancy estimation through motion detection |
| MOSFET Driver Module | Switches the ventilation fan |
| Arctic P12 Pro 120mm Fan | Active ventilation |

---

# Power Budget

The following values are design estimates used during the system planning stage. Actual power measurements will be collected during Phase 3.5 – Power System Validation.

## Estimated Component Current

| Component | Supply Voltage | Typical Current | Peak Current | Notes |
|-----------|---------------:|----------------:|-------------:|------|
| Arctic P12 Pro Fan | 12V | 0.33 A | ~0.50 A | Running current measured during testing |
| Buck Converter | 12V → 5V | Approximately 90% efficient | — | Small conversion losses |
| ESP32 Development Board | 5V | 100–250 mA | ~500 mA | Higher during Wi-Fi transmission |
| HC-SR501 PIR Sensor | 5V | <1 mA | <1 mA | Continuous monitoring |
| BME280 Sensor | 3.3V | <1 mA | <1 mA | Negligible power consumption |
| MOSFET Driver Module | 3.3V | Negligible | Negligible | Logic-level switching only |

---

## Estimated System Current

### Fan OFF

When the fan is not operating, the system consists primarily of the ESP32 and environmental sensors.

Estimated electronics power:

- ESP32: ~250 mA
- BME280: <1 mA
- PIR Sensor: <1 mA

Approximate system power:

```
5V × 0.25A ≈ 1.25W
```

Equivalent battery current:

```
1.25W ÷ 12V ≈ 0.10A
```

Accounting for buck converter losses:

**Estimated Battery Current ≈ 0.12A**

---

### Fan ON

Fan power:

```
12V × 0.33A = 3.96W
```

Electronics power:

```
≈1.25W
```

Total system power:

```
≈5.21W
```

Equivalent battery current:

```
5.21W ÷ 12V ≈ 0.43A
```

Including converter losses:

**Estimated Battery Current ≈ 0.45A**

---

# Battery Runtime Analysis

Battery specifications:

- Nominal Voltage: 12.8V
- Capacity: 10Ah
- Stored Energy: Approximately 128Wh

---

## Estimated Runtime (Fan OFF)

Average system power:

```
≈1.4W
```

Estimated operating time:

```
128Wh ÷ 1.4W ≈ 91 Hours
```

---

## Estimated Runtime (Fan ON Continuously)

Average system power:

```
≈5.4W
```

Estimated operating time:

```
128Wh ÷ 5.4W ≈ 24 Hours
```

---

## Expected Real-World Runtime

Because the ventilation fan operates only when required, expected runtime should fall between approximately **24 and 90 hours**, depending on:

- Ambient temperature
- Fan duty cycle
- Wi-Fi activity
- Solar charging conditions

---

# Solar Charging Analysis

Solar panel specifications:

- Rated Power: 20W
- Nominal Voltage: 12V

Ideal output:

```
20W
```

Real-world expected output:

```
10–15W
```

Under normal daylight conditions, the solar panel should be capable of supplying the operating load while simultaneously recharging the battery.

---

# Engineering Design Decisions

## Direct 12V Fan Supply

The Arctic P12 Pro fan operates directly from the battery rather than the buck converter.

Benefits include:

- Reduced current through the buck converter
- Improved efficiency
- Lower stress on the regulated electronics supply
- Simplified power distribution

---

## Buck Converter

A dedicated buck converter supplies the ESP32 and sensors with a stable regulated 5V input while maintaining high conversion efficiency.

---

## LiFePO₄ Battery Selection

A LiFePO₄ battery was selected due to its:

- Long service life
- Stable output voltage
- High safety
- Excellent compatibility with solar-powered embedded systems
- Integrated Battery Management System (BMS)

---

# Safety Considerations

The completed system will include:

- Inline fuse on the battery positive terminal
- Common system ground
- Proper wire sizing
- Weather-resistant enclosure
- Battery protection through the integrated BMS

---

# Future Work

The next milestone focuses on integrating the hardware designed during this phase.

Phase 3.2 will include:

- Install inline battery fuse
- Integrate LiFePO₄ battery
- Connect buck converter
- Verify battery-powered operation
- Remove dependency on the bench power supply

---

# Results


Completed:

- Designed complete system power architecture
- Documented system power flow
- Calculated estimated electrical load
- Estimated battery runtime
- Estimated solar charging capability
- Documented engineering design decisions
- Established foundation for hardware integration

---

# Summary

Phase 3.1 established the electrical foundation of the Solar-Powered Environmental Monitoring System by documenting the complete power architecture, estimating system power consumption, analyzing expected battery runtime, and justifying key hardware selections. Completing this design analysis prior to hardware integration reduces implementation risk and provides a clear roadmap for the remaining power system development.
