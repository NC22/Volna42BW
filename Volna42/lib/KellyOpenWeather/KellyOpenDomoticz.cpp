#include "KellyOpenDomoticz.h"


KellyOpenDomoticz::KellyOpenDomoticz(int timeout): KellyWeatherApi(timeout) {

}

void KellyOpenDomoticz::closeConnection() {
    http.end();
    client.stop();
}

int KellyOpenDomoticz::loadCurrent(String & nurl) {
    String login = ""; String pass = "";
    return loadCurrent(nurl, login, pass);
}

/*
  Get data from Domoticz API    

  result code : 
  200 - success
  -1 - fail to connect to url
  1 - bad or empty response
  2 - bad data
*/
int KellyOpenDomoticz::loadCurrent(String & nurl, String & login, String & pass) {
    
    Serial.println(F("[Weather API] - Domoticz parser"));
    weatherLoaded = false;
    error = "";
    
    http.begin(client, nurl);
    
    if (login.length() > 0) {

        http.setAuthorization(login.c_str(), pass.c_str());
        Serial.println(F("[KellyDomoticz] [AUTH] Basic auth mode"));

    } else {

        Serial.println(F("[KellyDomoticz] [AUTH] Anonimous mode"));
    }


    http.setReuse(false);  
    http.addHeader("Connection", "close");
    int httpResponseCode = http.GET();   
    
    if (httpResponseCode < 0) {

        Serial.print(F("[KellyDomoticz] Fail to connect ext sensor... Code "));
        Serial.println(httpResponseCode);

        if (httpResponseCode == -11) { // connected, but read timeout
        
            closeConnection();
            error = "[KellyDomoticz] Fail - Connected but read timeout";
            return -1;

        } else {

            closeConnection();
            error = "[KellyDomoticz] Fail - no connection";
            return -1;
        }
    }
    
    String payload = http.getString();
    String collectedData;
    closeConnection();
    
    /*
        Domoticz

        old url format - /json.htm?type=devices&rid=___ID___ 
        new url format - /json.htm?type=command&param=getdevices&rid=___ID___

        "BatteryLevel" : 89,
        "Data" : "34.8 C, 36 %",
        "Name" : "\u0414\u0430\u0442\u0447\u0438\u043a ... \u043a (Temperature + Humidity)",
        "Humidity" : 36.0,
    */
    
    int maxLength = 12;
    bool tempFound = KellyOWParserTools::collectJSONFieldData("Temp", payload, collectedData, maxLength);
    
    if (!tempFound) {

        Serial.print(F("Bad data : ")); Serial.println(payload.substring(0, 255));
        error = "[KellyDomoticz] error : " + KellyOWParserTools::sanitizeResponse(payload);
        return 1;
            
    } else {
      
        temp = KellyOWParserTools::validateFloatVal(collectedData);
        if (temp <= BAD_SENSOR_DATA) {   

            Serial.print(F("Bad data : no temperature | ")); Serial.println(payload.substring(0, 255));
            error = "[KellyDomoticz] Bad data";
            return 2;

        } else {

            KellyOWParserTools::collectJSONFieldData("Humidity", payload, collectedData, maxLength);
            hum = KellyOWParserTools::validateFloatVal(collectedData);

            KellyOWParserTools::collectJSONFieldData("Barometer", payload, collectedData, maxLength);                
            pressure = KellyOWParserTools::validateFloatVal(collectedData);

            if (pressure > BAD_SENSOR_DATA) {
            pressure = pressure * 100.0f;
            }

            KellyOWParserTools::collectJSONFieldData("BatteryLevel", payload, collectedData, maxLength);
            bat = KellyOWParserTools::validateFloatVal(collectedData);

            if (bat > 100) bat = 100;
            else if (bat < 0) bat = BAD_SENSOR_DATA;

            weatherLoaded = true;
            return 200;
        }
    }
}

KellyOWIconType KellyOpenDomoticz::getMeteoIconState(const String& collectedData) {

    return kowUnknown;

}