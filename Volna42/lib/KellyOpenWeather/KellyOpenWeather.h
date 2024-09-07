#ifndef KellyOpenWeather_h
#define KellyOpenWeather_h

#include <Arduino.h>
#include <vector>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "KellyOWParserTools.h"

// class KellyOWParserListener;

enum KellyOWIconType { kowClearSky, kowFewClouds, kowScatterClouds, kowBrokenClouds, kowShowerRain, kowRain, kowThunderstorm, kowSnow, kowFog, kowUnknown };

class KellyOpenWeather {
    private :
        // JsonStreamingParser * parser;
        // KellyOWParserListener * listener;
        bool parseURL(String &host, int &port, String &path);
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
        
        String url;
        KellyOpenWeather(String url = "", int timeout = 5000);
        KellyOWIconType getMeteoIconState(String icon);
        int loadCurrent();
        void end();
        
};

#endif	



