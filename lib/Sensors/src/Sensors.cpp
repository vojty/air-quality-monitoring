#include <Arduino.h>

#include "SHTSensor.h"
#include "SensorData.h"

SensorData collectData(SHTSensor* sensor, HardwareSerial* serial) {
  if (!sensor->init()) {
    serial->println("Cannot initialize SHT sensor.");
  }
  sensor->setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);  // only supported by SHT3x
  sensor->readSample();

  SensorData data;
  data.temperature = sensor->getTemperature();
  data.humidity = sensor->getHumidity();
  return data;
}