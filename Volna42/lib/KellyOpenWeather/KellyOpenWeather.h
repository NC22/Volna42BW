#ifndef KellyOpenWeather_h
#define KellyOpenWeather_h

#include <Arduino.h>
#include <vector>

// #include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "KellyOWParserTools.h"

// class KellyOWParserListener;

enum KellyOWIconType { kowClearSky, kowFewClouds, kowScatterClouds, kowBrokenClouds, kowShowerRain, kowRain, kowThunderstorm, kowSnow, kowFog, kowUnknown };

class KellyOpenWeather {
    private :
        // JsonStreamingParser * parser;
        // KellyOWParserListener * listener;
        bool collectJSONFieldDataRaw(int paramStart, int len, String & data, String & storage);
        bool collectJSONFieldData(String fieldName, String & payload, String & storage);

        int connectionTimeout;
        // todo - load weather week plan
        
    public:
        
        bool weatherLoaded = false;
        String error;
        float temp;
        float hum;
        float pressure;
        KellyOWIconType weatherType;
        
        KellyOpenWeather(int timeout = 5000);
        KellyOWIconType getMeteoIconState(String icon);
        int loadCurrent(String & nurl);
        void end();
        
};

#endif	



