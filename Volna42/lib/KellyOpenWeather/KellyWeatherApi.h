#ifndef __KellyWeatherApi_H_
#define __KellyWeatherApi_H_

#include <Arduino.h>
#include <KellyOWParserTools.h>


class KellyWeatherApi {
    protected :
        int connectionTimeout;

         WiFiClient * client = NULL;
         WiFiClientSecure * clientSecure = NULL;

         void clientStart(bool https);
         void clientEnd();
    public:
        
        bool weatherLoaded = false;
        String error;
        float temp = BAD_SENSOR_DATA;
        float hum = BAD_SENSOR_DATA;
        float pressure = BAD_SENSOR_DATA;
        KellyOWIconType weatherType;
        
        KellyWeatherApi(int timeout = 5000);
        virtual KellyOWIconType getMeteoIconState(int weatherCode);
        virtual int loadCurrent(String & nurl) = 0;
        virtual void end();

        virtual ~KellyWeatherApi() = default; 
};

#endif
