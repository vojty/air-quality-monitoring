#ifndef SENSOR_MESSAGE_H
#define SENSOR_MESSAGE_H

typedef struct SensorMessage {
  int boardId;
  float temperature;
  float humidity;
  int messageId;
  float battery;
  int interval;
} SensorMessage;

#endif
