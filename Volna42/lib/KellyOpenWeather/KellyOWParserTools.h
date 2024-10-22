#ifndef KellyOWParserTools_H
#define KellyOWParserTools_H

#include <Arduino.h>

#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#if !defined(BAD_SENSOR_DATA)
    #define BAD_SENSOR_DATA -1000.0
#endif

enum KellyOWIconType { 
    kowClearSky, kowFewClouds, kowScatterClouds, kowBrokenClouds, kowShowerRain, kowRain, kowThunderstorm, kowSnow, kowFog, kowUnknown 
};

enum KellyOWStringType { 
    kJSONString, kJSONMix, kJSONArray, kJSONObject 
};

// HTTP Response \ String parser tools

class KellyOWParserTools {
public:
    static float validateFloatVal(String v);
    static int validateIntVal(String v);
    static bool collectJSONFieldDataRaw(int paramStart, int len, String &data, String &storage, unsigned int maxLength = 255);
    static bool collectJSONFieldData(String fieldName, String & payload, String & storage, unsigned int maxLength = 255);
    static void clientReadHeaders(uint16_t &code, uint16_t &contentLength, WiFiClient * client, unsigned int maxIdleTime) ;
    static void clientReadBody(String &out, uint16_t size, WiFiClient * client, unsigned int maxIdleTime) ;
    static void clientEnd(WiFiClient * client, WiFiClientSecure * clientSecure);
    static void clientEnd(WiFiClient * client);
    static void clientEnd(WiFiClientSecure * clientSecure);
    static bool parseURL(String &url, String &host, int &port, String &path);
    static String sanitizeResponse(String var);
};

#endif 