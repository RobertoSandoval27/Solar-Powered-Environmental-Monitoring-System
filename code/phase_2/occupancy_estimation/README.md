# Occupancy Estimation

## Objective

Integrate the HC-SR501 PIR sensor into the primary environmental controller to estimate dog occupancy 

## Features

- Detects motion using the HC-SR501 PIR sensor
- Tracks the time of the most recent motion event
- Estimates occupancy based on configurable inactivity timeout

## Algorithm

Whenever motion is detected:

- Store the current system time.

If motion has been detected within the configured timeout period:

- Estimated Occupancy = Likely Present

Otherwise:

- Estimated Occupancy = No Recent Activity

## User Configuration

Current command:

TIMEOUT=5

This sets the occupancy timeout period in minutes; The default timout period is 5 minutes.

## Results
Successfully integrated PIR motion detection into the primary firmware.


## Future Improvements

- Adjustable timeout through web dashboard
- Occupancy confidence indicator
- Multiple motion event analysis
