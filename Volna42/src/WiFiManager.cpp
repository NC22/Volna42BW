#include "WifiManager.h"


WiFiManager::WiFiManager() {
    
}

bool WiFiManager::isEnabled() {
    return enabled;
}


IPAddress WiFiManager::getIP() {
    return isAPmode() ? WiFi.softAPIP() : WiFi.localIP();
}

wl_status_t WiFiManager::connect(String sid, String password) {

    if (sid.length() <= 0) return WL_NO_SSID_AVAIL;
    
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);   

    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    WiFi.begin(sid.c_str(), password.c_str());
    unsigned int waitTime = 0;

    unsigned long askTimerStart = millis();
    while (true) {

      if (millis() - askTimerStart >= 1000) {
        
        waitTime++;
        askTimerStart = millis();
        lastConnectStatus = WiFi.status();

        #if defined(ESP32)

            if ( lastConnectStatus == WL_CONNECTED) {
                enabled = true;
                break;
            } else if (lastConnectStatus == WL_CONNECT_FAILED) {
                Serial.print(F("FAIL TO CONNECT TO LAST CONFIGURED ACCESS POINT"));
                break;
            } else if (lastConnectStatus == WL_NO_SSID_AVAIL) {
                Serial.println(F("No any networks with specified network name | SSID"));
                break;
            }
            
        #else 

            if ( lastConnectStatus == WL_CONNECTED) {
                enabled = true;
                break;
            } else if (lastConnectStatus == WL_CONNECT_FAILED) {
                Serial.print(F("FAIL TO CONNECT TO LAST CONFIGURED ACCESS POINT"));
                break;
            } else if (lastConnectStatus == WL_WRONG_PASSWORD) {
                Serial.print(F("WRONG PASSWORD"));
                break;            
            } else if (lastConnectStatus == WL_NO_SSID_AVAIL) {
                Serial.println(F("No any networks with specified network name | SSID"));
                break;
            }
        #endif

        
      }

      if (waitTime > 10) {
          Serial.print(F("Connect fail, exit by timeout"));
          return WL_CONNECT_FAILED;
      }

      delay(100); 
    }

    return lastConnectStatus;
}

bool WiFiManager::isAPmode() {
  return WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA ? true : false;
}

void WiFiManager::runAsAccesspoint(String ssid, String pass) {

    WiFi.softAPdisconnect();
    WiFi.disconnect();
    //WiFi.mode(WIFI_AP);

    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    int channel = 12;
    bool result = false;
    while (result == false && channel > 0 && channel <= 12) {
      result = WiFi.softAP(ssid.c_str(), pass.c_str(), channel);
      channel--;
      if (!result) {
        Serial.print(F("Fail to create WiFi network on Channel : "));
        Serial.println(channel);
      }
    }

    if (result) {
      result = WiFi.softAPConfig(local_ip, gateway, subnet);
      if (!result) {
         Serial.println(F("Fail to configure AP network"));
      }
    }
   
    delay(100);
    if (result) {
      Serial.print(F("Run as Access Point [ready] on Channel : "));
      Serial.println(channel);
    } else {
      Serial.print(F("Run as Access Point [FAIL]"));
    }

    enabled = true;
}

/* deprecated \ unused  replaced by webServerBase method instead */

String WiFiManager::getWiFiSelectorHtml() {
  
  String ssid;
  
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;

  bool hidden = false;
  int scanResult;

  String selectorHtml; 

  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/false);

  char buffer[256];
  sprintf(buffer, "Networks : %d", scanResult);
  
  selectorHtml += buffer;
  selectorHtml += "<select name=\"network\">";

  if (scanResult == 0) {
    selectorHtml += "<option>No networks</option>";
  } else if (scanResult > 0) {
 
    for (int8_t i = 0; i < scanResult; i++) {
      
      selectorHtml += buffer;
      #if defined(ESP32)
          WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel);
      #else 
          WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      #endif

      selectorHtml += "<option value=\"" + ssid + "\">" + ssid + "</option>";
      
       yield();
    }

    selectorHtml += "</select>";

  } else {

    sprintf(buffer, "WiFi scan error : %d", scanResult);    
    selectorHtml = buffer;
  
  }

  return selectorHtml;
}




