#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "Adafruit_LC709203F.h"
#include "HardwareSerial.h"

Adafruit_LC709203F* getBatteryMonitor(HardwareSerial* serial);

#endif
