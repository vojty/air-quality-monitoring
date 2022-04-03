#ifndef SENSORS_H
#define SENSORS_H

#include "SHTSensor.h"
#include "SensorData.h"

SensorData collectData(SHTSensor* sensor, HardwareSerial* serial);

#endif
