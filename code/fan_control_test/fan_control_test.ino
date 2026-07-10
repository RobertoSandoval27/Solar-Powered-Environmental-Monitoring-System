const int FAN_CONTROL_PIN = 5;

void setup() {
  Serial.begin(115200);

  pinMode(FAN_CONTROL_PIN, OUTPUT);

  digitalWrite(FAN_CONTROL_PIN, LOW);

  Serial.println("MOSFET Fan Control Test Started");
}

void loop() {

  Serial.println("Fan ON");
  digitalWrite(FAN_CONTROL_PIN, HIGH);
  delay(3000);

  Serial.println("Fan OFF");
  digitalWrite(FAN_CONTROL_PIN, LOW);
  delay(3000);
}
