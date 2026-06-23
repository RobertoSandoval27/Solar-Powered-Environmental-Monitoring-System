#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(21, 22);

  Serial.println("Chip ID test starting...");

  Wire.beginTransmission(0x76);
  Wire.write(0xD0);

  byte error = Wire.endTransmission();

  Serial.print("I2C error code: ");
  Serial.println(error);

  Wire.requestFrom(0x76, 1);

  if (Wire.available()) {
    byte id = Wire.read();

    Serial.print("Chip ID: 0x");
    Serial.println(id, HEX);
  } else {
    Serial.println("No data returned.");
  }
}

void loop() {
}
