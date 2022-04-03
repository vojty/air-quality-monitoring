#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "Configuration.h"
#include "SensorMessage.h"

uint8_t broadcastAddress[6];

void transmitMessage(SensorMessage message) {
  WiFi.mode(WIFI_STA);

  // Parse MAC address from config
  sscanf(HQ_MAC_ADDRESS, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &broadcastAddress[0], &broadcastAddress[1],
         &broadcastAddress[2], &broadcastAddress[3], &broadcastAddress[4], &broadcastAddress[5]);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, sizeof(broadcastAddress));
  peerInfo.encrypt = false;
  peerInfo.channel = 1;
  peerInfo.ifidx = ESP_IF_WIFI_STA;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.printf("Sending: { temperature: %f, humidity: %f, messageId: %d, battery: %f }\n", message.temperature,
                message.humidity, message.messageId, message.battery);

  // Send data
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&message, sizeof(message));
  if (result == ESP_OK) {
    Serial.println("Successfully sent");
  } else {
    Serial.printf("Error sending the message, error=%d\n", result);
  }
}
