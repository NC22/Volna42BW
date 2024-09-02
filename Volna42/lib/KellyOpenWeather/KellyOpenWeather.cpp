#include "KellyOpenWeather.h"

KellyOpenWeather::KellyOpenWeather(String nurl) {

    url = nurl;
    // parser = new JsonStreamingParser();
    // KellyOWParserListener parseListener = KellyOWParserListener();

    //listener = &parseListener;
   //   listener->controller = this;
   // parser->setListener(&parseListener);
}

void KellyOpenWeather::end() {
  // free resources
}

bool KellyOpenWeather::loadCurrent() {
  
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

  if (parseURL(host, port, path)) {

    if (port == 443) https = true;
    
    WiFiClientSecure * clientSecure = NULL;
    WiFiClient * client = NULL;
    
    if (https) {
      clientSecure = new WiFiClientSecure;
    } else {
      client = new WiFiClient;
    }

    bool connected = false;
    if (https) {
      clientSecure->setInsecure();  
      connected = clientSecure->connect(host.c_str(), port);
    } else {
      connected = client->connect(host.c_str(), port);
    }

    if (connected) {
      Serial.println(F("Connected to server"));

      String tmp = String("GET ") + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
      host = "";
      path = "";

      if (https) {
        clientSecure->print(tmp);
      } else {
        client->print(tmp);
      }

      while ((https ? clientSecure : client)->connected()) {
        tmp = (https ? clientSecure : client)->readStringUntil('\n');
        if (tmp == "\r") {
          // Serial.println("Headers received");
          break;
        }
      }

      tmp = "";
      while ((https ? clientSecure : client)->available()) {
        tmp += (https ? clientSecure : client)->readStringUntil('\n');
      } 

      // Serial.println("RESPONSE : :: ");
      // Serial.println(tmp);

      String collectedData;
      if (KellyOWParserTools::collectJSONFieldData("cod", tmp, collectedData)) {

          if (collectedData != "200") {
              KellyOWParserTools::collectJSONFieldData("message", tmp, collectedData);
              error = collectedData;
              return false;
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

                  if (temp <= -1000) {                    
                      error = "Parse Temp fail";
                      return false;
                  } else {
                      weatherLoaded = true;
                  }

              } else {
                error = "Temp read fail";
                return false;
              }
          } else {
              error = "Main block not found";
              return false;
          }

      } else {
        error = "Response not contain result code number";
        return false;
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

      return true;
      
    } else {
      Serial.println(F("Connection failed!"));
      return false;
    }
    
  } else {
    Serial.println(F("Failed to parse URL"));
    return false;
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

bool KellyOpenWeather::parseURL(String &host, int &port, String &path) {

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