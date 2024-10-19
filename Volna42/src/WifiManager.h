#include <Arduino.h>

#include <SystemDefines.h>
#if defined(ESP32)
    #include <WiFi.h>
#else 
    #include <ESP8266WiFi.h>
#endif

// ToDo check connection lost event?
/*
    currently used
    
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
*/

class WiFiManager {
    private :
        bool enabled = false;
    public:
        bool isEnabled();
        WiFiManager();
        wl_status_t connect(String sid, String password);
        String getWiFiSelectorHtml();
        IPAddress getIP();
        bool isAPmode();
        void runAsAccesspoint(String ssid, String pass);

        wl_status_t lastConnectStatus;
};

