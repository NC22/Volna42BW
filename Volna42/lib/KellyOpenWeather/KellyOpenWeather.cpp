#include "KellyOpenWeather.h"

KellyOpenWeather::KellyOpenWeather(int timeout) {
  
    connectionTimeout = timeout;
    // parser = new JsonStreamingParser();
    // KellyOWParserListener parseListener = KellyOWParserListener();

    //listener = &parseListener;
   //   listener->controller = this;
   // parser->setListener(&parseListener);
}

void KellyOpenWeather::end() {
  // free resources
}

/*
  Get data from OpenWeather API

  result code : 
  200 - success
  -1 - fail to connect to url
  -2 - fail to parse url
  1-4 - response parsing errors
  5 - response "cod" value != 200
*/
int KellyOpenWeather::loadCurrent(String & nurl) {
  
  weatherLoaded = false;
  error = "";
  // #if defined(ESP8266)
  // if (client.connect(host, port)) {
  // #else
  // if (client.connect(host.c_str(), port)) {
  // #endif
  
  int port;
  bool https = false;
  String host;
  String path;

  if (parseURL(nurl, host, port, path)) {

    if (port == 443) https = true;
    
    WiFiClientSecure * clientSecure = NULL;
    WiFiClient * client = NULL;
    
    if (https) {
      clientSecure = new WiFiClientSecure;
      #if defined(ESP32)
        clientSecure->setTimeout(connectionTimeout / 1000);
      #else 
        clientSecure->setTimeout(connectionTimeout);
      #endif
    } else {
      client = new WiFiClient;   
      #if defined(ESP32)
        client->setTimeout(connectionTimeout / 1000);
      #else 
        client->setTimeout(connectionTimeout);
      #endif
    }

    bool connected = false;
    if (https) {
      clientSecure->setInsecure();  
      connected = clientSecure->connect(host.c_str(), port);
    } else {
      connected = client->connect(host.c_str(), port);
    }

    if (connected) {
      Serial.println(F("[OpenWeather] Connected to server...read response..."));

      String tmp = String("GET ") + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
      host = "";
      path = "";

      if (https) {
        clientSecure->print(tmp);
      } else {
        client->print(tmp);
      }

      uint16_t code, contentLength;
      KellyOWParserTools::clientReadHeaders(code, contentLength, client, clientSecure, connectionTimeout);
      if (tmp.length() > 0) {
        Serial.print(F("[OpenWeather] Headers received | HTTP RESPONSE Code : ")); Serial.print(code);
        Serial.print(F(" | Content-length : ")); Serial.println(contentLength);
      } else {
        Serial.print(F("[OpenWeather] Headers empty"));
      }

      if (contentLength > 2000 || contentLength == 0) {
        contentLength = 2000;
        Serial.print(F("[OpenWeather] Unknown content length -> read max amount or exit by Timeout"));
      }

      if (contentLength > 0) {

        KellyOWParserTools::clientReadBody(tmp, contentLength, client, clientSecure, connectionTimeout);

        // Serial.println(F("RESPONSE : :: "));
        // Serial.println(tmp);

      } else {        
        Serial.print(F("[OpenWeather] Empty body "));
      }


      String collectedData;
      if (KellyOWParserTools::collectJSONFieldData("cod", tmp, collectedData)) {

          if (collectedData != "200") {
              KellyOWParserTools::collectJSONFieldData("message", tmp, collectedData);
              error = collectedData;
              return 5;
          } 

          weatherType = kowUnknown;
          if (KellyOWParserTools::collectJSONFieldData("weather", tmp, collectedData)) {
              if (KellyOWParserTools::collectJSONFieldData("icon", tmp, collectedData)) {
                  weatherType = getMeteoIconState(collectedData);
              }
          }

          if (KellyOWParserTools::collectJSONFieldData("main", tmp, collectedData)) {
              if (KellyOWParserTools::collectJSONFieldData("temp", tmp, collectedData)) {

                  temp = KellyOWParserTools::validateFloatVal(collectedData);
                  KellyOWParserTools::collectJSONFieldData("humidity", tmp, collectedData);
                  hum = KellyOWParserTools::validateFloatVal(collectedData);
                  KellyOWParserTools::collectJSONFieldData("pressure", tmp, collectedData);
                  pressure = KellyOWParserTools::validateFloatVal(collectedData);
                  if (pressure <= -1000) {

                  } else {
                    pressure = pressure * 100.0f;
                  }

                  if (temp <= -1000) {                    
                      error = "Parse Temp fail";
                      return 4;
                  } else {
                      weatherLoaded = true;
                  }

              } else {
                error = "Temp read fail";
                return 3;
              }
          } else {
              error = "Main block not found";
              return 2;
          }

      } else {
        error = "Response not contain code number";
        return 1;
      }

      if (https) {
        if (clientSecure) {
          clientSecure->stop();
          delete clientSecure;
        }
      } else {
        if (client) {
          client->stop();
          delete client;
        }
      }

      return 200;
      
    } else {
      Serial.println(F("[OpenWeather] Connection failed!"));
      return -1;
    }
    
  } else {
    Serial.println(F("[OpenWeather] Failed to parse URL"));
    return -2;
  }
  
}

KellyOWIconType KellyOpenWeather::getMeteoIconState(String icon)
{
  if (icon == "01d" || icon == "01n") {
    Serial.println(F("kowClearSky"));
    return kowClearSky;
  }

  if (icon == "02d" || icon == "02n") {
    Serial.println(F("kowFewClouds"));
    return kowFewClouds;
  }

  if (icon == "03d" || icon == "03n") {
    Serial.println(F("kowScatterClouds"));
    return kowScatterClouds;
  }

  if (icon == "04d" || icon == "04n") {
    Serial.println(F("kowBrokenClouds"));
    return kowBrokenClouds;
  }

  if (icon == "09d" || icon == "09n") {
    Serial.println(F("kowShowerRain"));
    return kowShowerRain;
  }
  
  if (icon == "10d" || icon == "10n") {
    Serial.println(F("kowRain"));
    return kowRain;
  }

  if (icon == "11d" || icon == "11n") {
    Serial.println(F("kowThunderstorm"));
    return kowThunderstorm;
  }
  
  if (icon == "13d" || icon == "13n") {
    Serial.println(F("kowSnow"));
    return kowSnow;
  }

  if (icon == "50d" || icon == "50n") {
    Serial.println(F("kowFog"));
    return kowFog;
  }

  Serial.println(F("kowUnknown"));
  return kowUnknown;
}

bool KellyOpenWeather::parseURL(String &url, String &host, int &port, String &path) {

  int protocolEnd = url.indexOf("://");
  if (protocolEnd == -1) {
    return false;
  }

  String proto = url.substring(0, protocolEnd);

  int hostStart = protocolEnd + 3;
  int pathStart = url.indexOf("/", hostStart);
  if (pathStart == -1) {
    pathStart = url.length();
  }
  host = url.substring(hostStart, pathStart);
  path = url.substring(pathStart);

  port = (proto == "https") ? 443 : 80;

  return true;
}