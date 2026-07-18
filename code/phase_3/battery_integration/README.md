# Phase 3.2 – Battery Integration

## Objective

Integrate the battery power system into the Solar-Powered Environmental Monitoring System and demonstrate that the embedded controller can operate independently from the laboratory bench power supply.

---

# Hardware Used

| Component | Description |
|-----------|-------------|
| 12V 10Ah LiFePO₄ Battery | Primary energy storage |
| LM2596 Buck Converter | 12V to 5V voltage regulation |
| ESP32 Development Board | Main controller |
| BME280 Environmental Sensor | Temperature and humidity sensing |
| HC-SR501 PIR Sensor | Motion sensing |
| MOSFET Driver Module | Fan switching |
| Arctic P12 Pro 120mm Fan | Active ventilation |

---

# Wiring

The battery supplies two independent loads:

- The Arctic P12 ventilation fan directly from the 12V battery.
- The ESP32 and all low-voltage electronics through the LM2596 buck converter.

## Wire Selection

18 AWG stranded wire was used for all battery and power distribution wiring.

Reasons for selecting 18 AWG:

- Supports current well above the expected system load.
- Low voltage drop over short cable lengths.
- Flexible enough for prototyping.
- Provides additional safety margin for future system expansion.

---

# Buck Converter Configuration

The LM2596 adjustable buck converter regulates the battery voltage to the 5V required by the ESP32.

Prior to connecting the controller, the converter output was adjusted using the onboard multi-turn potentiometer.

Adjustment procedure:

1. Connect battery positive to **IN+**
2. Connect battery negative to **IN−**
3. Measure voltage across **OUT+** and **OUT−**
4. Rotate the potentiometer until the output measured **5.00V**

![power_route](/docs/diagrams/buck_converter_diagram.png)

---

# System Wiring

1. Connect alligator clips to each lead on the battery
2. Connect the possitive lead and attatch it to one side of the inline 5A fuse holder, with the other side of the inline fuse holder going into a 3 channel wago connector
3. The negative lead will go to common ground
4. Connect the buck converter's "IN+" to the 3 channel wago, and "IN-" to ground
5. Connect the buck converter's "OUT+" to pin 19 on the ESP32, and "OUT-" to ground
![LM2596 Buck Converter](/docs/diagrams/power_block_diagram.png)

---

# Validation Procedure

The system was tested in two operating configurations.

## Development Configuration

During firmware development:

- ESP32 powered through USB.
- Fan powered from the battery.
- Serial Monitor used for firmware debugging.

This configuration simplified software development while allowing the battery-powered ventilation hardware to be tested independently.

---

## Battery Validation Configuration

For validation testing:

- USB disconnected.
- ESP32 powered only through the battery and buck converter.
- Fan powered directly from the battery.

This confirmed that the controller was capable of operating without the laboratory bench power supply.

USB and battery power were intentionally **not applied simultaneously** during validation testing.

---

# Validation Results

| Test | Result |
|------|--------|
| Battery Voltage | 13.7V |
| Buck Converter Output | 5.00V |

-ESP32 Powered from Battery

-MOSFET Driver Powered

-Fan Operated as intended

-Persistent Settings Retained 

# Future Validation

Environmental sensor readings and occupancy estimation will continue to be verified during Phase 4.

The planned web dashboard will provide a live monitoring interface displaying:

- Temperature
- Humidity
- Fan status
- Occupancy estimation
- Battery voltage
- Power consumption
- Charging status

This interface will eliminate the need for a USB serial connection when observing real-time system behavior.

---

# Results

Completed:

- Battery integration
- Buck converter configuration
- Stable 5V regulation
- Battery-powered ESP32 operation
- Battery-powered fan operation
- Removal of dependency on the bench power supply

---

# Summary

Phase 3.2 successfully transitioned the project from a bench-powered prototype to a battery-powered embedded system. The LM2596 buck converter provided a stable regulated 5V supply for the ESP32 while the Arctic P12 fan operated directly from the 12V LiFePO₄ battery. Validation testing confirmed successful standalone operation, establishing the hardware foundation required for future battery monitoring, solar charging integration, and remote system management.
