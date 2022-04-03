#include "Adafruit_LC709203F.h"
#include "BatteryMonitor.h"
#include "Configuration.h"

Adafruit_LC709203F *batteryMonitor;

void setup() {
  Serial.begin(115200);

  batteryMonitor = getBatteryMonitor(&Serial);
}

void loop() {
  delay(2000);  // dont query too often!

  Serial.print("Batt Voltage: ");
  Serial.println(batteryMonitor->cellVoltage(), 3);
  Serial.printf("Batt Percent: %.1f %\n", batteryMonitor->cellPercent());
}