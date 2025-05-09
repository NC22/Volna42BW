#include "KellyOpenHA.h"


KellyOpenHA::KellyOpenHA(int timeout): KellyWeatherApi(timeout) {

}

KellyHAPartialType KellyOpenHA::fillPartialData(String & payload, String & collectedData) {
    
    if (payload.indexOf("temperature") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        temp = KellyOWParserTools::validateFloatVal(collectedData);
        if (KellyOWParserTools::collectJSONFieldData("unit_of_measurement", payload, collectedData)) {
            temp = validateByUnitTemperature(temp, collectedData);
        }
        return kowHATemperature;
    } else if (payload.indexOf("humidity") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        hum = KellyOWParserTools::validateFloatVal(collectedData);
        return kowHAHumidity;
    } else if (payload.indexOf("pressure") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        pressure = KellyOWParserTools::validateFloatVal(collectedData);
        if (KellyOWParserTools::collectJSONFieldData("unit_of_measurement", payload, collectedData)) {
            pressure = validateByUnitPressure(pressure, collectedData);
        }
        return kowHAPressure;
    } else if (payload.indexOf("battery") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        bat = KellyOWParserTools::validateFloatVal(collectedData);
        return kowHABattery;
    }
    
    return kowHAUnknown;
}

// back to Celsius if returned format mismatched
float KellyOpenHA::validateByUnitTemperature(float temperature, String & unitsInfo) {
    
    // first symbol is UTF8 - (0) degree and its two bytes - 0xC2 0xB0
    
    if (unitsInfo[2] == 'F') {
        return (temperature - 32.0f) * 0.55556f;
    } else {
        return temperature;
    }
}

// back to hPa if returned format mismatched
float KellyOpenHA::validateByUnitPressure(float pressure, String & unitsInfo) {
    if (unitsInfo == "mmHg") {
        return pressure * 1.3332239f;
    } else if (unitsInfo == "inHg") {
        return pressure * 33.8639f; 
    } else {
        return pressure; // hPa or mbar
    }
}

KellyHAPartialType KellyOpenHA::requestProcess(String & url, String & token, bool partialOnly) {
    
    WiFiClient client;
    HTTPClient http;

    http.begin(client, url);

    http.setAuthorization("");
    String auth = "Bearer " + token;
    http.addHeader("Authorization", auth.c_str());

    int httpResponseCode = http.GET();    
    
    if (httpResponseCode <= 0) {
        
        Serial.print(F("[KellyOpenHA] Failed to connect"));
        Serial.println(url);
        
        error = "Connection error with Home Assistant (No response)";        
        http.end();
        return kowHAConnectError;
        
    } else if (httpResponseCode != 200) {
        
        Serial.print(F("[KellyOpenHA] Bad response | Code : ")); Serial.println(httpResponseCode);
        Serial.println(url);
        
        error = "Home Assistant response error | Code : " + String(httpResponseCode);        
        http.end();
        return kowHAConnectError;
    }
    
    String payload = http.getString();
    String collectedData;

    http.end();        

    KellyHAPartialType ptype = kowHAUnknown;
    
    // Case when server answer contains all sensor data at once
    // for ex. weather.forecast_home_assistant - contains also weather state with icon 
    
    if (!partialOnly && KellyOWParserTools::collectJSONFieldData("temperature", payload, collectedData)) {
        
        temp = KellyOWParserTools::validateFloatVal(collectedData);
        if (KellyOWParserTools::collectJSONFieldData("temperature_unit", payload, collectedData)) {
            temp = validateByUnitTemperature(temp, collectedData);
        }
        
        if (KellyOWParserTools::collectJSONFieldData("humidity", payload, collectedData)) {
            hum = KellyOWParserTools::validateFloatVal(collectedData);
        }

        if (KellyOWParserTools::collectJSONFieldData("pressure", payload, collectedData)) {

            pressure = KellyOWParserTools::validateFloatVal(collectedData);
            if (KellyOWParserTools::collectJSONFieldData("pressure_unit", payload, collectedData)) {
                pressure = validateByUnitPressure(pressure, collectedData);
            }
        }

        if (KellyOWParserTools::collectJSONFieldData("battery", payload, collectedData)) {
            bat = KellyOWParserTools::validateFloatVal(collectedData);            
        }

        if (KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
            weatherType = getMeteoIconState(collectedData);  
        }

        ptype = kowHAALL;
        
    } else {    
        // find partial data in answer        
        ptype = fillPartialData(payload, collectedData);        
    }

    if (ptype == kowHAALL || ptype == kowHABattery) {

        if (bat > 100) bat = 100;
        else if (bat < 0) bat = BAD_SENSOR_DATA;
    } 

    if (ptype == kowHAALL || ptype == kowHAPressure) {

        if (pressure > BAD_SENSOR_DATA) {
            pressure = pressure * 100.0f;
        }
    }

    return ptype;    
}

int KellyOpenHA::loadCurrent(String & nurl) {
    String token = "";
    return loadCurrent(nurl, token);
}

/*
  Get data from Home Assistant API    

  result code : 
  200 - success
  -1 - fail to connect to url (description in error state & in log)
  1 - no data
  2 - bad or not enough data
*/
int KellyOpenHA::loadCurrent(String & nurl, String & token) {
    
    Serial.println(F("[Weather API] - Home Assistant parser"));
    weatherLoaded = false;
    error = "";
    
    int separatorPos = nurl.indexOf(",");
    String baseUrl = separatorPos != -1 ? nurl.substring(0, separatorPos) : nurl; // take only first ID for baseUrl if more then one /api/state/[id1]
    
    KellyHAPartialType addedData = requestProcess(baseUrl, token, separatorPos == -1 ? false : true); // if only one ID specifed - take all data at once

    if (addedData == kowHAConnectError) { // no connection
                
        // error string setted in requestProcess() method

        if (error.length() <= 0) {
            error = "[KellyOpenHA] Connection fail";
        }

        return -1;

    } else if (addedData == kowHAUnknown) { // unexpected data or format   

        if (error.length() <= 0) {
            error = "[KellyOpenHA] Wrong data format";
        }

        return 1; // no data

    } else if (separatorPos == -1) { 

        // collected all we can from one request. If some thing not found we need to specify list of IDs 

    } else {

        // continue to grab addition sensors by IDs
            
        baseUrl = baseUrl.substring(0, nurl.lastIndexOf('/') + 1);
        String currentId = "";
        for (unsigned int i = separatorPos+1; i <= nurl.length(); i++) { // separatorPos - skip to second element /api/state/[id1],[id2],...[idn]
            char c = i < nurl.length() ? nurl[i] : ',';
            
            if (c == ',') {
    
                Serial.print(F("[KellyOpenHA] Request sensor data for addition ID - "));
                Serial.println(currentId);
    
                currentId = baseUrl + currentId;
                requestProcess(currentId, token, true);            
                currentId = "";

            } else {
                currentId += c;
            }
        }
    }    

    if (temp <= BAD_SENSOR_DATA) {   

        error = "[KellyOpenHA] no temperature data";
        return 2;

    } else {

        weatherLoaded = true;
        return 200;
    }
}

KellyOWIconType KellyOpenHA::getMeteoIconState(const String& collectedData) {

    if (collectedData == "clear-night") {
        Serial.println(F("kowClearSky"));
        return kowClearSky;
    } else if (collectedData == "cloudy") {
        Serial.println(F("kowBrokenClouds"));
        return kowBrokenClouds;
    } else if (collectedData == "exceptional") {
        Serial.println(F("kowUnknown"));
        return kowUnknown;
    } else if (collectedData == "fog") {
        Serial.println(F("kowFog"));
        return kowFog;
    } else if (collectedData == "hail") {
        Serial.println(F("kowRain - Hail"));
        return kowRain; 
    } else if (collectedData == "lightning") {
        Serial.println(F("Lightning"));
        return kowThunderstorm; 
    } else if (collectedData == "lightning-rainy") {
        Serial.println(F("kowRain - Lightning"));
        return kowThunderstorm; 
    } else if (collectedData == "partlycloudy") {
        Serial.println(F("kowFewClouds"));
        return kowFewClouds;
    } else if (collectedData == "pouring") {
        Serial.println(F("kowShowerRain"));
        return kowShowerRain;
    } else if (collectedData == "rainy") {
        Serial.println(F("kowRain"));
        return kowRain;
    } else if (collectedData == "snowy") {
        Serial.println(F("kowSnow"));
        return kowSnow;
    } else if (collectedData == "snowy-rainy") {
        Serial.println(F("kowSnow - Rain"));
        return kowSnow;
    } else if (collectedData == "sunny") {
        Serial.println(F("kowClearSky"));
        return kowClearSky;
    } else if (collectedData == "windy") {
        Serial.println(F("kowWindy"));
        return kowUnknown;
    } else if (collectedData == "windy-variant") {
        Serial.println(F("kowWindyVariant"));
        return kowUnknown;
    } else {
        Serial.println(F("kowUnknown"));
        return kowUnknown;
    }
}