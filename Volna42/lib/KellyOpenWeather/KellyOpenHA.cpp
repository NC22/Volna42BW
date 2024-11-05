#include "KellyOpenHA.h"


KellyOpenHA::KellyOpenHA(int timeout): KellyWeatherApi(timeout) {

}

KellyHAPartialType KellyOpenHA::fillPartialData(String & payload, String & collectedData) {
    
    if (payload.indexOf("temperature") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        temp = KellyOWParserTools::validateFloatVal(collectedData);
        return kowHATemperature;
    } else if (payload.indexOf("humidity") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        hum = KellyOWParserTools::validateFloatVal(collectedData);
        return kowHAHumidity;
    } else if (payload.indexOf("pressure") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        pressure = KellyOWParserTools::validateFloatVal(collectedData) * 100.0f;
        return kowHAPressure;
    } else if (payload.indexOf("battery") != -1 && KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
        bat = KellyOWParserTools::validateFloatVal(collectedData);
        return kowHABattery;
    }
    
    return kowHAUnknown;
}

KellyHAPartialType KellyOpenHA::requestProcess(String & url, bool partialOnly) {
    
    WiFiClient client;
    HTTPClient http;
    http.begin(client, url);
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
    
    // Check if answer already contain all needed data
    
    if (!partialOnly && KellyOWParserTools::collectJSONFieldData("temperature", payload, collectedData)) {
        
        temp = KellyOWParserTools::validateFloatVal(collectedData);

        if (KellyOWParserTools::collectJSONFieldData("humidity", payload, collectedData)) {
            hum = KellyOWParserTools::validateFloatVal(collectedData);
        }
        if (KellyOWParserTools::collectJSONFieldData("pressure", payload, collectedData)) {
            pressure = KellyOWParserTools::validateFloatVal(collectedData) * 100.0f;
        }
        if (KellyOWParserTools::collectJSONFieldData("battery", payload, collectedData)) {
            bat = KellyOWParserTools::validateFloatVal(collectedData);
        }
        if (KellyOWParserTools::collectJSONFieldData("state", payload, collectedData)) {
            weatherType = getMeteoIconState(collectedData);  
        }

        return kowHAALL;
        
    } else {
    
        // Only partial data in answer        
        return fillPartialData(payload, collectedData);        
    }
    
}

/*
  Get data from Home Assistant API    

  result code : 
  200 - success
  -1 - fail to connect to url (description in error state & in log)
  1, 2 - no data, not enough data
*/
int KellyOpenHA::loadCurrent(String & nurl) {
    
    Serial.println(F("[Weather API] - Home Assistant parser"));
    weatherLoaded = false;
    error = "";
    
    int separatorPos = nurl.indexOf(",");
    String baseUrl = separatorPos != -1 ? nurl.substring(0, separatorPos) : nurl;
    
    KellyHAPartialType addedData = requestProcess(baseUrl, false);
    if (addedData == kowHAConnectError) {
        return -1;
    } else if (addedData == kowHAUnknown) {               
        return 1; // no data
    } else if (addedData != kowHATemperature && separatorPos == -1) {
        return 2; // not enough data
    }

    weatherLoaded = true;
    
    // Partial data addition devices requests    
    // no addition device ids -> exit, At least temperature is collected
    
    if (separatorPos == -1) {
        return 200;
    }
    
    baseUrl = baseUrl.substring(0, nurl.lastIndexOf('/') + 1);
    String currentId = "";
    for (unsigned int i = separatorPos; i <= nurl.length(); i++) { // separatorPos - skip to second element /api/state/[id1],[id2],...[idn]
        char c = i < nurl.length() ? nurl[i] : ',';
        
        if (c == ',') {
            currentId = baseUrl + currentId;
            requestProcess(currentId, true);            
            currentId = ""; 
        } else {
            currentId += c;
        }
    }
    
    return 200;
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