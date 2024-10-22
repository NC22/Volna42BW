#include "KellyOpenWeather.h"

KellyOpenWeather::KellyOpenWeather(int timeout): KellyWeatherApi(timeout) {

}

/*
  Get data from OpenWeather API

  result code : 
  200 - success
  -1 - fail to connect to url
  -2 - fail to parse url
  1-4 - response parsing errors
  5 - response "cod" value != 200
  6 - fail to read response headers
  7 - empty body
*/
int KellyOpenWeather::loadCurrent(String & nurl) {
  
  Serial.println(F("[Weather API] - OpenWeather parser"));

  weatherLoaded = false;
  error = "";  
  int port;
  String host, path, tmp;

  if (!KellyOWParserTools::parseURL(nurl, host, port, path)) {
    Serial.println(F("[OpenWeather] Failed to parse URL"));
    return -2;
  }

  clientStart(port == 443);
 
  if (!client->connect(host.c_str(), port)) {
    Serial.println(F("[OpenWeather] Connection failed!"));
    clientEnd();
    return -1;
  }

  Serial.println(F("[OpenWeather] Connected to server...read response..."));

  uint16_t code, contentLength; 
  KellyOWParserTools::clientSendRequestHeaders(host, path, client);
  KellyOWParserTools::clientReadHeaders(code, contentLength, client, connectionTimeout);
  
  if (code > 0) {

    Serial.print(F("[OpenWeather] Headers received | HTTP RESPONSE Code : ")); Serial.print(code);
    Serial.print(F(" | Content-length : ")); Serial.println(contentLength);

  } else {

    error = "[OpenWeather] Headers empty | No HTTP Code detected";
    clientEnd();
    return 6;
  }

  if (contentLength > 2000 || contentLength == 0) {
    contentLength = 2000;
    Serial.print(F("[OpenWeather] Unknown content length -> read max amount or exit by Timeout"));
  }

  KellyOWParserTools::clientReadBody(tmp, contentLength, client, connectionTimeout);
  clientEnd();

  // Serial.println(F("RESPONSE : :: "));
  // Serial.println(tmp);

  if (tmp.length() <= 0) {
    error = "[OpenWeather] Empty body";
    return 7;
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
              if (pressure <= BAD_SENSOR_DATA) {

              } else {
                pressure = pressure * 100.0f;
              }

              if (temp <= BAD_SENSOR_DATA) {                    
                  error = "[OpenWeather] Parse Temp fail";
                  return 4;
              } else {
                  weatherLoaded = true;
              }

          } else {
            error = "[OpenWeather] Temp read fail";
            return 3;
          }
      } else {
          error = "[OpenWeather] Main block not found";
          return 2;
      }

  } else {
    error = "[OpenWeather] Response not contain code number | HTTP CODE : " + String(code);
    return 1;
  }

  return 200;
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