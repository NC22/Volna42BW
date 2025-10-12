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
        float temp = BAD_SENSOR_DATA; // temperature in celsius
        float hum = BAD_SENSOR_DATA; //  humidity in percents
        float pressure = BAD_SENSOR_DATA; //  pressure value in pascals format (hpa * 100)
        int bat = BAD_SENSOR_DATA;

        KellyOWIconType weatherType;
        
        KellyWeatherApi(int timeout = 5000);
        virtual KellyOWIconType getMeteoIconState(int weatherCode);
        virtual int loadCurrent(String & nurl) = 0;
        virtual int loadCurrent(String & nurl, String & token);
        virtual int loadCurrent(String & nurl, String & login, String & pass);
        virtual void end();


        virtual ~KellyWeatherApi() = default; 
};

#endif
