#ifndef deepSleepFix_h
#define deepSleepFix_h

#if defined(ESP8266)

#include <Arduino.h>
extern "C" {
  #include "user_interface.h"
}
// Fixed deep sleep methods implemented by nikolz0 that can work for some cheap ESP8266 D1 Mini boards that have issue with native deepSleep function
// https://github.com/esp8266/Arduino/issues/6318

#define ets_wdt_disable ((void (*)(void))0x400030f0)
#define ets_delay_us ((void (*)(int))0x40002ecc)

void nkDeepsleep(uint32 t_us, unsigned int method);

#endif

#endif