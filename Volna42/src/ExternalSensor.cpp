#include "ExternalSensor.h"

/*
  Request data from remote sensor
*/
bool ExternalSensor::requestData(String &url, String &login, String &pass, externalSensorData &resultData, String &error) {

    if (url.length() <= 0 || url.equals(F("off"))) {
      Serial.print(F("[Weather API] external sensor not configured or disabled. INPUT : ")); Serial.println(url);
      return false;
    }

    int resultCode = -1;
    KellyWeatherApi * weatherApi = NULL;

    if (url.indexOf("openweather") != -1) {
       weatherApi = new KellyOpenWeather(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
    } else if (url.indexOf("open-meteo") != -1) {
       weatherApi = new KellyOpenMeteo(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
    } else if (url.indexOf("/api/states") != -1) {
       weatherApi = new KellyOpenHA(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
    } else {
      // default controller - domoticz
       weatherApi = new KellyOpenDomoticz(EXTERNAL_SENSOR_CONNECT_TIMEOUT);
    }

    for (int8_t attempt = 1; attempt <= EXTERNAL_SENSOR_CONNECT_ATTEMPTS; attempt++) {

        if (url.indexOf("openweather") != -1) {
          resultCode = weatherApi->loadCurrent(url);    
        } else if (url.indexOf("open-meteo") != -1) {
          resultCode = weatherApi->loadCurrent(url);    
        } else if (url.indexOf("/api/states") != -1) {
          resultCode = weatherApi->loadCurrent(url, pass);    
        } else {
          resultCode = weatherApi->loadCurrent(url, login, pass);    
        }

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
          
          if (resultCode == -1) {

            Serial.println(F("[Weather API] Fail to connect web-server - no response or unavailable..."));
            Serial.println(F("[Weather API] Retry to connect..."));
            
            error = "";

          } else {

            Serial.println(F("[Weather API] Wrong configuration or server response - skip attempts"));
            return false;
          }
        }
    }

    Serial.println(F("[Weather API] Max attempts reached"));

    weatherApi->end();
    delete weatherApi;
    return false;
    
}
