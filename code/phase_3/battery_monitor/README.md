# Battery Voltage Monitoring

## Objective

Add battery-voltage monitoring to the Solar-Powered Environmental Monitoring System so the ESP32 can report the condition of the 12V LiFePO₄ battery.

---

## Hardware Used

| Component | Purpose |
|-----------|---------|
| ESP32 Development Board | Main embedded controller |
| 12V 10Ah LiFePO₄ Battery | System energy storage |
| 47 kΩ Resistor | Upper voltage-divider resistor |
| 10 kΩ Resistor | Lower voltage-divider resistor |
| LM2596 Buck Converter | Supplies regulated 5V to the ESP32 |
| Digital Multimeter | Circuit verification and calibration |
| Breadboard and Jumper Wires | Prototype assembly |

---

## Voltage Divider Design

The battery-voltage monitor uses two resistors connected in series across the battery supply.

![Wiring for battery monitoring](/docs/photos/battery_monitoring_wiring.png)


The voltage divider is connected as a separate parallel branch after the 5A fuse.

It does not supply power to the fan or buck converter. It only produces a reduced measurement voltage for the ESP32 ADC.

The fan and buck converter remain connected directly to the fused 12V power rail.

---

## Grounding

The voltage divider and ESP32 must share the same electrical reference.

The following connections use a common system ground:

- Battery negative
- Buck converter input negative
- Buck converter output negative
- ESP32 ground
- MOSFET driver ground
- Bottom of the 9.994 kΩ resistor

---

## Resistor Measurements

The resistor values were verified with a digital multimeter before installation.

| Resistor | Nominal Value | Measured Value |
|----------|--------------:|---------------:|
| R1 | 47 kΩ | 46.9 kΩ |
| R2 | 10 kΩ | 9.994 kΩ |

---

## Divider Ratio

The voltage-divider multiplication ratio is:

```text
Divider Ratio = (R1 + R2) / R2
```

Using the measured resistor values:

```text
Divider Ratio = (46.9 kΩ + 9.994 kΩ) / 9.994 kΩ
Divider Ratio ≈ 5.693
```

The battery voltage is reconstructed in firmware using:

```text
Battery Voltage = ADC Voltage × Divider Ratio × Calibration Factor
```

---

## Initial Hardware Validation

Before connecting the divider output to GPIO34, the circuit was tested with a multimeter.

Measured divider-node voltage:

```text
2.386 V
```

The divider output remained below the ESP32's 3.3V analog-input limit.

At the maximum anticipated 14.6V battery-charging voltage, the divider is expected to produce approximately:

```text
14.6 V / 5.693 ≈ 2.56 V
```

This provides adequate safety margin for the ESP32 ADC input.

---

## Battery Voltage Comparison

Three values were considered during implementation:

| Measurement Method | Battery Voltage |
|--------------------|----------------:|
| Calculated from the 2.386V divider-node measurement | 13.58V |
| Voltage Accross Battery Terminals | 13.32V |
| Serial Montitor Value | 13.22V |

The ESP32 reading was approximately:

```text
13.32 V - 13.22 V = 0.10 V
```

below the simultaneous direct multimeter measurement.

This corresponds to an error of approximately:

```text
0.10 / 13.32 × 100 ≈ 0.75%
```

The calculated 13.58V value was based on an earlier divider-node measurement, so it was not recorded at exactly the same time as the final 13.32V and 13.22V comparison. Battery voltage can change as the fan operates, the battery rests, or the solar charger is connected.

---

## ESP32 ADC Configuration

GPIO34 was selected because:

- It is an input-only pin.
- It supports analog measurements.
- It is part of ADC1.
- ADC1 remains available while Wi-Fi is active.

The ADC was configured with:

```cpp
analogReadResolution(12);
analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);
```

The wider attenuation range allows the ESP32 to measure the approximately 2.3–2.6V divider output.

---

## ADC Averaging

Thirty-two ADC readings are averaged for every reported battery-voltage value.

This reduces short-term variation caused by:

- ADC noise
- Fan switching
- Buck-converter activity
- Electrical interference
- Normal measurement variation

The first ADC reading is discarded before averaging because the first conversion can occasionally be less stable.

---

## Battery Percentage Decision

An estimated battery percentage was initially implemented.

Testing showed that the estimated percentage changed significantly even when the measured voltage changed only slightly. This occurs because LiFePO₄ batteries maintain a relatively flat voltage through much of their discharge cycle.

The percentage display was therefore removed from the core firmware.

The system now reports the directly measured value:

```text
Battery Voltage : 13.22 V
```

This is more transparent and reliable than presenting an unstable percentage estimate.

More advanced state-of-charge estimation may be implemented later using current monitoring, voltage trends, and coulomb-counting hardware.

---

## Firmware Integration

Battery monitoring was integrated into the existing environmental-controller firmware rather than developed as a separate test sketch.

A new function was added:

```cpp
float readBatteryVoltage();
```

This function:

1. Reads GPIO34.
2. Averages multiple ADC samples.
3. Converts the ADC reading to volts.
4. Applies the measured divider ratio.
5. Applies the calibration factor.
6. Returns the estimated battery-terminal voltage.

---

## Serial Monitor Output

The controller now reports battery voltage with the existing environmental and system data.

![Battery Voltage Serial Output](/docs/photos/battery_voltage_serial_output.png)

---

## Limitations

The current design measures battery terminal voltage only.

It does not directly measure:

- Charging current
- Discharging current
- Solar-panel output power
- System power consumption
- Accurate battery state of charge

Battery voltage can also vary depending on:

- Fan operation
- Solar charging
- Battery temperature
- Time since charging
- Electrical load

These limitations will be considered when interpreting the displayed voltage.

---

## Summary

Phase 4.1 added battery-voltage monitoring to the Solar-Powered Environmental Monitoring System using a resistor voltage divider and the ESP32 ADC.

The completed circuit safely reduced the LiFePO₄ battery voltage to the ESP32 measurement range. The firmware reported 13.22V while a simultaneous multimeter measurement reported 13.32V, resulting in an approximate 0.10V or 0.75% difference.

The directly measured battery voltage was retained as the primary battery-status value. Percentage estimation was removed because voltage-only LiFePO₄ state-of-charge estimates proved too unstable for reliable user presentation.
