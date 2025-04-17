#if !defined(ExternalSensor_H)
#define ExternalSensor_H

#include <Arduino.h>

#include <UserDefines.h>
#include <SystemDefines.h>
#include <EnvStructs.h>
#include <KellyOpenWeather.h>
#include <KellyOpenMeteo.h>
#include <KellyOpenHA.h>

#if defined(ESP32)
    #include <HTTPClient.h>
#else 
    #include <ESP8266HTTPClient.h>
#endif

class ExternalSensor {
public:
    static bool requestData(String &url, String &login, String &pass, externalSensorData &resultData, String &error, int8_t attempt = 0);
};


#endif