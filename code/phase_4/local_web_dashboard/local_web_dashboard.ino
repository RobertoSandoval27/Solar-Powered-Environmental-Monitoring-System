#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Preferences.h>

// =====================================================
// Wi-Fi credentials
// Keep the quotation marks.
// =====================================================

const char* WIFI_SSID = "YOUR_NETWORK_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// =====================================================
// Hardware enable settings
//
// FIRST FULL-DASHBOARD TEST:
// Leave these false while the battery is disconnected.
// Only the BME280 will be active.
// =====================================================

const bool ENABLE_FAN_CONTROL = false;
const bool ENABLE_PIR_SENSOR = false;
const bool ENABLE_BATTERY_MONITOR = false;

// =====================================================
// Pin assignments
// =====================================================

const int FAN_CONTROL_PIN = 5;
const int PIR_PIN = 4;
const int BATTERY_ADC_PIN = 34;

// =====================================================
// Default persistent settings
// =====================================================

const float DEFAULT_FAN_ON_TEMP_F = 85.0;
const float DEFAULT_FAN_OFF_TEMP_F = 80.0;

const unsigned long DEFAULT_OCCUPANCY_TIMEOUT_MS =
    300000UL;

// =====================================================
// Battery voltage-divider configuration
// =====================================================

const float BATTERY_R1_OHMS = 46900.0;
const float BATTERY_R2_OHMS = 9994.0;

const float BATTERY_DIVIDER_RATIO =
    (BATTERY_R1_OHMS + BATTERY_R2_OHMS) /
    BATTERY_R2_OHMS;

const float BATTERY_CALIBRATION_FACTOR = 0.9933;

const int BATTERY_SAMPLE_COUNT = 32;
const int BATTERY_SAMPLE_DELAY_MS = 2;

// =====================================================
// Timing
// =====================================================

const unsigned long SENSOR_UPDATE_INTERVAL_MS = 2000UL;
const unsigned long SERIAL_UPDATE_INTERVAL_MS = 4000UL;
const unsigned long WIFI_RETRY_INTERVAL_MS = 10000UL;
const unsigned long HTTP_CLIENT_TIMEOUT_MS = 1500UL;

unsigned long lastSensorUpdateMs = 0;
unsigned long lastSerialUpdateMs = 0;
unsigned long lastWiFiRetryMs = 0;

// =====================================================
// Persistent user settings
// =====================================================

float fanOnTempF = DEFAULT_FAN_ON_TEMP_F;
float fanOffTempF = DEFAULT_FAN_OFF_TEMP_F;

unsigned long occupancyTimeoutMs =
    DEFAULT_OCCUPANCY_TIMEOUT_MS;

// =====================================================
// Live system state
// =====================================================

float currentTempF = 0.0;
float currentHumidity = 0.0;
float currentBatteryVoltage = 0.0;

bool bmeDataValid = false;
bool fanState = false;
bool motionDetected = false;

unsigned long lastMotionTimeMs = 0;

// =====================================================
// Objects
// =====================================================

Adafruit_BME280 bme;
Preferences preferences;
WiFiServer server(80);

// =====================================================
// Function declarations
// =====================================================

void connectToWiFi();
void maintainWiFi();

void handleHttpClient();

void sendHttpResponse(
    WiFiClient& client,
    const String& status,
    const String& contentType,
    const String& body
);

void sendDashboardPage(WiFiClient& client);
void sendJsonData(WiFiClient& client);
void sendNotFound(WiFiClient& client);

String buildDashboardPage();
String buildJsonData();
String jsonEscape(const String& input);

void updateSystemData();
void updateFanControl(float temperatureF);
void updateOccupancyEstimate();

float readBatteryVoltage();

String getFanStatus();
String getMotionStatus();
String getOccupancyStatus();
String getLastMotionString();

void loadSettings();
void saveSettings();
void resetSettings();

void checkSerialCommands();
void printCommandMenu();
void printSystemStatus();

// =====================================================
// Setup
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("Environmental Controller Starting...");

  pinMode(FAN_CONTROL_PIN, OUTPUT);
  digitalWrite(FAN_CONTROL_PIN, LOW);

  pinMode(PIR_PIN, INPUT);
  pinMode(BATTERY_ADC_PIN, INPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);

  Wire.begin(21, 22);
  delay(100);

  if (!bme.begin(0x76, &Wire)) {
    Serial.println("BME280 not found.");
    Serial.println(
        "Check 3.3V, GND, SDA GPIO21, and SCL GPIO22."
    );

    while (true) {
      digitalWrite(FAN_CONTROL_PIN, LOW);
      delay(1000);
    }
  }

  Serial.println("BME280 FOUND!");

  loadSettings();
  updateSystemData();
  connectToWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    Serial.println("HTTP server started.");
  }

  Serial.println();
  printCommandMenu();
}

// =====================================================
// Main loop
// =====================================================

void loop() {
  maintainWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    handleHttpClient();
  }

  checkSerialCommands();

  unsigned long now = millis();

  if (
      now - lastSensorUpdateMs >=
      SENSOR_UPDATE_INTERVAL_MS
  ) {
    lastSensorUpdateMs = now;
    updateSystemData();
  }

  if (
      now - lastSerialUpdateMs >=
      SERIAL_UPDATE_INTERVAL_MS
  ) {
    lastSerialUpdateMs = now;
    printSystemStatus();
  }

  delay(2);
}

// =====================================================
// Wi-Fi
// =====================================================

void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  delay(200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long connectionTimeoutMs = 20000UL;
  unsigned long startTime = millis();

  while (
      WiFi.status() != WL_CONNECTED &&
      millis() - startTime < connectionTimeoutMs
  ) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected successfully.");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Open in browser: http://");
    Serial.println(WiFi.localIP());

    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("Wi-Fi connection timed out.");
    Serial.println(
        "Controller will continue running and retry."
    );
  }
}

void maintainWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  unsigned long now = millis();

  if (
      now - lastWiFiRetryMs <
      WIFI_RETRY_INTERVAL_MS
  ) {
    return;
  }

  lastWiFiRetryMs = now;

  Serial.println("Attempting Wi-Fi reconnection...");

  WiFi.disconnect();
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// =====================================================
// HTTP server
// =====================================================

void handleHttpClient() {
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  unsigned long clientStartTime = millis();

  while (
      client.connected() &&
      !client.available() &&
      millis() - clientStartTime <
          HTTP_CLIENT_TIMEOUT_MS
  ) {
    delay(1);
  }

  if (!client.available()) {
    client.stop();
    return;
  }

  String requestLine = client.readStringUntil('\n');
  requestLine.trim();

  while (client.connected()) {
    String headerLine = client.readStringUntil('\n');
    headerLine.trim();

    if (headerLine.length() == 0) {
      break;
    }
  }

  if (requestLine.startsWith("GET /data ")) {
    sendJsonData(client);
  }

  else if (
      requestLine.startsWith("GET / ") ||
      requestLine.startsWith("GET /index.html ")
  ) {
    sendDashboardPage(client);
  }

  else {
    sendNotFound(client);
  }

  delay(1);
  client.stop();
}

void sendHttpResponse(
    WiFiClient& client,
    const String& status,
    const String& contentType,
    const String& body
) {
  client.print("HTTP/1.1 ");
  client.println(status);

  client.print("Content-Type: ");
  client.println(contentType);

  client.println("Connection: close");
  client.println("Cache-Control: no-store");
  client.println("Access-Control-Allow-Origin: *");

  client.print("Content-Length: ");
  client.println(body.length());

  client.println();
  client.print(body);
}

void sendDashboardPage(WiFiClient& client) {
  sendHttpResponse(
      client,
      "200 OK",
      "text/html; charset=utf-8",
      buildDashboardPage()
  );
}

void sendJsonData(WiFiClient& client) {
  sendHttpResponse(
      client,
      "200 OK",
      "application/json",
      buildJsonData()
  );
}

void sendNotFound(WiFiClient& client) {
  sendHttpResponse(
      client,
      "404 Not Found",
      "text/plain; charset=utf-8",
      "404: Page not found"
  );
}

// =====================================================
// JSON
// =====================================================

String buildJsonData() {
  String json;
  json.reserve(500);

  json += "{";

  json += "\"temperature\":";
  json += String(currentTempF, 2);
  json += ",";

  json += "\"humidity\":";
  json += String(currentHumidity, 2);
  json += ",";

  json += "\"bmeValid\":";
  json += bmeDataValid ? "true" : "false";
  json += ",";

  json += "\"fanEnabled\":";
  json += ENABLE_FAN_CONTROL ? "true" : "false";
  json += ",";

  json += "\"fan\":\"";
  json += jsonEscape(getFanStatus());
  json += "\",";

  json += "\"pirEnabled\":";
  json += ENABLE_PIR_SENSOR ? "true" : "false";
  json += ",";

  json += "\"motion\":\"";
  json += jsonEscape(getMotionStatus());
  json += "\",";

  json += "\"occupancy\":\"";
  json += jsonEscape(getOccupancyStatus());
  json += "\",";

  json += "\"lastMotion\":\"";
  json += jsonEscape(getLastMotionString());
  json += "\",";

  json += "\"batteryEnabled\":";
  json += ENABLE_BATTERY_MONITOR ? "true" : "false";
  json += ",";

  json += "\"batteryVoltage\":";
  json += String(currentBatteryVoltage, 2);
  json += ",";

  json += "\"fanOnTemp\":";
  json += String(fanOnTempF, 2);
  json += ",";

  json += "\"fanOffTemp\":";
  json += String(fanOffTempF, 2);
  json += ",";

  json += "\"occupancyTimeout\":";
  json += String(occupancyTimeoutMs / 60000UL);
  json += ",";

  json += "\"wifiRssi\":";
  json += String(WiFi.RSSI());

  json += "}";

  return json;
}

String jsonEscape(const String& input) {
  String output;
  output.reserve(input.length() + 8);

  for (
      size_t index = 0;
      index < input.length();
      index++
  ) {
    char character = input[index];

    if (
        character == '"' ||
        character == '\\'
    ) {
      output += '\\';
    }

    output += character;
  }

  return output;
}

// =====================================================
// Dashboard HTML
// =====================================================

String buildDashboardPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>Doghouse Monitoring System</title>

  <style>
    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      padding: 22px;
      background: #f3f5f7;
      color: #202b33;
      font-family: Arial, Helvetica, sans-serif;
    }

    .container {
      width: 100%;
      max-width: 940px;
      margin: 0 auto;
    }

    .header,
    .card,
    .settings {
      background: white;
      border-radius: 14px;
      box-shadow: 0 3px 12px rgba(0, 0, 0, 0.08);
    }

    .header {
      padding: 24px;
      margin-bottom: 18px;
    }

    h1 {
      margin: 0 0 8px;
      font-size: 28px;
    }

    .subtitle {
      margin: 0;
      color: #62717d;
    }

    .connection {
      display: flex;
      align-items: center;
      gap: 8px;
      margin-top: 14px;
      font-size: 14px;
    }

    .dot {
      width: 10px;
      height: 10px;
      border-radius: 50%;
      background: #2b9348;
    }

    .grid {
      display: grid;
      grid-template-columns:
        repeat(auto-fit, minmax(210px, 1fr));
      gap: 16px;
    }

    .card {
      min-height: 135px;
      padding: 20px;
    }

    .label {
      color: #6c7a86;
      font-size: 13px;
      font-weight: bold;
      letter-spacing: 0.05em;
      text-transform: uppercase;
    }

    .value {
      margin-top: 11px;
      font-size: 28px;
      font-weight: bold;
    }

    .detail {
      margin-top: 10px;
      color: #687782;
      font-size: 14px;
      line-height: 1.55;
    }

    .settings {
      margin-top: 16px;
      padding: 20px;
    }

    .settings h2 {
      margin: 0 0 15px;
      font-size: 20px;
    }

    .settings-grid {
      display: grid;
      grid-template-columns:
        repeat(auto-fit, minmax(180px, 1fr));
      gap: 12px;
    }

    .setting {
      padding: 14px;
      background: #f3f5f7;
      border-radius: 10px;
    }

    .setting-label {
      color: #687782;
      font-size: 13px;
    }

    .setting-value {
      margin-top: 5px;
      font-size: 19px;
      font-weight: bold;
    }

    .disabled {
      color: #8a949c;
      font-size: 18px;
    }

    .footer {
      margin-top: 18px;
      color: #74818b;
      font-size: 13px;
      text-align: center;
    }

    @media (max-width: 520px) {
      body {
        padding: 13px;
      }

      .header {
        padding: 19px;
      }

      h1 {
        font-size: 23px;
      }

      .value {
        font-size: 24px;
      }
    }
  </style>
</head>

<body>
  <main class="container">
    <section class="header">
      <h1>Doghouse Monitoring System</h1>

      <p class="subtitle">
        Live environmental and system data
      </p>

      <div class="connection">
        <span class="dot" id="connectionDot"></span>
        <span id="connectionText">Connected</span>
      </div>
    </section>

    <section class="grid">
      <article class="card">
        <div class="label">Temperature</div>

        <div class="value">
          <span id="temperature">--</span> &deg;F
        </div>
      </article>

      <article class="card">
        <div class="label">Humidity</div>

        <div class="value">
          <span id="humidity">--</span> %
        </div>
      </article>

      <article class="card">
        <div class="label">Ventilation Fan</div>
        <div class="value" id="fan">--</div>
      </article>

      <article class="card">
        <div class="label">Battery Voltage</div>

        <div class="value">
          <span id="batteryVoltage">--</span> V
        </div>
      </article>

      <article class="card">
        <div class="label">Occupancy</div>

        <div class="value" id="occupancy">--</div>

        <div class="detail">
          Motion: <span id="motion">--</span><br>
          Last motion:
          <span id="lastMotion">--</span>
        </div>
      </article>

      <article class="card">
        <div class="label">Wi-Fi Signal</div>

        <div class="value">
          <span id="wifiRssi">--</span> dBm
        </div>
      </article>
    </section>

    <section class="settings">
      <h2>Controller Settings</h2>

      <div class="settings-grid">
        <div class="setting">
          <div class="setting-label">
            Fan ON temperature
          </div>

          <div class="setting-value">
            <span id="fanOnTemp">--</span> &deg;F
          </div>
        </div>

        <div class="setting">
          <div class="setting-label">
            Fan OFF temperature
          </div>

          <div class="setting-value">
            <span id="fanOffTemp">--</span> &deg;F
          </div>
        </div>

        <div class="setting">
          <div class="setting-label">
            Occupancy timeout
          </div>

          <div class="setting-value">
            <span id="occupancyTimeout">--</span> min
          </div>
        </div>
      </div>
    </section>

    <div class="footer">
      Dashboard updates every two seconds.
    </div>
  </main>

  <script>
    function setText(id, value) {
      document.getElementById(id).textContent = value;
    }

    async function updateDashboard() {
      const dot =
        document.getElementById("connectionDot");

      const connectionText =
        document.getElementById("connectionText");

      try {
        const response = await fetch(
          "/data",
          { cache: "no-store" }
        );

        if (!response.ok) {
          throw new Error("HTTP error");
        }

        const data = await response.json();

        setText(
          "temperature",
          data.bmeValid
            ? Number(data.temperature).toFixed(2)
            : "ERROR"
        );

        setText(
          "humidity",
          data.bmeValid
            ? Number(data.humidity).toFixed(2)
            : "ERROR"
        );

        setText(
          "fan",
          data.fanEnabled
            ? data.fan
            : "Disabled for test"
        );

        document.getElementById("fan")
          .className =
          data.fanEnabled
            ? "value"
            : "value disabled";

        setText(
          "batteryVoltage",
          data.batteryEnabled
            ? Number(data.batteryVoltage).toFixed(2)
            : "--"
        );

        setText(
          "occupancy",
          data.pirEnabled
            ? data.occupancy
            : "Disabled for test"
        );

        document.getElementById("occupancy")
          .className =
          data.pirEnabled
            ? "value"
            : "value disabled";

        setText(
          "motion",
          data.pirEnabled
            ? data.motion
            : "--"
        );

        setText(
          "lastMotion",
          data.pirEnabled
            ? data.lastMotion
            : "--"
        );

        setText("wifiRssi", data.wifiRssi);

        setText(
          "fanOnTemp",
          Number(data.fanOnTemp).toFixed(2)
        );

        setText(
          "fanOffTemp",
          Number(data.fanOffTemp).toFixed(2)
        );

        setText(
          "occupancyTimeout",
          data.occupancyTimeout
        );

        dot.style.background = "#2b9348";
        connectionText.textContent = "Connected";
      }

      catch (error) {
        dot.style.background = "#c92a2a";

        connectionText.textContent =
          "Dashboard connection lost";
      }
    }

    updateDashboard();
    setInterval(updateDashboard, 2000);
  </script>
</body>
</html>
)rawliteral";
}

// =====================================================
// System updates
// =====================================================

void updateSystemData() {
  float tempC = bme.readTemperature();
  float humidity = bme.readHumidity();

  if (!isnan(tempC) && !isnan(humidity)) {
    currentTempF =
        (tempC * 9.0 / 5.0) + 32.0;

    currentHumidity = humidity;
    bmeDataValid = true;
  } else {
    bmeDataValid = false;
  }

  if (ENABLE_PIR_SENSOR) {
    updateOccupancyEstimate();
  } else {
    motionDetected = false;
  }

  if (
      ENABLE_FAN_CONTROL &&
      bmeDataValid
  ) {
    updateFanControl(currentTempF);
  } else {
    fanState = false;
    digitalWrite(FAN_CONTROL_PIN, LOW);
  }

  if (ENABLE_BATTERY_MONITOR) {
    currentBatteryVoltage =
        readBatteryVoltage();
  } else {
    currentBatteryVoltage = 0.0;
  }
}

// =====================================================
// Fan control
// =====================================================

void updateFanControl(float temperatureF) {
  if (
      !fanState &&
      temperatureF >= fanOnTempF
  ) {
    fanState = true;
  }

  if (
      fanState &&
      temperatureF <= fanOffTempF
  ) {
    fanState = false;
  }

  digitalWrite(
      FAN_CONTROL_PIN,
      fanState ? HIGH : LOW
  );
}

// =====================================================
// Occupancy
// =====================================================

void updateOccupancyEstimate() {
  int pirState = digitalRead(PIR_PIN);

  if (pirState == HIGH) {
    motionDetected = true;
    lastMotionTimeMs = millis();
  } else {
    motionDetected = false;
  }
}

String getOccupancyStatus() {
  if (!ENABLE_PIR_SENSOR) {
    return "Disabled";
  }

  if (lastMotionTimeMs == 0) {
    return "No Motion Recorded";
  }

  if (
      millis() - lastMotionTimeMs <=
      occupancyTimeoutMs
  ) {
    return "Likely Present";
  }

  return "No Recent Activity";
}

String getLastMotionString() {
  if (!ENABLE_PIR_SENSOR) {
    return "Disabled";
  }

  if (lastMotionTimeMs == 0) {
    return "Never";
  }

  unsigned long elapsedSeconds =
      (millis() - lastMotionTimeMs) / 1000UL;

  unsigned long minutes =
      elapsedSeconds / 60UL;

  unsigned long seconds =
      elapsedSeconds % 60UL;

  return String(minutes) +
         " min " +
         String(seconds) +
         " sec ago";
}

String getFanStatus() {
  if (!ENABLE_FAN_CONTROL) {
    return "Disabled";
  }

  return fanState ? "ON" : "OFF";
}

String getMotionStatus() {
  if (!ENABLE_PIR_SENSOR) {
    return "Disabled";
  }

  return motionDetected ? "YES" : "NO";
}

// =====================================================
// Battery monitoring
// =====================================================

float readBatteryVoltage() {
  unsigned long totalMillivolts = 0;

  analogReadMilliVolts(BATTERY_ADC_PIN);
  delay(2);

  for (
      int sample = 0;
      sample < BATTERY_SAMPLE_COUNT;
      sample++
  ) {
    totalMillivolts +=
        analogReadMilliVolts(BATTERY_ADC_PIN);

    delay(BATTERY_SAMPLE_DELAY_MS);
  }

  float averageMillivolts =
      totalMillivolts /
      static_cast<float>(
          BATTERY_SAMPLE_COUNT
      );

  float dividerVoltage =
      averageMillivolts / 1000.0;

  return dividerVoltage *
         BATTERY_DIVIDER_RATIO *
         BATTERY_CALIBRATION_FACTOR;
}

// =====================================================
// Preferences
// =====================================================

void loadSettings() {
  preferences.begin("settings", false);

  fanOnTempF =
      preferences.getFloat(
          "fanOn",
          DEFAULT_FAN_ON_TEMP_F
      );

  fanOffTempF =
      preferences.getFloat(
          "fanOff",
          DEFAULT_FAN_OFF_TEMP_F
      );

  occupancyTimeoutMs =
      preferences.getULong(
          "timeout",
          DEFAULT_OCCUPANCY_TIMEOUT_MS
      );

  Serial.println("Settings loaded.");
}

void saveSettings() {
  preferences.putFloat(
      "fanOn",
      fanOnTempF
  );

  preferences.putFloat(
      "fanOff",
      fanOffTempF
  );

  preferences.putULong(
      "timeout",
      occupancyTimeoutMs
  );

  Serial.println("Settings saved.");
}

void resetSettings() {
  fanOnTempF =
      DEFAULT_FAN_ON_TEMP_F;

  fanOffTempF =
      DEFAULT_FAN_OFF_TEMP_F;

  occupancyTimeoutMs =
      DEFAULT_OCCUPANCY_TIMEOUT_MS;

  saveSettings();

  Serial.println(
      "Settings reset to defaults."
  );
}

// =====================================================
// Serial output
// =====================================================

void printSystemStatus() {
  Serial.println(
      "========================================"
  );

  Serial.print("Temperature      : ");

  if (bmeDataValid) {
    Serial.print(currentTempF, 2);
    Serial.println(" F");
  } else {
    Serial.println("ERROR");
  }

  Serial.print("Humidity         : ");

  if (bmeDataValid) {
    Serial.print(currentHumidity, 2);
    Serial.println(" %");
  } else {
    Serial.println("ERROR");
  }

  Serial.println();

  Serial.print("Fan              : ");
  Serial.println(getFanStatus());

  Serial.print("Motion           : ");
  Serial.println(getMotionStatus());

  Serial.print("Occupancy        : ");
  Serial.println(getOccupancyStatus());

  Serial.print("Last Motion      : ");
  Serial.println(getLastMotionString());

  Serial.println();

  Serial.print("Battery Voltage  : ");

  if (ENABLE_BATTERY_MONITOR) {
    Serial.print(currentBatteryVoltage, 2);
    Serial.println(" V");
  } else {
    Serial.println("Disabled");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Dashboard        : http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(
        "Dashboard        : Wi-Fi disconnected"
    );
  }

  Serial.println(
      "========================================"
  );

  Serial.println();
}

// =====================================================
// Serial commands
// =====================================================

void checkSerialCommands() {
  if (Serial.available() <= 0) {
    return;
  }

  String command =
      Serial.readStringUntil('\n');

  command.trim();
  command.toUpperCase();

  if (command.startsWith("ON=")) {
    float value =
        command.substring(3).toFloat();

    if (value > fanOffTempF) {
      fanOnTempF = value;
      saveSettings();

      Serial.print(
          "Fan ON threshold updated to "
      );
      Serial.print(fanOnTempF);
      Serial.println(" F");
    } else {
      Serial.println(
          "ON threshold must be greater than OFF."
      );
    }
  }

  else if (command.startsWith("OFF=")) {
    float value =
        command.substring(4).toFloat();

    if (value < fanOnTempF) {
      fanOffTempF = value;
      saveSettings();

      Serial.print(
          "Fan OFF threshold updated to "
      );
      Serial.print(fanOffTempF);
      Serial.println(" F");
    } else {
      Serial.println(
          "OFF threshold must be less than ON."
      );
    }
  }

  else if (command.startsWith("TIMEOUT=")) {
    float value =
        command.substring(8).toFloat();

    if (value > 0.0) {
      occupancyTimeoutMs =
          static_cast<unsigned long>(
              value * 60000.0
          );

      saveSettings();

      Serial.print(
          "Occupancy timeout updated to "
      );
      Serial.print(value);
      Serial.println(" minutes");
    } else {
      Serial.println(
          "Timeout must be greater than zero."
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
        "Unknown command. Type HELP."
    );
  }
}

void printCommandMenu() {
  Serial.println("Commands:");

  Serial.println(
      "ON=85        Set fan ON temperature"
  );

  Serial.println(
      "OFF=80       Set fan OFF temperature"
  );

  Serial.println(
      "TIMEOUT=5    Set occupancy timeout"
  );

  Serial.println(
      "RESET        Restore default settings"
  );

  Serial.println(
      "HELP         Show command list"
  );

  Serial.println();
}
