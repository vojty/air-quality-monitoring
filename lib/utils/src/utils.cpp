#include "utils.h"

#include <esp_bt.h>
#include <esp_wifi.h>

#include "Arduino.h"
#include "Wifi.h"
#include "Wire.h"
#include "driver/adc.h"

/* Conversion factor for micro seconds to seconds */
#define uS_TO_S_FACTOR 1000000

void scanI2c() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("done\n");
  }
}

// https://savjee.be/2019/12/esp32-tips-to-increase-battery-life/
void goToDeepSleep(int sleep_seconds) {
  // the original code from the website crashes :/
  adc_power_release();
  esp_sleep_enable_timer_wakeup(sleep_seconds * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void disableBuiltinLed() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

String trimGz(String s) {
  if (s.endsWith(".gz")) {
    int end = s.lastIndexOf(".");
    return s.substring(0, end);
  } else {
    return s;
  }
}
