#ifndef WebServerBase_h
#define WebServerBase_h

#include <Arduino.h>

#if defined(ESP32)

    #include <WebServer.h>
    #include <WiFi.h>
#else 
    #include <ESP8266WebServer.h>
    #include <ESP8266WiFi.h>
#endif


#include <Env.h>

typedef struct {
  const char* data;
  unsigned int size;
} webData;

// todo - move client + env to uiLocal folder - it will be different from project to project
// add to client - addition initialization & script for generate & upload binary images to screen
// in env - add config options for mode select - default - show temp & hum, slave - wait framebuffer from host (configurable host string)

#include <ui/out/client.h>
#include <ui/out/client_end.h>
#include <ui/out/init.h>
#include <ui/out/KellyEnv.h>
#include <ui/out/KellyTools.h>
#include <ui/out/style.h>
#include <ui/out/env.h>

class WebServerBase {
    private : 

        void getEnvJS();
        void getToolsJS();
        void getCoreJs();
        void getStyleCss();
     
        void apiGetInfo();
        void apiGetWiFiList();
        void apiUpdateConfig();
        void apiResetConfig();
        void apiReboot();
        void apiClearRom();

        void showDefaultPage();

    protected :        
        #if defined(ESP32)
            WebServer * server = NULL;
        #else 
            ESP8266WebServer * server = NULL;
        #endif
        Env * env;
        int port;

        bool defaultRoute(bool return404 = false);

        virtual void router();
        virtual String getInfo();
        String getWiFiListJSON();
         
        void outputROMData(const char * data, unsigned int size);

    public:
       
        WebServerBase(Env * env, int port = 80);
        virtual void runConfigured();
        void tick();

        bool ramFriendlyMode = false;
        unsigned int bufferSize = 512;
};




#endif