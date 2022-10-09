#include <Adafruit_LC709203F.h>
#include <Arduino.h>
#include <SHTSensor.h>
#include <esp_bt.h>
#include <esp_wifi.h>

#include "BatteryMonitor.h"
#include "Configuration.h"
#include "SensorData.h"
#include "Sensors.h"
#include "StationUtils.h"
#include "utils.h"

SHTSensor sht(SHTSensor::SHT3X);
Adafruit_LC709203F* batteryMonitor;

RTC_DATA_ATTR unsigned int messageId = 0;

void setup() {
  unsigned long startTime = millis();
  disableBuiltinLed();

  Serial.begin(115200);

  SensorData data = collectData(&sht, &Serial);
  batteryMonitor = getBatteryMonitor(&Serial);
  float batteryPercent = batteryMonitor->cellPercent();
  Serial.printf("Collect data time: %lums\n", millis() - startTime);

  SensorMessage message;
  message.boardId = BOARD_ID;
  message.humidity = data.humidity;
  message.temperature = data.temperature;
  message.messageId = messageId++;
  message.battery = batteryPercent;
  message.interval = STATION_SLEEP_TIME_SECONDS;
  transmitMessage(message);

  goToDeepSleep(STATION_SLEEP_TIME_SECONDS);
}

void loop() {
  // we use deep sleep -> empty loop
}