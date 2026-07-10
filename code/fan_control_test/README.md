# MOSFET Fan Control Test

## Objective

Verify that the ESP32 can control a 12V DC fan through a logic-level MOSFET driver module.

## Hardware

- ESP32 Dev Board
- MOSFET PWM Controller Module by Ceksezx
- Arctic P12 Pro 120mm 12V PWM Fan
- Jesverty SPS-3010 DC Bench Power Supply
- Breadboard
- Jumper Wires
- 2 Alligator clip jumper wires

## Wiring

### Control Side

| MOSFET Module | ESP32 |
|---------------|-------|
| TRIG/PWM | GPIO5 |
| GND | GND |

### Power Side

| MOSFET Module | Connection |
|---------------|------------|
| VIN+ | +12V DC Power Supply |
| VIN- | Power Supply Ground |
| OUT+ | Fan +12V |
| OUT- | Fan Ground |

## Procedure
OPTIONAL: Cut the 4-pin connector from the fan to isolate the wires, and connect a single channel level connector on each to bridge an extension wire. This made it easier in my situation since I was getting a short when utilizing a 4-pin connector breakout.
1. Configure the DC bench power supply to 12.0V.
2. Limit the current to approximately 0.5A.
3. Connect the MOSFET module to the ESP32.
4. Connect the fan to MOSFET module
5. Connect the power source to MOSFET module
6. Have a common ground 
7. Upload the fan control sketch.
8. Verify the MOSFET switches the fan ON and OFF within Serial Monitor.

## Results

The ESP32 successfully controlled the MOSFET module using a 3.3V GPIO output.

The MOSFET module successfully switched power to the Arctic P12 Pro fan.

Measured Data:

- Supply Voltage: 12.0V
- Fan Current: ~0.33A

## Notes

Initial testing using the LiFePO₄ battery was unsuccessful due to the battery being heavily discharged. It would not hold charge well, resulting in the fan not turning. Additionally, one of my solders for the MOSFET module didn't make a great connection, causing it to never power on; There is a red LED to indicate whether it is powered correctly.

A regulated DC bench power supply was substituted to isolate the issue and verify that:

- ESP32 control logic was functioning correctly.
- MOSFET driver module operated correctly.
- Arctic P12 Pro fan operated normally.
I utilized the bottom pair of GND and PWM on the MOSFET module by soldering two pins allowing it to be placed on a breadboard.

## Reference Pictures
### Fan Pins
![Fan Pinout](/docs/diagrams/Fan_Pins.png)
### MOSFET Pins
![MOSFET Module Pinout](/docs/diagrams/MOSFET-Pinout.png)
