#ifndef NVSMem_h
#define NVSMem_h

#include <Arduino.h>

#if defined(ESP32)

#include <nvs_flash.h>
#include <nvs.h>

bool readRTCUserMemory(uint32_t index, uint32_t* data, size_t len);
bool writeRTCUserMemory(uint32_t index, uint32_t* data, size_t len);
#endif

#endif