#ifndef UTILS_H
#define UTILS_H

#include <WiFi.h>

typedef struct SensorData {
  int boardId;
  float temperature;
  float humidity;
  int messageId;
} SensorData;

void scanI2c(void);

void goToDeepSleep(int sleep_seconds);

void disableBuiltinLed(void);

String trimGz(String s);

#endif
