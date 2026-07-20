# Local Web Dashboard

## Objective

Develop a local web dashboard hosted directly by the ESP32 to replace the USB Serial Monitor as the primary interface for monitoring the Solar-Powered Environmental Monitoring System. The dashboard provides live access to environmental conditions, controller status, occupancy estimation, battery voltage, and user-configurable settings using any device connected to the same local Wi-Fi network.

---

# Features

The dashboard currently displays:

- Temperature
- Humidity
- Fan status
- Motion detection
- Occupancy estimation
- Time since last motion
- Battery voltage
- Wi-Fi signal strength
- Fan ON temperature
- Fan OFF temperature
- Occupancy timeout

The page automatically refreshes every two seconds without requiring a manual reload.

---

# Hardware

| Component | Purpose |
|-----------|---------|
| ESP32 Development Board | Embedded web server |
| BME280 | Temperature and humidity sensing |
| HC-SR501 PIR Sensor | Motion detection |
| LM2596 Buck Converter | 5V power regulation |
| 12V LiFePO₄ Battery | System power |
| Solar Charging System | Renewable power source |

---

# Software

The dashboard was implemented using:

- WiFi.h
- WiFiServer
- HTML
- CSS
- JavaScript
- JSON

The ESP32 hosts the webpage internally and serves live system data over the local network.

---
# Dashboard Refresh

The dashboard automatically requests updated system data from the ESP32 every two seconds using JavaScript. Rather than reloading the entire webpage, only the displayed values are refreshed, reducing network traffic while providing a smooth, real-time monitoring experience.

---

# Hardware Validation

The dashboard was validated in two stages.

### Initial Validation

The dashboard was first tested using only:

- ESP32
- BME280

This isolated the networking subsystem from the remaining hardware.

---

### Full System Validation

After successful network validation:

- PIR sensor reconnected
- Battery monitor reconnected
- MOSFET driver and fan reconnected

The complete monitoring system successfully operated from battery power while simultaneously serving the web dashboard.

---

# Brownout Investigation

During development the original ESP32 repeatedly reset whenever Wi-Fi initialization began.

Testing determined:

- The firmware was not responsible.
- The dashboard software was not responsible.
- A minimal Wi-Fi sketch also produced brownout resets.

Replacing the ESP32 development board completely resolved the issue.

The replacement board successfully:

- Connected to Wi-Fi
- Hosted the dashboard
- Operated from battery power
- Served live environmental data

This troubleshooting process confirmed that the original board contained a hardware fault rather than a software defect.

---

# Dashboard

The completed dashboard is shown below.

![Web Dashboard](/docs/photos/web_dashboard_live_data.png)

---

# Future Improvements

Future revisions may include:

- Improved visual design
- Mobile-first layout
- Battery charging indicator
- Manual fan override
- Live graphs
- System notifications

---

# Summary

Phase 4.2 replaced the USB Serial Monitor with a locally hosted web dashboard running directly on the ESP32.

The dashboard provides live access to environmental measurements, occupancy estimation, battery voltage, controller settings, and system status from any device connected to the local network.

Successful validation demonstrated stable operation while the ESP32 was powered solely from the solar-charged battery system, completing the first fully standalone version of the project.
