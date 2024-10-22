#ifndef KellyOpenMeteo_h
#define KellyOpenMeteo_h

#include <Arduino.h>
#include <vector>

// #include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "KellyOWParserTools.h"
#include "KellyWeatherApi.h"

class KellyOpenMeteo : public KellyWeatherApi {
    protected :
    public:        
        KellyOpenMeteo(int timeout = 5000);
        KellyOWIconType getMeteoIconState(int weatherCode);
        int loadCurrent(String & nurl);
};

#endif	



