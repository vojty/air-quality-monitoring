#include <Arduino.h>

#include "Configuration.h"
#include "SHTSensor.h"
#include "SensorData.h"

TwoWire shtWire = TwoWire(SHT_BUS);

SensorData collectData(SHTSensor* sensor, HardwareSerial* serial) {
  shtWire.begin(SHT_SDA_PIN, SHT_SDL_PIN);

  if (!sensor->init(shtWire)) {
    serial->println("Cannot initialize SHT sensor.");
  }
  sensor->setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);  // only supported by SHT3x
  sensor->readSample();

  SensorData data;
  data.temperature = sensor->getTemperature();
  data.humidity = sensor->getHumidity();
  return data;
}