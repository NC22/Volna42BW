#ifndef KellyDomoticz_h
#define KellyDomoticz_h

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


class KellyOpenDomoticz : public KellyWeatherApi {
    protected :
        
        WiFiClient client;
        HTTPClient http;

        float validateByUnitTemperature(float temperature, String & unitsInfo);
        float validateByUnitPressure(float pressure, String & unitsInfo);
        void closeConnection();

    public:        
        KellyOpenDomoticz(int timeout = 5000);
        KellyOWIconType getMeteoIconState(const String& collectedData);
        int loadCurrent(String & nurl);
        int loadCurrent(String & nurl, String & login,  String & pass);
        bool retrySubData = true; // retry once addition requests
};

#endif	



