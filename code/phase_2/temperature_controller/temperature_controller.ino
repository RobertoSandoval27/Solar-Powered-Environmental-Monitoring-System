#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

const int FAN_CONTROL_PIN = 5;

float fanOnTempF = 85.0;
float fanOffTempF = 80.0;

bool fanState = false;

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(FAN_CONTROL_PIN, OUTPUT);
  digitalWrite(FAN_CONTROL_PIN, LOW);

  Wire.begin(21, 22);

  Serial.println("Temperature Controller Starting...");

  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  Serial.println("BME280 FOUND!");
  Serial.println("Commands:");
  Serial.println("ON=85   sets fan ON temperature");
  Serial.println("OFF=80  sets fan OFF temperature");
  Serial.println();
}

void loop() {
  checkSerialCommands();

  float tempC = bme.readTemperature();
  float tempF = (tempC * 9.0 / 5.0) + 32.0;

  if (!fanState && tempF >= fanOnTempF) {
    fanState = true;
  }

  if (fanState && tempF <= fanOffTempF) {
    fanState = false;
  }

  digitalWrite(FAN_CONTROL_PIN, fanState ? HIGH : LOW);

  Serial.println("==============================");
  Serial.print("Temperature: ");
  Serial.print(tempF);
  Serial.println(" F");

  Serial.print("Fan ON Threshold: ");
  Serial.print(fanOnTempF);
  Serial.println(" F");

  Serial.print("Fan OFF Threshold: ");
  Serial.print(fanOffTempF);
  Serial.println(" F");

  Serial.print("Fan State: ");
  Serial.println(fanState ? "ON" : "OFF");

  Serial.println("Enter command like ON=88 or OFF=82");
  Serial.println("==============================");
  Serial.println();

  delay(2000);
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
      } else {
        Serial.println("Invalid OFF threshold. OFF must be less than ON.");
      }
    }

    else {
      Serial.println("Unknown command. Use ON=85 or OFF=80.");
    }
  }
}
