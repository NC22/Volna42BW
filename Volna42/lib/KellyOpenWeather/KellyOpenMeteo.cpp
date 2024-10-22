#include "KellyOpenMeteo.h"

KellyOpenMeteo::KellyOpenMeteo(int timeout): KellyWeatherApi(timeout) {

}

/*
  Get data from Open-Meteo API    
  https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&current=temperature_2m,weather_code,relative_humidity_2m&format=json&timeformat=unixtime
  
  result code : 
  200 - success
  -1 - fail to connect to url
  -2 - fail to parse url
  2, 4 - response parsing errors
  5 - server returned "error" message
  6 - fail to read response headers
  7 - empty body
*/
int KellyOpenMeteo::loadCurrent(String & nurl) {
    
  Serial.println(F("[Weather API] - Open-Meteo parser"));
  weatherLoaded = false;
  error = "";

  int port;
  String host, path;

  if (KellyOWParserTools::parseURL(nurl, host, port, path)) {
    
    WiFiClient *client = new WiFiClient;
    
    #if defined(ESP32)
        client->setTimeout(connectionTimeout / 1000);
    #else 
        client->setTimeout(connectionTimeout);
    #endif

    bool connected = client->connect(host.c_str(), port);

    if (connected) {
      Serial.println(F("[OpenMeteo] Connected to server...read response..."));

      String tmp = String("GET ") + path + " HTTP/1.1\r\n" + 
                   "Host: " + host + "\r\n" + 
                   "Connection: close\r\n\r\n";

      client->print(tmp);
      uint16_t code, contentLength; 
      KellyOWParserTools::clientReadHeaders(code, contentLength, client, connectionTimeout);
      
      if (code > 0) {

        Serial.print(F("[OpenMeteo] Headers received | HTTP RESPONSE Code : ")); Serial.print(code);
        Serial.print(F(" | Content-length : ")); Serial.println(contentLength);

      } else {
        error = "[OpenMeteo] Headers empty | No HTTP Code detected";
        KellyOWParserTools::clientEnd(client, nullptr);
        return 6;
      }

      if (contentLength > 2000 || contentLength == 0) {
        contentLength = 2000;
        Serial.print(F("[OpenMeteo] Unknown content length -> read max amount or exit by Timeout"));
      }

      KellyOWParserTools::clientReadBody(tmp, contentLength, client, connectionTimeout);
      KellyOWParserTools::clientEnd(client, nullptr);

      if (tmp.length() <= 0) {
        error = "[OpenMeteo] Empty body";
        return 7;
      }

      String collectedData;
      if (KellyOWParserTools::collectJSONFieldData("error", tmp, collectedData)) {
          error = collectedData;
          return 5;
      }

      if (KellyOWParserTools::collectJSONFieldData("current", tmp, collectedData)) {

        // Serial.println("KellyOpenMeteo");
        //  Serial.println(collectedData);
        
            tmp = collectedData;

          if (KellyOWParserTools::collectJSONFieldData("temperature_2m", tmp, collectedData)) {
              temp = KellyOWParserTools::validateFloatVal(collectedData);
          }

          if (KellyOWParserTools::collectJSONFieldData("relative_humidity_2m", tmp, collectedData)) {
              hum = KellyOWParserTools::validateFloatVal(collectedData);
          }
          
          if (KellyOWParserTools::collectJSONFieldData("weather_code", tmp, collectedData)) {
              weatherType = getMeteoIconState(KellyOWParserTools::validateIntVal(collectedData));
          }

          if (temp <= BAD_SENSOR_DATA) {                    
              error = "[OpenMeteo] Parse _Temperature_ value - Fail";
              return 4;
          } else {
              weatherLoaded = true;
          }
      } else {
          error = "[OpenMeteo] No _current_ data block found";
          return 2;
      }

      return 200;
      
    } else {
      Serial.println(F("[OpenMeteo] Connection failed!"));
      return -1;
    }
    
  } else {
    Serial.println(F("[OpenMeteo] Failed to parse URL"));
    return -2;
  }
}

KellyOWIconType KellyOpenMeteo::getMeteoIconState(int weatherCode) {
    switch (weatherCode) {
        case 0: // Ясное небо
            Serial.println(F("kowClearSky"));
            return kowClearSky;

        case 1: // В основном ясно
        case 2: // Частично облачно
            Serial.println(F("kowFewClouds"));
            return kowFewClouds;

        case 3: // Переменная облачность
        case 4: // Облачно
            Serial.println(F("kowScatterClouds"));
            return kowScatterClouds;

        case 45: // Туман
        case 48: // Иней или изморозь
            Serial.println(F("kowFog"));
            return kowFog;

        case 51: // Легкая морось
        case 53: // Умеренная морось
        case 55: // Сильная морось
            Serial.println(F("kowShowerRain"));
            return kowShowerRain;

        case 61: // Легкий дождь
        case 63: // Умеренный дождь
        case 65: // Сильный дождь
            Serial.println(F("kowRain"));
            return kowRain;

        case 66: // Легкий дождь с замерзанием
        case 67: // Сильный дождь с замерзанием
        case 71: // Легкий снегопад
        case 73: // Умеренный снегопад
        case 75: // Сильный снегопад
            Serial.println(F("kowSnow"));
            return kowSnow;

        case 80: // Легкий ливень
        case 81: // Умеренный ливень
        case 82: // Сильный ливень
            Serial.println(F("kowShowerRain"));
            return kowShowerRain;

        case 85: // Легкий снежный ливень
        case 86: // Сильный снежный ливень
            Serial.println(F("kowSnow"));
            return kowSnow;

        case 95: // Гроза
        case 96: // Гроза с легким градом
        case 99: // Гроза с сильным градом
            Serial.println(F("kowThunderstorm"));
            return kowThunderstorm;

        default: // Неизвестный код
            Serial.println(F("kowUnknown"));
            return kowUnknown;
    }
}