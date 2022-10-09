#ifndef COLLECTED_SENSOR_MESSAGE_H
#define COLLECTED_SENSOR_MESSAGE_H

#include <Arduino_JSON.h>

#include "SensorMessage.h"

class CollectedSensorMessage {
  int stationId;
  float temperature;
  float humidity;
  float battery;
  int messageId;
  int receivedAt;
  int interval;

 public:
  CollectedSensorMessage() { this->stationId = 0; };

  void setValues(SensorMessage message, int receivedAt) {
    this->stationId = message.boardId;
    this->temperature = message.temperature;
    this->humidity = message.humidity;
    this->battery = message.battery;
    this->messageId = message.messageId;
    this->interval = message.interval;

    this->receivedAt = receivedAt;
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
    json["battery"] = this->battery;
    json["messageId"] = this->messageId;
    json["receivedAt"] = this->receivedAt;
    json["interval"] = this->interval;
    return JSON.stringify(json);
  }
};

#endif
