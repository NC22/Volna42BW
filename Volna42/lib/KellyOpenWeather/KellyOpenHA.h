#ifndef KellyOpenHA_h
#define KellyOpenHA_h

#include <Arduino.h>
#include <vector>

// #include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "KellyOWParserTools.h"
#include "KellyWeatherApi.h"

#if defined(ESP32)
    #include <HTTPClient.h>
#else 
    #include <ESP8266HTTPClient.h>
#endif

/* Warning - untested */
/* Implemented to replace External Sensor native method for Home Assistant responses parsing */

enum KellyHAPartialType { 
    kowHATemperature, kowHAHumidity, kowHAPressure, kowHABattery, kowHAUnknown, kowHAALL, kowHAConnectError
};

class KellyOpenHA : public KellyWeatherApi {
    protected :
        KellyHAPartialType requestProcess(String & url, String & token, bool partialOnly);
        KellyHAPartialType fillPartialData(String & payload, String & collectedData);
        float validateByUnitTemperature(float temperature, String & unitsInfo);
        float validateByUnitPressure(float pressure, String & unitsInfo);

    public:        
        KellyOpenHA(int timeout = 5000);
        KellyOWIconType getMeteoIconState(const String& collectedData);
        int loadCurrent(String & nurl);
        int loadCurrent(String & nurl, String & token);
};

#endif	



