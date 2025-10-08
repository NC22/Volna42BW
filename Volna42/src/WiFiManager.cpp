#include "WifiManager.h"

#if defined(ESP32)
#include "esp_wifi.h"
#include "esp_log.h"
#endif

WiFiManager::WiFiManager() {
    
}

IPAddress WiFiManager::getIP() {
    return isAPmode() ? WiFi.softAPIP() : WiFi.localIP();
}

bool WiFiManager::stReconnectTick() {

  #if !defined(WIFI_AP_MODE_RECONNECT_TIMER) || WIFI_AP_MODE_RECONNECT_TIMER <= 0
    return false;
  #endif

  if (isAPmode() && millis() >= apReconnectTimer) {
    apReconnectTimer = millis() + (WIFI_AP_MODE_RECONNECT_TIMER * 1000);
    return true;
  }

  return false;
}

wl_status_t WiFiManager::connect(String sid, String password, bool resetDefault) {

    if (sid.length() <= 0) {
      return WL_NO_SSID_AVAIL;
    }

    enabledStatus = WIFI_MODE_NULL;

    if (resetDefault) {
      WiFi.softAPdisconnect();
      WiFi.disconnect();
    }

    WiFi.mode(WIFI_STA);

    #if defined(ESP32) && defined(WIFI_TX_POWER)
        esp_wifi_set_max_tx_power(WIFI_TX_POWER); 
    #endif

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
                enabledStatus = WiFi.getMode();
                
                // #if defined(ESP32) && defined(WIFI_TX_POWER)
                //    WiFi.setSleep(false);
                //    esp_wifi_set_ps(WIFI_PS_NONE);
                //    delay(500); 
                //    esp_netif_dhcpc_start(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF")); // force DCHP restart to fix http requests - they not work by default ~20-30sec 
                // 
                // #endif

                break;
            } else if (lastConnectStatus == WL_CONNECT_FAILED) {
                Serial.print(F("...WL_CONNECT_FAILED..."));
                break;
            } else if (lastConnectStatus == WL_NO_SSID_AVAIL) {
                Serial.print(F("...Not found by SSID..."));
                break;
            }
            
        #else 

            if ( lastConnectStatus == WL_CONNECTED) {
                enabledStatus = WiFi.getMode();
                break;
            } else if (lastConnectStatus == WL_CONNECT_FAILED) {
                Serial.print(F("...WL_CONNECT_FAILED..."));
                break;
            } else if (lastConnectStatus == WL_WRONG_PASSWORD) {
                Serial.print(F("...Wrong password..."));
                break;            
            } else if (lastConnectStatus == WL_NO_SSID_AVAIL) {
                Serial.print(F("...Not found by SSID..."));
                break;
            }
        #endif

        
      }

      if (waitTime > 10) {
          Serial.print(F("...Exit by timeout..."));
          return WL_CONNECT_FAILED;
      }

      delay(100); 
    }

    return lastConnectStatus;
}

bool WiFiManager::isAPmode() {
  return enabledStatus == WIFI_AP || enabledStatus == WIFI_AP_STA ? true : false;
}

void WiFiManager::runAsAccesspoint(String ssid, String pass) {

    enabledStatus = WIFI_MODE_NULL;
    WiFi.softAPdisconnect();
    WiFi.disconnect();    
    WiFi.persistent(false);

    #if defined(ESP32) && defined(WIFI_TX_POWER)
        esp_wifi_set_max_tx_power(WIFI_TX_POWER); 
    #endif
    //WiFi.mode(WIFI_AP);

    IPAddress local_ip(192, 168, 1, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    
    int channel = 12;
    #if defined(AP_NETWORK_WIFI_CHANNEL) 
        channel = AP_NETWORK_WIFI_CHANNEL;
    #endif

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

    apReconnectTimer = millis() + (WIFI_AP_MODE_RECONNECT_TIMER * 1000);
    enabledStatus = WiFi.getMode();
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




