#ifndef UTILS_H
#define UTILS_H

#include <WiFi.h>

typedef struct SensorData {
  int boardId;
  float temperature;
  float humidity;
  int messageId;
} SensorData;

void scan_i2c(void);
void go_to_deep_sleep(int sleep_seconds);

String trim_gz(String s);

#endif
