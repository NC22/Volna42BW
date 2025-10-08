#include "ExternalSensor.h"

/*
  Request data from remote sensor
*/
bool ExternalSensor::requestData(String &url, String &login, String &pass, externalSensorData &resultData, String &error, int8_t attempt) {

    attempt++;
    
    if (attempt > EXTERNAL_SENSOR_CONNECT_ATTEMPTS){      

      Serial.println(F("Max attempts reached"));
      return false;
    } 

    if (url.length() <= 0 || url.equals(F("off"))) {
      Serial.print(F("updateExtSensorData - external sensor not configured or disabled. INPUT : ")); Serial.println(url);
      return false;
    }

    if (attempt > 1) {        
        delay(400);
    }

    // Open-Meteo, OpenWeather

    int resultCode = -1;
    KellyWeatherApi * weatherApi = NULL;

    if (url.indexOf("openweather") != -1) {
       weatherApi = new KellyOpenWeather(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
       resultCode = weatherApi->loadCurrent(url);    
    } else if (url.indexOf("open-meteo") != -1) {
       weatherApi = new KellyOpenMeteo(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
       resultCode = weatherApi->loadCurrent(url);    
    } else if (url.indexOf("/api/states") != -1) {
       weatherApi = new KellyOpenHA(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
       resultCode = weatherApi->loadCurrent(url, pass);    
    }

    if (weatherApi) {

      if (resultCode == 200) {

        Serial.println(F("[Weather API] Success"));
        
        Serial.println(F("--------"));
        Serial.println(weatherApi->temp);
        Serial.println(weatherApi->hum);
        Serial.println(weatherApi->pressure);
        Serial.println(weatherApi->bat);
        Serial.println(F("--------"));

        resultData.isDataValid = true;
        resultData.temperature = weatherApi->temp;
        resultData.pressure = weatherApi->pressure;
        resultData.humidity = weatherApi->hum;
        resultData.bat = weatherApi->bat;
        resultData.t = time(nullptr);
        resultData.icon = weatherApi->weatherType;
        
        weatherApi->end();
        delete weatherApi;
        return true;
        
      } else {
        
        Serial.print(F("[Weather API] Parser error | Result Code : ")); Serial.println(resultCode);
        error = weatherApi->error;        
        Serial.println(weatherApi->error);
        
        weatherApi->end();
        delete weatherApi;

        if (resultCode == -1) {

          Serial.println(F("[Weather API] Fail to connect web-server - no response or unavailable..."));
          Serial.println(F("[Weather API] Retry to connect..."));
          
          error = "";
          return requestData(url, login, pass, resultData, error, attempt);
        }

      }

      return false;
    
    // Default controller - used for Domoticz & HomeAssistant, todo - move to common WeatherApi

    } else {

      WiFiClient client;
      HTTPClient http;
      // http.setReuse(false);
      
      externalSensorData newData;
      newData.isDataValid = false;
      newData.temperature = BAD_SENSOR_DATA;
      newData.humidity = BAD_SENSOR_DATA;
      newData.bat = BAD_SENSOR_DATA;
      newData.pressure = BAD_SENSOR_DATA;
      newData.icon = kowUnknown;
      newData.t = time(nullptr);

      #if defined(ESP32)
          http.setTimeout(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
          client.setTimeout(EXTERNAL_SENSOR_CONNECT_TIMEOUT / 1000);
      #else 
          http.setTimeout(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
          client.setTimeout(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
      #endif

      http.begin(client, url);

      if (login.length() > 0) {

        http.setAuthorization(login.c_str(), pass.c_str());
        Serial.println(F("[AUTH] Basic auth mode"));

      } else if (pass.length() > 0) {
          
        http.setAuthorization("");
        String token = "Bearer " + pass;
        http.addHeader("Authorization", token.c_str());

        Serial.println(F("[AUTH] Bearer auth mode"));
      } else {
        
        Serial.println(F("[AUTH] Anonimous mode"));
      }

      Serial.println(F("updateExtSensorData"));

      int httpResponseCode = http.GET();
      if (httpResponseCode < 0) {

        Serial.print(F("Fail to connect ext sensor... Code "));
        Serial.println(httpResponseCode);

        if (httpResponseCode == -11) { // connected, but read timeout

            #if defined(ESP8266)
            // can be memory leak inside HTTPClient itself on ESP8266, test with WiFiClient directly
            client.abort(); // Abort method is required to prevent memory leak on stuck connections
            #endif

            http.end();

            Serial.println(F("Client abort connection"));
            return false;

        } else {

          http.end();

          Serial.println(F("Retry to connect ext sensor..."));
          return requestData(url, login, pass, resultData, error, attempt);
        }

      }

      if (httpResponseCode > 0) {
        
          String payload = http.getString();

          String collectedData;
          String resultData = "";
          bool homeAssistant = false;

          if (url.indexOf("/api/states") != -1) {
            homeAssistant = true;
          }

          Serial.println(homeAssistant ? F("[HomeAssistant] parser") : F("[Domoticz] parser"));

            /*
              Domoticz

              old url format - /json.htm?type=devices&rid=___ID___ 
              new url format - /json.htm?type=command&param=getdevices&rid=___ID___

              "BatteryLevel" : 89,
              "Data" : "34.8 C, 36 %",
              "Name" : "\u0414\u0430\u0442\u0447\u0438\u043a ... \u043a (Temperature + Humidity)",
              "Humidity" : 36.0,

              HA

              url format - /api/states/___ID___

              "battery": 87,
              "battery_low": false,
              "humidity": 40.85,
              "temperature": 26.32,
              "friendly_name": "Датчик почвы (каланхое спальня) Влага"
            */
            
            int maxLength = 12;
            
            // todo - переписать контроллер HA - должен быть опционально список url - запросы последовательны - данные читать из state т.к. не всегда возвращаются все данные сразу
            // запросы выполняются по /api/states/entity_id

            bool tempFound = KellyOWParserTools::collectJSONFieldData(homeAssistant ? "temperature" : "Temp", payload, collectedData, maxLength);
            if (!tempFound && homeAssistant) {
                tempFound = KellyOWParserTools::collectJSONFieldData("state", payload, collectedData, maxLength);
               
            }
             Serial.println(collectedData);
            if (!tempFound) {

                Serial.print(F("Bad data : ")); Serial.println(payload.substring(0, 255));
                error = "External sensor error : " + KellyOWParserTools::sanitizeResponse(payload);

            } else {
              
              newData.temperature = KellyOWParserTools::validateFloatVal(collectedData);
              if (newData.temperature <= BAD_SENSOR_DATA) {   

                Serial.print(F("Bad data : no temperature | ")); Serial.println(payload.substring(0, 255));
                error = "External sensor error : no temperature data";

              } else {

                newData.isDataValid = true;
                
                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "humidity" : "Humidity", payload, collectedData, maxLength);
                newData.humidity = KellyOWParserTools::validateFloatVal(collectedData);

                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "pressure" : "Barometer", payload, collectedData, maxLength);                
                newData.pressure = KellyOWParserTools::validateFloatVal(collectedData);

                if (newData.pressure > BAD_SENSOR_DATA) {
                    newData.pressure = newData.pressure * 100.0f;
                }

                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "battery" : "BatteryLevel", payload, collectedData, maxLength);
                newData.bat = KellyOWParserTools::validateFloatVal(collectedData);

                     if (newData.bat > 100) newData.bat = 100;
                else if (newData.bat < 0) newData.bat = BAD_SENSOR_DATA;

                if (homeAssistant) {
                    
                  KellyOWParserTools::collectJSONFieldData("state", payload, collectedData, 16);
                  /*
                    "clear-night": "Clear, night",
                    "cloudy": "Cloudy",
                    "exceptional": "Exceptional",
                    "fog": "Fog",
                    "hail": "Hail",
                    "lightning": "Lightning",
                    "lightning-rainy": "Lightning, rainy",
                    "partlycloudy": "Partly cloudy",
                    "pouring": "Pouring",
                    "rainy": "Rainy",
                    "snowy": "Snowy",
                    "snowy-rainy": "Snowy, rainy",
                    "sunny": "Sunny",
                    "windy": "Windy",
                    "windy-variant": "Windy"
                  */

                  if (collectedData == "clear-night") { 
                      Serial.println(F("kowClearSky"));
                      newData.icon = kowClearSky;
                  } else if (collectedData == "cloudy") { 
                      Serial.println(F("kowBrokenClouds"));
                      newData.icon = kowBrokenClouds;
                  } else if (collectedData == "exceptional") { 
                      Serial.println(F("kowUnknown"));
                      newData.icon = kowUnknown;
                  } else if (collectedData == "fog") { 
                      Serial.println(F("kowFog"));
                      newData.icon = kowFog;
                  } else if (collectedData == "hail") { 
                      Serial.println(F("kowRain - Hail")); 
                      newData.icon = kowRain;
                  } else if (collectedData == "lightning") { 
                      Serial.println(F("Lightning"));
                      newData.icon = kowRain;
                  } else if (collectedData == "lightning-rainy") { 
                      Serial.println(F("kowRain - Lightning"));
                      newData.icon = kowRain;
                  } else if (collectedData == "partlycloudy") {
                      Serial.println(F("kowFewClouds"));
                      newData.icon = kowFewClouds;
                  } else if (collectedData == "pouring") { // Heavy rain
                      Serial.println(F("kowShowerRain"));
                      newData.icon = kowShowerRain;
                  } else if (collectedData == "rainy") {
                      Serial.println(F("kowRain"));
                      newData.icon = kowRain;
                  } else if (collectedData == "snowy") {
                      Serial.println(F("kowSnow"));
                      newData.icon = kowSnow;
                  } else if (collectedData == "snowy-rainy") { // Mixed snow and rain
                      Serial.println(F("kowSnow"));
                      newData.icon = kowSnow; 
                  } else if (collectedData == "sunny") { // Clear, sunny day
                      Serial.println(F("kowClearSky"));
                      newData.icon = kowClearSky;
                  } else if (collectedData == "windy") { // Windy
                      Serial.println(F("kowUnknown")); 
                      newData.icon = kowUnknown;
                  } else if (collectedData == "windy-variant") { // Windy variant
                      Serial.println(F("kowUnknown")); 
                      newData.icon = kowUnknown;
                  } else {
                      Serial.println(F("kowUnknown")); 
                      newData.icon = kowUnknown;
                  }
                }
              }
              
              Serial.println(F("Collect data - OK, Result :")); 
            }

      } else {

        if (httpResponseCode == -1) {
            error = "External sensor error : Server not accessable";
        } else {
            error = "External sensor error : Unknown error code: " + String(httpResponseCode);
        }

        Serial.print(F("Error code: ")); Serial.println(httpResponseCode);
        // logReport("cant get ext sensor data " + String(httpResponseCode))
      }

      
      http.end();
      
      if (newData.isDataValid) {
        
        Serial.print(F("temperature: ")); Serial.println(newData.temperature);
        Serial.print(F("humidity: ")); Serial.println(newData.humidity);
        Serial.print(F("pressure: ")); Serial.println(newData.pressure);
        Serial.print(F("bat: ")); Serial.println(newData.bat);

        resultData = newData;
        // lastState.connectTimes++;
        // lastState.syncT = time(nullptr);
      }     
      
      return newData.isDataValid;
    }
}
