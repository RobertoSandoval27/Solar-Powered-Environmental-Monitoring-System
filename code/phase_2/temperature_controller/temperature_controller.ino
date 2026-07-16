#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Preferences.h>

// Pin assignments
const int FAN_CONTROL_PIN = 5;
const int PIR_PIN = 4;

// Default settings
const float DEFAULT_FAN_ON_TEMP_F = 85.0;
const float DEFAULT_FAN_OFF_TEMP_F = 80.0;
const unsigned long DEFAULT_OCCUPANCY_TIMEOUT_MS = 300000; // 5 minutes

// User settings
float fanOnTempF;
float fanOffTempF;
unsigned long occupancyTimeoutMs;

// Occupancy variables
unsigned long lastMotionTime = 0;
bool motionDetected = false;

// Fan state
bool fanState = false;

// Sensor and storage objects
Adafruit_BME280 bme;
Preferences preferences;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(FAN_CONTROL_PIN, OUTPUT);
  digitalWrite(FAN_CONTROL_PIN, LOW);

  pinMode(PIR_PIN, INPUT);

  Wire.begin(21, 22);
  delay(100);

  loadSettings();

  Serial.println("Temperature Controller Starting...");

  bool status = bme.begin(0x76, &Wire);

  if (!status) {
    Serial.println("BME280 not found!");
    Serial.print("Sensor ID: 0x");
    Serial.println(bme.sensorID(), HEX);
    while (1);
  }

  Serial.println("BME280 FOUND!");
  Serial.println();

  printCommandMenu();
}

void loop() {
  checkSerialCommands();

  float tempC = bme.readTemperature();
  float tempF = (tempC * 9.0 / 5.0) + 32.0;
  float humidity = bme.readHumidity();

  updateFanControl(tempF);
  updateOccupancyEstimate();
  printSystemStatus(tempF, humidity);

  delay(2000);
}

void loadSettings() {
  preferences.begin("settings", false);

  fanOnTempF = preferences.getFloat("fanOn", DEFAULT_FAN_ON_TEMP_F);
  fanOffTempF = preferences.getFloat("fanOff", DEFAULT_FAN_OFF_TEMP_F);
  occupancyTimeoutMs = preferences.getULong("timeout", DEFAULT_OCCUPANCY_TIMEOUT_MS);

  Serial.println("Settings loaded.");
}

void saveSettings() {
  preferences.putFloat("fanOn", fanOnTempF);
  preferences.putFloat("fanOff", fanOffTempF);
  preferences.putULong("timeout", occupancyTimeoutMs);

  Serial.println("Settings saved.");
}

void resetSettings() {
  fanOnTempF = DEFAULT_FAN_ON_TEMP_F;
  fanOffTempF = DEFAULT_FAN_OFF_TEMP_F;
  occupancyTimeoutMs = DEFAULT_OCCUPANCY_TIMEOUT_MS;

  saveSettings();

  Serial.println("Settings reset to defaults.");
}

void updateFanControl(float tempF) {
  if (!fanState && tempF >= fanOnTempF) {
    fanState = true;
  }

  if (fanState && tempF <= fanOffTempF) {
    fanState = false;
  }

  digitalWrite(FAN_CONTROL_PIN, fanState ? HIGH : LOW);
}

void updateOccupancyEstimate() {
  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) {
    motionDetected = true;
    lastMotionTime = millis();
  } else {
    motionDetected = false;
  }
}

String getOccupancyStatus() {
  if (lastMotionTime == 0) {
    return "No Motion Recorded";
  }

  if (millis() - lastMotionTime <= occupancyTimeoutMs) {
    return "Likely Present";
  }

  return "No Recent Activity";
}

String getLastMotionString() {
  if (lastMotionTime == 0) {
    return "Never";
  }

  unsigned long elapsedSeconds = (millis() - lastMotionTime) / 1000;
  unsigned long minutes = elapsedSeconds / 60;
  unsigned long seconds = elapsedSeconds % 60;

  String timeString = "";
  timeString += minutes;
  timeString += " min ";
  timeString += seconds;
  timeString += " sec ago";

  return timeString;
}

void printSystemStatus(float tempF, float humidity) {
  Serial.println("========================================");

  Serial.print("Temperature : ");
  Serial.print(tempF);
  Serial.println(" F");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println();

  Serial.print("Fan         : ");
  Serial.println(fanState ? "ON" : "OFF");

  Serial.println();

  Serial.print("Motion      : ");
  Serial.println(motionDetected ? "YES" : "NO");

  Serial.print("Occupancy   : ");
  Serial.println(getOccupancyStatus());

  Serial.print("Last Motion : ");
  Serial.println(getLastMotionString());

  Serial.println();

  Serial.print("Fan ON Temp : ");
  Serial.print(fanOnTempF);
  Serial.println(" F");

  Serial.print("Fan OFF Temp: ");
  Serial.print(fanOffTempF);
  Serial.println(" F");

  Serial.print("Timeout     : ");
  Serial.print(occupancyTimeoutMs / 60000);
  Serial.println(" min");

  Serial.println("========================================");
  Serial.println();
}

void checkSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command.startsWith("ON=")) {
      float value = command.substring(3).toFloat();

      if (value > fanOffTempF) {
        fanOnTempF = value;
        Serial.print("Updated Fan ON Threshold to ");
        Serial.print(fanOnTempF);
        Serial.println(" F");
        saveSettings();
      } else {
        Serial.println("Invalid ON threshold. ON must be greater than OFF.");
      }
    }

    else if (command.startsWith("OFF=")) {
      float value = command.substring(4).toFloat();

      if (value < fanOnTempF) {
        fanOffTempF = value;
        Serial.print("Updated Fan OFF Threshold to ");
        Serial.print(fanOffTempF);
        Serial.println(" F");
        saveSettings();
      } else {
        Serial.println("Invalid OFF threshold. OFF must be less than ON.");
      }
    }

    else if (command.startsWith("TIMEOUT=")) {
      float value = command.substring(8).toFloat();

      if (value > 0) {
        occupancyTimeoutMs = (unsigned long)(value * 60000);
        Serial.print("Updated Occupancy Timeout to ");
        Serial.print(value);
        Serial.println(" minutes");
        saveSettings();
      } else {
        Serial.println("Invalid timeout. Use TIMEOUT=5 for 5 minutes.");
      }
    }

    else if (command == "RESET") {
      resetSettings();
    }

    else if (command == "HELP") {
      printCommandMenu();
    }

    else {
      Serial.println("Unknown command. Type HELP for command list.");
    }
  }
}

void printCommandMenu() {
  Serial.println("Commands:");
  Serial.println("ON=85        Set fan ON temperature");
  Serial.println("OFF=80       Set fan OFF temperature");
  Serial.println("TIMEOUT=5    Set occupancy timeout in minutes");
  Serial.println("RESET        Restore default settings");
  Serial.println("HELP         Show command list");
  Serial.println();
}
