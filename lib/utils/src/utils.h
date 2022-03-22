#ifndef UTILS_H
#define UTILS_H

#include <Arduino_JSON.h>
#include <WiFi.h>

typedef struct SensorData {
  int boardId;
  float temperature;
  float humidity;
  int messageId;
} SensorData;

class CollectedSensorData {
  int stationId;
  float temperature;
  float humidity;
  int messageId;
  int timestamp;

 public:
  CollectedSensorData() { this->stationId = 0; };

  void setValues(SensorData data, int timestamp) {
    this->stationId = data.boardId;
    this->temperature = data.temperature;
    this->humidity = data.humidity;
    this->messageId = data.messageId;
    this->timestamp = timestamp;
  };

  bool isValid() {
    // zero is default value
    return this->stationId > 0;
  }

  String toJson() {
    JSONVar json;
    json["type"] = "STATION";
    json["stationId"] = this->stationId;
    json["temperature"] = this->temperature;
    json["humidity"] = this->humidity;
    json["messageId"] = this->messageId;
    json["timestamp"] = this->timestamp;
    return JSON.stringify(json);
  }
};

void scanI2c(void);

void goToDeepSleep(int sleep_seconds);

void disableBuiltinLed(void);

String trimGz(String s);

#endif
