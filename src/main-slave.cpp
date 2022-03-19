#include <Config.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <utils.h>

// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 1

#include <forcedClimate.h>

ForcedClimate climateSensor = ForcedClimate(Wire, BME280_I2C_ADDRESS);

// MAC Address of the receiver
uint8_t broadcastAddress[] = {0x94, 0x3c, 0xc6, 0x05, 0x27, 0xac};

RTC_DATA_ATTR unsigned int messageId = 0;

SensorData collectData() {
  climateSensor.begin();
  climateSensor.takeForcedMeasurement();

  SensorData data;
  data.boardId = BOARD_ID;
  data.temperature = climateSensor.getTemperatureCelcius();
  data.humidity = climateSensor.getRelativeHumidity();
  data.messageId = messageId++;

  Serial.printf("\nData: temperature=%f, humidity=%f, messageId=%d\n\n", data.temperature, data.humidity,
                data.messageId);

  return data;
}

void setup() {
  unsigned long startTime = millis();
  disableBuiltinLed();

  // Init Serial Monitor
  Serial.begin(115200);
  Serial.printf("MAC address: %s\n", WiFi.macAddress().c_str());

  // Init Wifi
  WiFi.mode(WIFI_STA);

  SensorData data = collectData();
  Serial.printf("Collect data time: %lums\n", millis() - startTime);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, sizeof(broadcastAddress));
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Send data
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&data, sizeof(data));
  Serial.printf("Total time: %lums\n", millis() - startTime);

  if (result == ESP_OK) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Error sending the data");
  }

  // Sleep
  goToDeepSleep(SLAVE_SLEEP_TIME_SECONDS);
}

void loop() {
  // empty since we use deep sleep
}