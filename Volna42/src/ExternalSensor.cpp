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

    // OpenWeather

    if (url.indexOf("openweather") != -1) {

      if (attempt > 1) {        
          delay(400);
      }

      KellyOpenWeather openWeatherLoader = KellyOpenWeather(EXTERNAL_SENSOR_CONNECT_TIMEOUT);  
      
      Serial.println(F("OpenWeather parser"));
      int resultCode = openWeatherLoader.loadCurrent(url);      
      if (resultCode == 200) {

        Serial.println(F("OpenWeather parser - success"));
        // Serial.println(openWeatherLoader.temp);
        // Serial.println(openWeatherLoader.hum);
        // Serial.println(openWeatherLoader.pressure);

        resultData.isDataValid = true;
        resultData.temperature = openWeatherLoader.temp;
        resultData.pressure = openWeatherLoader.pressure;
        resultData.humidity = openWeatherLoader.hum;
        resultData.t = time(nullptr);
        resultData.icon = openWeatherLoader.weatherType;
        
        openWeatherLoader.end();
        return true;
        
      } else {
        
        Serial.print(F("OpenWeather parser - error. Code : ")); Serial.println(resultCode);
        error = openWeatherLoader.error;        
        Serial.println(openWeatherLoader.error);
        openWeatherLoader.end();

        if (resultCode == -1 || (resultCode >= 1 && resultCode <= 20)) {

          Serial.println(F("Fail to connect OpenWeather server - no response or unavailable..."));
          Serial.println(F("Retry to connect OpenWeather..."));
          return requestData(url, login, pass, resultData, error, attempt);
        }

      }

      return false;
    
    // Domoticz & HomeAssistant

    } else {

      if (attempt > 1) {
          delay(200);
      }

      WiFiClient client;
      HTTPClient http;
      
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
        Serial.println(F("Retry to connect ext sensor..."));
        return requestData(url, login, pass, resultData, error, attempt);
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
            if (!KellyOWParserTools::collectJSONFieldData(homeAssistant ? "temperature" : "Temp", payload, collectedData, maxLength)) {

                Serial.print(F("Bad data : ")); Serial.println(payload.substring(0, 255));
                error = "External sensor error : " + KellyOWParserTools::sanitizeResponse(payload);

            } else {
              
              newData.temperature = KellyOWParserTools::validateFloatVal(collectedData);
              if (newData.temperature <= -1000) {   

                Serial.print(F("Bad data : no temperature")); Serial.println(payload.substring(0, 255));
                error = "External sensor error : no temperature data";

              } else {

                newData.isDataValid = true;
                
                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "humidity" : "Humidity", payload, collectedData, maxLength);
                newData.humidity = KellyOWParserTools::validateFloatVal(collectedData);

                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "pressure" : "Barometer", payload, collectedData, maxLength);                
                newData.pressure = KellyOWParserTools::validateFloatVal(collectedData);

                if (newData.pressure > -1000) {
                    newData.pressure = newData.pressure * 100.0f;
                }

                KellyOWParserTools::collectJSONFieldData(homeAssistant ? "battery" : "BatteryLevel", payload, collectedData, maxLength);
                newData.bat = KellyOWParserTools::validateFloatVal(collectedData);

                     if (newData.bat > 100) newData.bat = 100;
                else if (newData.bat < 0) newData.bat = -1000;
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
