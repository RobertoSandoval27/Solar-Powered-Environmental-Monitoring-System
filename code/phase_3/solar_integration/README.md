# Solar Integration

## Objective

Integrate the solar charging system into the Solar-Powered Environmental Monitoring System, allowing the battery to be recharged while maintaining operation of the embedded controller.

---

# Hardware Used

| Component | Description |
|-----------|-------------|
| 20W Monocrystalline Solar Panel | Renewable energy source |
| 10A PWM Solar Charge Controller | Battery charging and overcharge protection |
| 12V 10Ah LiFePO₄ Battery | Energy storage |
| LM2596 Buck Converter | 12V to 5V voltage regulation |
| ESP32 Development Board | Main embedded controller |

---

# Solar Panel Specifications

| Specification | Value |
|--------------|------:|
| Solar Panel Type | Monocrystalline |
| Rated Power | 20 W |
| Nominal System Voltage | 12 V |
| Maximum Power Voltage (Vmp) | 18 V |
| Maximum Power Current (Imp) | 1.11 A |
| Open Circuit Voltage (Voc) | 21.6 V |
| Short Circuit Current (Isc) | 1.19 A |
| Weight | 3 lb |
| Dimensions | 15 × 13.2 × 0.7 in |

---

# Wiring Procedure

The solar charging system was connected according to the instructions within its manual
1. Place solar panel facing direct sunlight
2. Connect the controller to the solar panel via the "solar" channel
3. Verify the first light is on, indicating the controller is recieving solar power
4. Connect the "battery" channel to the terminals on your 12v battery
5. Verify the second light is on, indicating that the battery is being charged

---

# Prototype Wiring

During development, temporary alligator clips supplied with the solar kit were used to simplify assembly and allow rapid hardware changes.

The alligator clips will be replaced during the final deployment with permanent connectors suitable for outdoor operation.

---

# Observations

According to the manufacturer documentation:

- Battery charging is indicated by the charging LED.
- A third LED illuminates when the battery reaches approximately **14.2 V**, indicating a fully charged battery.

---

# Future Validation

The future web dashboard will display:

- Battery voltage
- Estimated battery state of charge
- Charging status
- Environmental data

This approach eliminates the need for a USB serial connection when monitoring system operation.

---

# Summary

Phase 3.3 completed the renewable energy subsystem by integrating the solar panel and charge controller into the existing battery-powered architecture. Successful hardware validation demonstrated that the charge controller detected both the battery and the solar panel while entering charging mode. This milestone completes the primary electrical power system and establishes the foundation for battery monitoring and future remote power telemetry.
