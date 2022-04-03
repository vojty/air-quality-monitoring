#include "BatteryMonitor.h"

#include "Adafruit_LC709203F.h"
#include "Configuration.h"
#include "HardwareSerial.h"
#include "Wire.h"

// Has to be global, dunno why
TwoWire batteryMonitorWire = TwoWire(0);

Adafruit_LC709203F* getBatteryMonitor(HardwareSerial* serial) {
  Adafruit_LC709203F* batteryMonitor = new Adafruit_LC709203F();

  batteryMonitorWire.begin(BATTERY_MONITOR_SDA_PIN, BATTERY_MONITOR_SDL_PIN);

  if (!batteryMonitor->begin(&batteryMonitorWire)) {
    serial->println(F("Couldnt find Adafruit LC709203F?\nMake sure a battery is plugged in!"));
    while (1) delay(10);
  }
  batteryMonitor->setPackSize(LC709203F_APA_3000MAH);

  return batteryMonitor;
}
