#include <Arduino.h>

#include <UserDefines.h>
#include <SystemDefines.h>
#if defined(ESP32)
    #include <WiFi.h>
#else 
    #include <ESP8266WiFi.h>
    #define WIFI_MODE_NULL WIFI_OFF
#endif

/*
    ToDo - check connection lost event? no any issues with current setup detected for now

    currently used :
    
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
*/

class WiFiManager {
    private :
    public:
        unsigned long apReconnectTimer; // initialized on AP mode enabled (runAsAccesspoint)
        bool stReconnectTick();

        #if defined(ESP32)
            wifi_mode_t enabledStatus = WIFI_MODE_NULL;
        #else 
            WiFiMode_t enabledStatus = WIFI_MODE_NULL;
        #endif

        WiFiManager();
        wl_status_t connect(String sid, String password, bool resetDefault = false);
        String getWiFiSelectorHtml();
        IPAddress getIP();
        void prepareToSleep();
        bool isAPmode();
        void runAsAccesspoint(String ssid, String pass);

        wl_status_t lastConnectStatus;
};

