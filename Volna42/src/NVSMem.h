#ifndef NVSMem_h
#define NVSMem_h

#include <Arduino.h>

#if defined(ESP32)

#include <nvs_flash.h>
#include <nvs.h>
#include <EnvStructs.h>
#include <UserDefines.h>

// My replacement methods to ESP8266 RTC read functions, with similar input vars

// esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
// esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);

bool readRTCUserMemoryActualRTC(rtcData &lastState);
bool writeRTCUserMemoryActualRTC(rtcData &lastState);
bool readRTCUserMemoryNVS(uint32_t* data, size_t len);
bool writeRTCUserMemoryNVS(uint32_t* data, size_t len);
#endif

#endif