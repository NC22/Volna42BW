#ifndef KellyOpenWeather_h
#define KellyOpenWeather_h

#include <Arduino.h>
#include <vector>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "KellyOWParserTools.h"
#include "KellyWeatherApi.h"

class KellyOpenWeather : public KellyWeatherApi {
    protected :
    public:
        
        KellyOpenWeather(int timeout = 5000);
        KellyOWIconType getMeteoIconState(String icon);
        int loadCurrent(String & nurl);        
};

#endif	



