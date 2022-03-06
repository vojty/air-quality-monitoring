#include <Arduino_JSON.h>
#include <Credentials.h>
#include <FileResponse.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <esp_now.h>
#include <utils.h>

#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

SensorData incomingReadings;

JSONVar board;
String jsonResponse;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // Get MAC of the sender
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
           mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

  // Parse data
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  board["boardId"] = incomingReadings.boardId;
  board["temperature"] = incomingReadings.temperature;
  board["humidity"] = incomingReadings.humidity;
  board["messageId"] = String(incomingReadings.messageId);
  board["timestamp"] = timeClient.getEpochTime();  // GMT
  jsonResponse = JSON.stringify(board);

  Serial.printf("Data: %s\n", jsonResponse.c_str());

  ws.textAll(jsonResponse.c_str());
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data,
             size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      // Send last data if any
      if (jsonResponse) {
        ws.textAll(jsonResponse.c_str());
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
  // Initialize Serial Monitor
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);

  // Set device as a Wi-Fi Station
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
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
    String url = trim_gz(filepath);

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

void loop() { timeClient.update(); }