

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <SensirionI2CScd4x.h>
#include <WiFi.h>
#include <Wire.h>
#include <pm1006.h>

#include "Configuration.h"

// PM1006 sensor
#define RXD2 16
#define TXD2 17
#define PIN_FAN 12

// LEDs
#define PIN_LED 25
#define LED_TOP 2
#define LED_CENTER 1
#define LED_BOTTOM 0
#define BRIGHTNESS 10

#define IP_ADDRESS "192.168.0.202"
#define GATEWAY "192.168.0.1"
#define SUBNET "255.255.255.0"
#define DEVICE_HOSTNAME "IKEA Vindriktning"
#define DEFAULT_SLEEP_SECONDS 50

// Webserver
IPAddress localIp;
IPAddress gateway;
IPAddress subnet;
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
AsyncWebServer server(80);

// LEDs
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(3, PIN_LED);
// Sensors
PM1006 pm1006(&Serial2);
SensirionI2CScd4x scd4x;

Preferences preferences;

StaticJsonDocument<250> jsonDocument;

struct Measurements {
  // From PM1006
  uint16_t pm2_5;  // μg/m3

  // From SCD4x
  uint16_t co2;       // ppm
  float temperature;  // °C
  float humidity;     // %
};

Measurements measurements;

void setLedColor(byte r, byte g, byte b, int id) {
  uint32_t rgb = rgbWS.Color(r, g, b);
  rgbWS.setPixelColor(id, rgb);
  rgbWS.show();
}

void toggleFan(bool enable) {
  if (enable) {
    digitalWrite(PIN_FAN, HIGH);
    Serial.println("Fan ON");
  } else {
    digitalWrite(PIN_FAN, LOW);
    Serial.println("Fan OFF");
  }
}

void readPM1006() {
  // Try to measure
  while (!pm1006.read_pm25(&measurements.pm2_5)) {
    delay(10);
  }
}

void readSDC41() { scd4x.readMeasurement(measurements.co2, measurements.temperature, measurements.humidity); }

int getSleepTime() { return preferences.getInt("sleep_time", DEFAULT_SLEEP_SECONDS); }
int setSleepTime(int sleep_time) { return preferences.putInt("sleep_time", sleep_time); }

void setup() {
  Serial.begin(115200);

  // Init PM1006 sensor
  pinMode(PIN_FAN, OUTPUT);  // init fan
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  preferences.begin("settings", false);

  // Init SCD41 sensor
  Wire.begin();
  scd4x.begin(Wire);
  scd4x.stopPeriodicMeasurement();
  scd4x.startPeriodicMeasurement();

  // Init LEDs
  rgbWS.begin();  // WS2718
  rgbWS.setBrightness(BRIGHTNESS);

  // Init WebServer
  WiFi.mode(WIFI_AP_STA);  // both station and access point
  WiFi.setHostname(DEVICE_HOSTNAME);
  localIp.fromString(IP_ADDRESS);
  gateway.fromString(GATEWAY);
  subnet.fromString(SUBNET);
  if (!WiFi.config(localIp, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure static IP address");
  }
  // Set device as a Wi-Fi Station
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to Wi-Fi...");
  }

  // Index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument json(512);
    json["humidity"] = measurements.humidity;
    json["pm2_5"] = measurements.pm2_5;
    json["co2"] = measurements.co2;
    json["temperature"] = measurements.temperature;
    json["sleep_time"] = getSleepTime();

    String jsonString;
    serializeJson(json, jsonString);
    request->send(200, "application/json", jsonString);
  });

  // AsyncCallbackJsonWebHandler automatically parses the JSON body and calls the provided callback
  AsyncCallbackJsonWebHandler *handler =
      new AsyncCallbackJsonWebHandler("/rest/endpoint", [](AsyncWebServerRequest *request, JsonVariant &json) {
        jsonDocument = json.as<JsonObject>();
        int sleep_time = jsonDocument["sleep_time"];
        Serial.printf("Received %d\n", sleep_time);
        jsonDocument.clear();

        if (sleep_time) {
          setSleepTime(sleep_time);
          jsonDocument["status"] = "OK";
        } else {
          jsonDocument["status"] = "NOK";
        }

        AsyncResponseStream *response = request->beginResponseStream("application/json");
        serializeJson(jsonDocument, *response);
        request->send(response);
      });
  server.addHandler(handler);

  server.begin();

  // Setup task runner

  // Serial.println("Start...");
  // delay(500);
  // Serial.println("1. LED Green");
  // setLedColor(0, 255, 0, LED_TOP);
  // delay(1000);
  // Serial.println("2. LED Green");
  // setLedColor(0, 255, 0, LED_CENTER);
  // delay(1000);
  // Serial.println("3. LED Green");
  // setLedColor(0, 255, 0, LED_BOTTOM);
  // delay(1000);
  // setLedColor(0, 0, 0, 1);
  // setLedColor(0, 0, 0, 2);
  // setLedColor(0, 0, 0, 3);
}

void collectData() {
  toggleFan(true);
  sleep(10);
  toggleFan(false);

  readPM1006();
  readSDC41();

  Serial.printf("PM2.5: %d µg/m3\n", measurements.pm2_5);
  Serial.printf("CO2: %dppm\n", measurements.co2);
  Serial.printf("Temperature: %f C\n", measurements.temperature);
  Serial.printf("Humidity: %f%%\n", measurements.humidity);
}

bool isBetween(float value, float min, float max) { return value >= min && value <= max; }
bool isBetween(uint16_t value, uint16_t min, uint16_t max) { return value >= min && value <= max; }

void handleLeds() {
  // https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
  if (isBetween(measurements.co2, 0, 400)) {
    setLedColor(0, 255, 127, LED_TOP);  // spring green
  } else if (isBetween(measurements.co2, 400, 1000)) {
    setLedColor(0, 255, 0, LED_TOP);  // green
  } else if (isBetween(measurements.co2, 1000, 2000)) {
    setLedColor(255, 255, 0, LED_TOP);  // yellow
  } else {
    setLedColor(255, 0, 0, LED_TOP);  // red
  }

  // https://aqicn.org/faq/2013-09-09/revised-pm25-aqi-breakpoints/
  // Good              0.0 - 12.0
  // Moderate         12.1 - 35.4
  // Unhealthy        35.5 - 55.4
  // Very Unhealthy   55.5 - 150.4
  // Hazardous       150.5
  if (isBetween(measurements.pm2_5, 0, 12)) {
    setLedColor(0, 255, 0, LED_BOTTOM);  // green
  } else if (isBetween(measurements.pm2_5, 12, 35.4)) {
    setLedColor(255, 255, 0, LED_BOTTOM);  // yellow
  } else if (isBetween(measurements.pm2_5, 35.5, 55.4)) {
    setLedColor(255, 140, 0, LED_BOTTOM);  // orange
  } else {
    setLedColor(255, 0, 0, LED_BOTTOM);  // red
  }
}

void loop() {
  collectData();
  handleLeds();
  sleep(getSleepTime());
}
