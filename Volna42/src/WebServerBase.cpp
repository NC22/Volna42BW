/*
	Basic ESP WebServer
	
    Common class used to implement base web-server service + JS + HTML user interface (_uiTools/ui) in Volna 42 and other projects
	Require several Env methods for work with config variables
	
	Extendable by child classes, but needs to be reworked a bit to move env calls outside, to make it more flexible and independent
	
    todo - replace Env direct calls by struct with function void referenses \ callbacks & check them by null pointer to prevent crashes
    option to get sys messages separatly
*/

#include <WebServerBase.h>
// #include "ArduinoJson.h"

WebServerBase::WebServerBase(Env * aenv, int port) {
    #if defined(ESP32)
        server = new WebServer(port);
    #else 
        server = new ESP8266WebServer(port);
       // ESP8266WebServer server = ESP8266WebServer(port);
    #endif
    
    env = aenv;
}

void WebServerBase::tick() {
    // server->client().connected()
    server->handleClient();    
}

bool WebServerBase::defaultRoute(bool return404) {
   
    if (server->uri() == "/" || server->uri() == "/home") {
        showDefaultPage();
    } else if (server->uri().indexOf("/out/env.js") != -1) {
        getEnvJS();
    } else if (server->uri().indexOf("/out/style.css") != -1) {
        getStyleCss();
    } else if (server->uri().indexOf("/out/core.js") != -1) {
        getCoreJs();
    } else if (server->uri().indexOf("/out/tools.js") != -1) {
        getToolsJS();
    } else if (server->uri().indexOf("/out/tools.list.js") != -1) {
        getToolsListJS();
    } else if (server->uri().indexOf("/out/tools.select.js") != -1) {
        getToolsSelectJS();
    } else if (server->uri().indexOf("/api/reset") != -1) {
        apiResetConfig();
    } else if (server->uri().indexOf("/api/reboot") != -1) {
        apiReboot();
    } else if (server->uri().indexOf("/api/wifilist") != -1) {
        apiGetWiFiList();
    } else if (server->uri().indexOf("/api/getinfo") != -1) {
        apiGetInfo();
    } else if (server->uri().indexOf("/api/commit") != -1) {
        apiUpdateConfig();
    } else if (server->uri().indexOf("/api/clearrom") != -1) {
        apiClearRom();
    } else if (return404) {
                
        String message = "404 Not Found\n\n";
        message += "URL: ";
        message += server->uri();
        message += "\nMethod: ";
        message += (server->method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server->args();
        message += "\n";

        server->send(404, "text/plain", message);

    } else {
		return false;
	}

    return true;
}

void WebServerBase::router() {
    defaultRoute(true);
}

void WebServerBase::runConfigured() {

    /*
    auto showDefaultPageCb = std::bind(&WebServerBase::showDefaultPage, this);

    server->on("/", showDefaultPageCb);
    server->on("/home", showDefaultPageCb);

    server->on("/out/env.js", std::bind(&WebServerBase::getEnvJS, this));
    server->on("/out/style.css", std::bind(&WebServerBase::getStyleCss, this));
    server->on("/out/core.js", std::bind(&WebServerBase::getCoreJs, this));
    server->on("/out/tools.js", std::bind(&WebServerBase::getToolsJS, this));

    server->on("/api/reset", std::bind(&WebServerBase::apiResetConfig, this)); 
    server->on("/api/reboot", std::bind(&WebServerBase::apiReboot, this)); 

    server->on("/api/wifilist", std::bind(&WebServerBase::apiGetWiFiList, this));
    server->on("/api/getinfo", std::bind(&WebServerBase::apiGetInfo, this));
    server->on("/api/commit", std::bind(&WebServerBase::apiUpdateConfig, this));
     server->on("/api/clearrom", std::bind(&WebServerBase::apiClearRom, this));
    */

    server->begin(); 
    // manual routing -> more flexible, less memory use on bind every route
    server->onNotFound([this]() { this->router(); });
    // server->onNotFound(std::bind(&WebServerBase::router, this)); 
}

void WebServerBase::getEnvJS() {

    if (ramFriendlyMode) {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_env_js);

        server->send(200, "text/javascript", "");
        
        outputROMData(webdata_env_js, webdataSize_env_js);
        server->sendContent("");

    } else {

         server->sendHeader("Cache-Control", "max-age=31536000");
         server->setContentLength(webdataSize_env_js);
         server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_env_js)); 
    }
}

void WebServerBase::getStyleCss() {  
  
    if (ramFriendlyMode) {
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_style_css);

        server->send(200, "text/css", "");

        outputROMData(webdata_style_css, webdataSize_style_css);
        server->sendContent("");
    } else {
            
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_style_css);
        server->send(200, "text/css; charset=utf-8", FPSTR(webdata_style_css)); 
    }
 
}

void WebServerBase::outputROMData(const char * data, unsigned int dataSize) {

    String buffer = "";
    buffer.reserve(bufferSize); 

    for (unsigned int i = 0; i < dataSize; i++) {
        char b = pgm_read_byte(&data[i]);

        buffer += b;

        if (buffer.length() >= bufferSize) {
            server->sendContent(buffer);
            buffer = "";
        }
    }

    if (buffer.length() > 0) {
        server->sendContent(buffer);
    }
}

void WebServerBase::getCoreJs() { 
    
    if (ramFriendlyMode) {
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyEnv_js);

        server->send(200, "text/javascript", "");

        outputROMData(webdata_KellyEnv_js, webdataSize_KellyEnv_js);
        server->sendContent("");
    }   else {
        
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyEnv_js);
        server->send(200, "text/css; charset=utf-8", FPSTR(webdata_KellyEnv_js)); 
    }
    // server->setContentLength(webdataSize_KellyEnv_js);
 
    
    // server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_KellyEnv_js)); 
}

void WebServerBase::getToolsJS() {   

    if (ramFriendlyMode) {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyTools_js);
        
        server->send(200, "text/javascript", "");

        // CONTENT_LENGTH_UNKNOWN 
        
        outputROMData(webdata_KellyTools_js, webdataSize_KellyTools_js);
        server->sendContent("");

    } else {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyTools_js);
        server->send(200, "text/css; charset=utf-8", FPSTR(webdata_KellyTools_js)); 
    }
}

void WebServerBase::getToolsListJS() {   

    if (ramFriendlyMode) {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyTools_list_widget_js);
        
        server->send(200, "text/javascript", "");
        outputROMData(webdata_KellyTools_list_widget_js, webdataSize_KellyTools_list_widget_js);
        server->sendContent("");

    } else {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyTools_list_widget_js);
        server->send(200, "text/css; charset=utf-8", FPSTR(webdata_KellyTools_list_widget_js)); 
    }
}

void WebServerBase::getToolsSelectJS() {   

    server->sendHeader("Cache-Control", "max-age=31536000");
    // server->setContentLength(webdataSize_KellyTools_select_widget_js);
    server->send(200, "text/css; charset=utf-8", FPSTR(webdata_KellyTools_select_widget_js)); 
}


void WebServerBase::apiGetInfo() {
    server->send(200, "application/json", "{" + getInfo() + "}"); 
}

void WebServerBase::apiClearRom() {
    
    if (server->method() != HTTP_POST) {
        server->send(200, "application/json", "{\"status\":\"fail\"}");  
        return;
    }

    String result = "fail";

    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "confirm-clear") {            
            result = env->getConfig()->commitEEPROM(true) ? "ok" : "fail";
            break; 
        }
    }
    server->send(200, "application/json", "{\"status\":\"" + result + "\"}"); 
}

void WebServerBase::apiResetConfig() {

    if (server->method() != HTTP_POST) {
        server->send(200, "application/json", "{\"status\":\"no\"}");  
        return;
    }

    bool resetConfirm = false;
    bool resetWifi = false;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "reset" && server->arg(i) == "1") {

            resetConfirm = true;
            
        } else if (server->argName(i) == "reset-wifi" && server->arg(i) == "1") {
        
            resetWifi = true;            
        }
    }        
 
    if (resetConfirm) {
        
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 

        env->resetConfig(resetWifi);         
        env->restart("Config reset");

    } else {
        server->send(200, "application/json", "{\"status\":\"no\"}");  
    }
}


void WebServerBase::apiGetWiFiList() {

    server->send(200, "application/json", getWiFiListJSON()); 
}

String WebServerBase::getInfo() {

    String json = ""; // "{";

    json += "\"wifiConnected\":";
    wl_status_t status = WiFi.status(); // todo get status from wifimanager - lastConnectStatus

    String defaultW = ",\"wifiName\":\"" + env->getConfig()->getString(cWifiNetwork) + "\"";
 
        #if defined(ESP32)

            if ( status == WL_CONNECTED) {
                json += "1,\"wifiName\":\"" + WiFi.SSID() + "\",\"wifiDbm\":" + String(WiFi.RSSI());        
            } else if (status == WL_CONNECT_FAILED) {
                json += "-1" + defaultW ;
            } else if (status == WL_NO_SSID_AVAIL) {
                json += "-3" + defaultW;
            } else {
                json += "-4" + defaultW;
            }
            
        #else 

            
            if ( status == WL_CONNECTED) {
                json += "1,\"wifiName\":\"" + WiFi.SSID() + "\",\"wifiDbm\":" + String(WiFi.RSSI());        
            } else if (status == WL_CONNECT_FAILED) {
                json += "-1" + defaultW ;
            } else if (status == WL_WRONG_PASSWORD) {
                json += "-2" + defaultW;
            } else if (status == WL_NO_SSID_AVAIL) {
                json += "-3" + defaultW;
            } else {
                json += "-4" + defaultW;
            }

        #endif


    json += ",\"ram\":" + String(ESP.getFreeHeap());
    json += ",\"dt\":\"";
    time_t now = time(nullptr);
    struct tm stnow;
    localtime_r(&now, &stnow);

    char buffer[64];
    strftime(buffer, sizeof(buffer), "%H:%M:%S %d.%m.%Y", &stnow);
    
    json += buffer;
    json += "\"";

   // json += "}";

    return json;
}

String WebServerBase::getWiFiListJSON() {
     
  String ssid;
  
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;

  bool hidden = false;
  int scanResult;
  
  uint8_t encType;  
  uint8_t hiddenT;
  
  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/ false, /*hidden=*/ false);
  String json = "{\"networks\":[";

  if (scanResult == 0) {

  } else if (scanResult > 0) {
    
    if (scanResult > 40) scanResult = 40;
    
    for (int8_t i = 0; i < scanResult; i++) {
      

    #if defined(ESP32)
        if (!WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel)) continue;
        encType = 0;
    #else 
        if (!WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden)) continue;
        encType = encryptionType == ENC_TYPE_NONE ? 0 : 1;
    #endif

       hiddenT = hidden ? 1 : 0;
       
       json += "{\"name\":\"" + ssid + "\",\"encrypted\":" + String(encType) + ",\"dbm\":" + String(rssi) + ",\"channel\":" + String(channel) + ",\"hidden\":" + String(hiddenT) + "}";

       if (i < scanResult - 1) json += ',';

       yield();
    }

  } else {

    return "{\"error\":\"WiFi scan error : " + String(scanResult) + "\"}";

  }

  json += "]}";

  return json;
}

void WebServerBase::apiReboot() {

    if (server->method() != HTTP_POST) {
        server->send(200, "application/json", "{\"status\":\"no\"}");  
        return;
    }
    
    bool rebootConfirm = false;
    bool skipValidate = true;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "reboot" && server->arg(i) == "1") {
            rebootConfirm = true;
        } else if (server->argName(i) == "skip-validate" && server->arg(i) == "1") {
            skipValidate = true;
        }
    } 

    if (rebootConfirm) {
        
        server->send(200, "application/json", "{\"status\":\"ok\"}");  
        env->restart("Reset-webAPI", skipValidate);
               
    } else server->send(200, "application/json", "{\"status\":\"no\"}"); 

}

void WebServerBase::apiUpdateConfig() {

    unsigned int argCollected = 0; unsigned int affectedKeys = 0;
    String ignoredKey = ""; // invalid, empty or incompatible data for field type
    String notExistKey = "";
    bool updateScreen = false;
    bool skipCommit = false;
    bool restartRequired = false;
    std::vector<cfgOptionKeys> updatedKeys;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "commit") {
            if (server->arg(i) == "skip") skipCommit = true;
            continue;
        }

        argCollected++;
        
        String rawValue = server->arg(i); 
        cfgOptionKeys optionKey = env->getConfig()->getOptionKey(server->argName(i).c_str());
        
        if (optionKey == cNull) {
            notExistKey += notExistKey ? "," + server->argName(i) : server->argName(i);
            continue;
        }

        // common important filter for password, todo - notify from client side if needed
        if (optionKey == cWifiPassword || optionKey == cWifiPasswordFallback) {
            if (rawValue.length() > 0) {
                if (rawValue.length() < 8 || rawValue.length() > 32) {
                    Serial.println("wrong password length (unsetted or min 8 max 32 letters)");
                    ignoredKey += ignoredKey ? "," + server->argName(i) : server->argName(i);
                    continue;
                }
            }
        }

        if (!env->getConfig()->set(optionKey, rawValue)) {
            ignoredKey += ignoredKey ? "," + server->argName(i) : server->argName(i);
            continue;
        } else {
            affectedKeys++;
            updatedKeys.push_back(optionKey);
        }

        if (!env->getConfig()->isReloadRequired(optionKey)) {
            updateScreen = true;
        } else {
            restartRequired = true;
        }
    } 

    env->validateConfig(-1, &updatedKeys);    
    if (!skipCommit && affectedKeys > 0) env->commitConfig();
    else {
        Serial.println("skip commit");
    }

    Serial.println("apiUpdateConfig : total : " + String(argCollected) + ", ignored : [" + ignoredKey + "] | not exist : [" + notExistKey + "]");

    String restart = restartRequired ? "true" : "false";
    server->send(200, "application/json", "{\"restart\":" + restart + ",\"status\":\"ok\",\"ignored\":\"" + ignoredKey + "\",\"notexist\":\"" + notExistKey + "\"}"); 
    
    // todo - add to env access to time, wifi, to prevent reload in some cases
    if (updateScreen) {
                
        Serial.println("no need restart...");
         
    } else {
        
        Serial.println("need restart...");
         
        // delay(500);
        // ESP.restart();
    }
}


void WebServerBase::showDefaultPage() {
    
    String currentCfg = env->getConfig()->getOptionsJSON();
    server->setContentLength(currentCfg.length() + webdataSize_client_html + webdataSize_client_end_html + webdataSize_init_js);

    // todo - not reconstruct string if possible - this use RAM, server->send can use PROGMEM pointers -> see getToolsSelectJS for ex.
        
    if (ramFriendlyMode) {

        // server->send(200, "text/html; charset=utf-8", FPSTR(webdata_client_html) + scripts ); -- join strings - unsafe
            
            server->send(200, "text/html", "");
            
            // outputROMData(webdata_client_html, webdataSize_client_html);
            
            server->sendContent(FPSTR(webdata_client_html));
            server->sendContent(currentCfg);
            
            server->sendContent(FPSTR(webdata_init_js));
            server->sendContent(FPSTR(webdata_client_end_html));
            // outputROMData(webdata_init_js, webdataSize_init_js);
            // outputROMData(webdata_client_end_html, webdataSize_client_end_html);
            
            server->sendContent("");

    } else {

        String response = "";
               response += FPSTR(webdata_client_html);
               response += currentCfg;
               response += FPSTR(webdata_init_js);
               response += FPSTR(webdata_client_end_html);
              
        server->send(200, "text/html; charset=utf-8", response); 
    }
}