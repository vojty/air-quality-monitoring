#ifndef UTILS_H
#define UTILS_H

#include <WString.h>

void scanI2c(void);

void goToDeepSleep(int sleep_seconds);

void disableBuiltinLed(void);

String trimGz(String s);

#endif
