#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Preferences.h>

// =====================================================
// Pin assignments
// =====================================================

const int FAN_CONTROL_PIN = 5;
const int PIR_PIN = 4;
const int BATTERY_ADC_PIN = 34;

// =====================================================
// Default user settings
// =====================================================

const float DEFAULT_FAN_ON_TEMP_F = 85.0;
const float DEFAULT_FAN_OFF_TEMP_F = 80.0;
const unsigned long DEFAULT_OCCUPANCY_TIMEOUT_MS = 300000UL;

// =====================================================
// Battery voltage-divider configuration
// =====================================================

// Measured resistor values
const float BATTERY_R1_OHMS = 46900.0;
const float BATTERY_R2_OHMS = 9994.0;

const float BATTERY_DIVIDER_RATIO =
    (BATTERY_R1_OHMS + BATTERY_R2_OHMS) / BATTERY_R2_OHMS;

// Initial calibration from:
// Battery terminals = 13.49 V
// Divider node = 2.386 V
const float BATTERY_CALIBRATION_FACTOR = 0.9933;

const int BATTERY_SAMPLE_COUNT = 32;
const int BATTERY_SAMPLE_DELAY_MS = 2;

// =====================================================
// User settings
// =====================================================

float fanOnTempF;
float fanOffTempF;
unsigned long occupancyTimeoutMs;

// =====================================================
// Runtime state
// =====================================================

bool fanState = false;
bool motionDetected = false;

unsigned long lastMotionTime = 0;

// =====================================================
// Objects
// =====================================================

Adafruit_BME280 bme;
Preferences preferences;

// =====================================================
// Setup
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(FAN_CONTROL_PIN, OUTPUT);
  digitalWrite(FAN_CONTROL_PIN, LOW);

  pinMode(PIR_PIN, INPUT);
  pinMode(BATTERY_ADC_PIN, INPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);

  Wire.begin(21, 22);
  delay(100);

  loadSettings();

  Serial.println();
  Serial.println("Environmental Controller Starting...");

  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found!");
    Serial.print("Sensor ID: 0x");
    Serial.println(bme.sensorID(), HEX);

    while (true) {
      digitalWrite(FAN_CONTROL_PIN, LOW);
      delay(1000);
    }
  }

  Serial.println("BME280 FOUND!");
  Serial.println("Battery monitor initialized on GPIO34.");
  Serial.println();

  printCommandMenu();
}

// =====================================================
// Main loop
// =====================================================

void loop() {
  checkSerialCommands();

  float tempC = bme.readTemperature();
  float humidity = bme.readHumidity();

  if (isnan(tempC) || isnan(humidity)) {
    Serial.println("Warning: Invalid BME280 reading.");
    delay(2000);
    return;
  }

  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  updateFanControl(tempF);
  updateOccupancyEstimate();

  float batteryVoltage = readBatteryVoltage();

  printSystemStatus(
      tempF,
      humidity,
      batteryVoltage
  );

  delay(2000);
}

// =====================================================
// Persistent settings
// =====================================================

void loadSettings() {
  preferences.begin("settings", false);

  fanOnTempF =
      preferences.getFloat("fanOn", DEFAULT_FAN_ON_TEMP_F);

  fanOffTempF =
      preferences.getFloat("fanOff", DEFAULT_FAN_OFF_TEMP_F);

  occupancyTimeoutMs =
      preferences.getULong(
          "timeout",
          DEFAULT_OCCUPANCY_TIMEOUT_MS
      );

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

// =====================================================
// Fan controller
// =====================================================

void updateFanControl(float tempF) {
  if (!fanState && tempF >= fanOnTempF) {
    fanState = true;
  }

  if (fanState && tempF <= fanOffTempF) {
    fanState = false;
  }

  digitalWrite(
      FAN_CONTROL_PIN,
      fanState ? HIGH : LOW
  );
}

// =====================================================
// Occupancy estimation
// =====================================================

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

  unsigned long timeSinceMotion =
      millis() - lastMotionTime;

  if (timeSinceMotion <= occupancyTimeoutMs) {
    return "Likely Present";
  }

  return "No Recent Activity";
}

String getLastMotionString() {
  if (lastMotionTime == 0) {
    return "Never";
  }

  unsigned long elapsedSeconds =
      (millis() - lastMotionTime) / 1000UL;

  unsigned long minutes = elapsedSeconds / 60UL;
  unsigned long seconds = elapsedSeconds % 60UL;

  String timeString;

  timeString += minutes;
  timeString += " min ";
  timeString += seconds;
  timeString += " sec ago";

  return timeString;
}

// =====================================================
// Battery monitoring
// =====================================================

float readBatteryVoltage() {
  unsigned long totalMillivolts = 0;

  // Discard the first reading for improved stability.
  analogReadMilliVolts(BATTERY_ADC_PIN);
  delay(2);

  for (int sample = 0;
       sample < BATTERY_SAMPLE_COUNT;
       sample++) {

    totalMillivolts +=
        analogReadMilliVolts(BATTERY_ADC_PIN);

    delay(BATTERY_SAMPLE_DELAY_MS);
  }

  float averageMillivolts =
      totalMillivolts /
      static_cast<float>(BATTERY_SAMPLE_COUNT);

  float adcVoltage =
      averageMillivolts / 1000.0;

  float batteryVoltage =
      adcVoltage *
      BATTERY_DIVIDER_RATIO *
      BATTERY_CALIBRATION_FACTOR;

  return batteryVoltage;
}

// =====================================================
// Serial status display
// =====================================================

void printSystemStatus(
    float tempF,
    float humidity,
    float batteryVoltage) {

  Serial.println("========================================");

  Serial.print("Temperature      : ");
  Serial.print(tempF, 2);
  Serial.println(" F");

  Serial.print("Humidity         : ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  Serial.println();

  Serial.print("Fan              : ");
  Serial.println(fanState ? "ON" : "OFF");

  Serial.println();

  Serial.print("Motion           : ");
  Serial.println(motionDetected ? "YES" : "NO");

  Serial.print("Occupancy        : ");
  Serial.println(getOccupancyStatus());

  Serial.print("Last Motion      : ");
  Serial.println(getLastMotionString());

  Serial.println();

  Serial.print("Battery Voltage  : ");
  Serial.print(batteryVoltage, 2);
  Serial.println(" V");

  Serial.println();

  Serial.print("Fan ON Temp      : ");
  Serial.print(fanOnTempF, 2);
  Serial.println(" F");

  Serial.print("Fan OFF Temp     : ");
  Serial.print(fanOffTempF, 2);
  Serial.println(" F");

  Serial.print("Occupancy Timeout: ");
  Serial.print(occupancyTimeoutMs / 60000UL);
  Serial.println(" min");

  Serial.println("========================================");
  Serial.println();
}

// =====================================================
// Serial command interface
// =====================================================

void checkSerialCommands() {
  if (Serial.available() <= 0) {
    return;
  }

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
      Serial.println(
          "Invalid ON threshold. ON must be greater than OFF."
      );
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
      Serial.println(
          "Invalid OFF threshold. OFF must be less than ON."
      );
    }
  }

  else if (command.startsWith("TIMEOUT=")) {
    float value = command.substring(8).toFloat();

    if (value > 0.0) {
      occupancyTimeoutMs =
          static_cast<unsigned long>(
              value * 60000.0
          );

      Serial.print("Updated Occupancy Timeout to ");
      Serial.print(value);
      Serial.println(" minutes");

      saveSettings();
    } else {
      Serial.println(
          "Invalid timeout. Use TIMEOUT=5 for 5 minutes."
      );
    }
  }

  else if (command == "RESET") {
    resetSettings();
  }

  else if (command == "HELP") {
    printCommandMenu();
  }

  else {
    Serial.println(
        "Unknown command. Type HELP for command list."
    );
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
