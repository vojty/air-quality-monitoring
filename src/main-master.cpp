#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <ErriezMHZ19B.h>
#include <NTPClient.h>
#include <SPIFFS.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>

#include "CollectedSensorMessage.h"
#include "Configuration.h"
#include "FileResponse.h"
#include "SensorMessage.h"
#include "utils.h"

#define MHZ19B_TX_PIN D3
#define MHZ19B_RX_PIN D2

// Static IP default values
IPAddress localIp;
IPAddress gateway;
IPAddress subnet;
// DNS is needed as well
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

// Init serial communication for MH-Z19B
SoftwareSerial mhzSerial(MHZ19B_TX_PIN, MHZ19B_RX_PIN);
ErriezMHZ19B mhz19b(&mhzSerial);

// Refactor these
JSONVar mainBoard;
String mainJsonResponse;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

CollectedSensorMessage sensorsCache[STATIONS_COUNT];

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

void sendJson(String json) {
  Serial.printf("Data: %s\n", json.c_str());
  ws.textAll(json.c_str());
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // Get MAC of the sender
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
           mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

  SensorMessage receivedData;

  // Parse data
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // Store data so we can send them when a client reconnects
  CollectedSensorMessage *data = &sensorsCache[receivedData.boardId];
  data->setValues(receivedData, timeClient.getEpochTime());
  String json = data->toJson();

  sendJson(json);
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data,
             size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      // Send last data if any
      for (CollectedSensorMessage cacheData : sensorsCache) {
        if (cacheData.isValid()) {
          sendJson(cacheData.toJson());
        }
      }

      if (mainJsonResponse) {
        sendJson(mainJsonResponse);
      }
      break;
    case WS_EVT_DISCONNECT:
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  disableBuiltinLed();
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  localIp.fromString(MASTER_IP_ADDRESS);
  gateway.fromString(MASTER_GATEWAY);
  subnet.fromString(MASTER_SUBNET);

  mhzSerial.begin(9600);
  // Optional: Detect MH-Z19B sensor (check wiring / power)
  while (!mhz19b.detect()) {
    Serial.println(F("Detecting MH-Z19B sensor..."));
    delay(2000);
  };

  WiFi.mode(WIFI_AP_STA);
  if (!WiFi.config(localIp, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure static IP address");
  }
  // Set device as a Wi-Fi Station
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Station MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  // Enable NTP
  timeClient.begin();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  initWebSocket();

  // Register ESP-NOW receive callback
  esp_now_register_recv_cb(OnDataRecv);

  // Register available assets
  File root = SPIFFS.open("/assets");
  while (File file = root.openNextFile()) {
    String filepath = String(file.name());
    String url = trimGz(filepath);

    server.on(url.c_str(), HTTP_GET, [filepath](AsyncWebServerRequest *request) {
      FileResponse *response = new FileResponse(SPIFFS, filepath.c_str());
      request->send(response);
    });
  }

  // Index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    FileResponse *response = new FileResponse(SPIFFS, "/index.html.gz", "text/html");
    request->send(response);
  });

  server.begin();
}

void printErrorCode(int16_t result) {
  // Print error code
  switch (result) {
    case MHZ19B_RESULT_ERR_CRC:
      Serial.println(F("CRC error"));
      break;
    case MHZ19B_RESULT_ERR_TIMEOUT:
      Serial.println(F("RX timeout"));
      break;
    default:
      Serial.print(F("Error: "));
      Serial.println(result);
      break;
  }
}

void loop() {
  // Update NTP
  timeClient.update();

  int16_t result;

  // Minimum interval between CO2 reads is required
  if (mhz19b.isReady()) {
    // Read CO2 concentration from sensor
    result = mhz19b.readCO2();

    // Print result
    if (result < 0) {
      // An error occurred
      printErrorCode(result);
    } else {
      mainBoard["type"] = "MASTER";
      mainBoard["timestamp"] = timeClient.getEpochTime();
      mainBoard["co2"] = result;
      mainJsonResponse = JSON.stringify(mainBoard);
      sendJson(mainJsonResponse);
    }
  }
}