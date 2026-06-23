#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
  Serial.begin(115200);
  delay(2000);

  // ESP32 I2C Pins
  Wire.begin(21, 22);

  Serial.println("Starting BME280 Test");

  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found!");
    while (1);
  }

  Serial.println("BME280 FOUND!");
  Serial.println();
}

void loop() {

  Serial.print("Temperature: ");
  Serial.print(bme.readTemperature());
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.println();

  delay(2000);
}
