#include "Env.h"

#if defined(ESP32)
  #include <FS.h>
  #include <SPIFFS.h>
#else 
  #include <FS.h>
  #include <LittleFS.h>
#endif

Env::Env() {

    lastState.sleepTimeCurrent = 1000;
    lastState.onBattery = false;
    partialUpdateRequired = false;
    
    secondTimerStart = millis();
    cuiWidgets.clear();
    resetTimers(true);
    resetPartialData(); // pos will restore from RTC memory

    #if defined(SLEEP_SWITCH_PIN)    
      pinMode(SLEEP_SWITCH_PIN, INPUT);
    #endif

}

void Env::begin() {

  cfg.version = cdConfigVersion;
  cfg.excludeOptions.clear();

  #if !defined(PARTIAL_UPDATE_SUPPORT)  
  cfg.excludeOptions.push_back(cUpdateMinutes); 
  #endif
  cfg.excludeOptions.push_back(cScreenRotate); 

  validateConfig(getConfig()->loadEEPROM());
  
  initSensors();    


  #if defined(ESP32)
      esp_reset_reason_t reason = esp_reset_reason();
  #else

      
     int reason = -1;
     ESP.getResetReason();
     reason = resetInfo.reason;

  #endif
  
  // Проснулся после глубокого сна \ клик по кнопке "Reset" (считает как при выходе из сна в штатном режиме - таймер смещается и требуется полный сброс - вкл. выкл. или дождатся цикла синхронизации)
  // todo check wakeup states - при клике на ресет - причина остается та же, 
  // память не сбрасывается до полной потери питания - вполне возможно что так реализован сам микрочип esp8266

  if ( reason == REASON_DEEP_SLEEP_AWAKE ) {

      Serial.println(F("[Boot] WakeUp automaticly -> restore stats"));     
      
      if (restoreRTCmem()) {

        lastState.timeConfigured = true;
        if (lastState.connectTimes > 100000) lastState.connectTimes = 0;

        // режим выхода из сна для обновления минут, возможно чего то еще в дальнейшем
        // В данном режиме время сна если активна опция, время сна = PARTIAL_UPDATE_INTERVAL
        
        #if defined(PARTIAL_UPDATE_SUPPORT)

          if (lastState.updateMinutes) {

              lastState.sleepTimeCurrent -= PARTIAL_UPDATE_INTERVAL;
              lastState.t += PARTIAL_UPDATE_INTERVAL;
              defaultTime = lastState.t;

              if (lastState.sleepTimeCurrent > 0) {
                partialUpdateRequired = true;
              } else {
                partialUpdateRequired = false;
                lastState.sleepTimeCurrent = lastState.sleepTime;
              }
              
          } else {
                
            lastState.t += lastState.sleepTime;
            defaultTime = lastState.t;
          }

        #else 

           // В данном режиме время сна = lastState.sleepTime (определено пользовательским конфигом -> validateConfig-> applyConfigToRTC)

          lastState.t += lastState.sleepTime;
          defaultTime = lastState.t;

        #endif

      } else {
        Serial.println(F("[Boot] Invalid RTC memory time -> ignored, defaults used"));
      }

      // ignore partial mode wakeups, to keep counter correct

      if (!isPartialUpdateRequired()) {
        lastState.wakeUps += 1;

        if (lastState.cuiLoop) {
          lastState.cuiTimeCurrent -= lastState.sleepTime;

          if (lastState.cuiTimeCurrent <= 0) {
              if (lastState.cuiFileIndex) lastState.cuiFileIndex++;
              lastState.cuiTimeCurrent = CUI_LOOP_INTERVAL;
          } 
        }

        Serial.print(F("[Boot] : WakeUps : ")); Serial.print(lastState.wakeUps);
        Serial.print(F(" | time : ")); Serial.println(lastState.t);
      }

    } else {

      // Софтверный рестарт (например через web интерфейс)

      if ( reason == REASON_SOFT_RESTART ) {

        Serial.println(F("[Boot] Software reboot -> reset stats & check cui index"));

        // ToDo move to separate method - load next cui item by index from deepsleep (loop mode)
        // show something after reboot (for web ui actions without permanent save to flash)
        if (restoreRTCmem() && lastState.cuiFileIndex > -1) {
          
          String restoredName = "";
          cuiGetNameByIndex(lastState.cuiFileIndex, restoredName);
          cuiSetState(cuiName.length() > 0, restoredName);
          
          if (lastState.cuiResetOnReboot) {
            lastState.cuiResetOnReboot = false;
            lastState.cuiFileIndex = -1;
          }

          Serial.print(F("[Direct input] Restore CUI file by index - fileindex : ")); Serial.println(lastState.cuiFileIndex);
        }

      // чистый запуск
      } else {

        Serial.println(F("[Boot] Device reseted -> reset stats"));
      }
      
      setDefaultLastStateData();
    }

}

bool Env::restoreRTCmem() {
  
#if defined(ESP32)

    if (!readRTCUserMemory(0, (uint32_t*) &lastState, sizeof(lastState))) {
        lastState.cfgVersion = 0;
        lastState.t = 0;
    }
    
#else
    ESP.rtcUserMemoryRead(0, (uint32_t*) &lastState, sizeof(lastState));
#endif

    if (lastState.t < 100000 || lastState.cfgVersion != cdConfigVersion) {

      Serial.println(F("[restoreRTCmem] memory version mismatch -> reset")); 

      setDefaultLastStateData();
      applyConfigToRTC();

      return false;
    } else return true;
}

void Env::setDefaultLastStateData() {

    Serial.println(F("[Boot] Reset all stats"));

    /*
      states for :

      -- sleepTime
      -- sleepTimeCurrent
      -- updateMinutes validated during config loading
      -- cuiFileIndex
      -- cuiLoop
      -- cuiTimeCurrent

      applyed during config validation by applyConfigToRTC or on RTC read fail reset
    */
    
    lastState.cuiResetOnReboot = false;
    lastState.wakeUps = 0;
    lastState.connectTimes = 0;
    lastState.lastTelemetrySize = 0;
    lastState.extData.isDataValid = false;
    lastState.t = defaultTime;
    lastState.onBattery = false;
    lastState.lowBatTick = 0;
    lastState.timeConfigured = false;
    lastState.cfgVersion = cdConfigVersion;

    lastState.lastPartialPos.xStart = -1;
    lastState.lastPartialPos.xEnd = -1;
}

// used time stored in RTC or default timestamp assigned in Env class for defaultTime

void Env::initDefaultTime() {

    String defaultTimestamp = getConfig()->cfgValues[cTimestamp];

    if (!lastState.timeConfigured && defaultTimestamp.length() > 0) {

      struct tm tm;
      time_t t;

      if (strptime(defaultTimestamp.c_str(), "%Y-%m-%d %H:%M:%S", &tm) == NULL) {
        
        Serial.println(F("Default time reassigned by user config - WRONG FORMAT"));

      } else {

        tm.tm_isdst = -1; 
        t = mktime(&tm);
        defaultTime = t;
        Serial.println(F("Default time reassigned by user config"));
      }

    } else {

      setenv("TZ", timezone.c_str(), 1);
      tzset();
    }

    
    Serial.println(F("Restore time by defaults (RTC or default setting)"));
    timeval tv = { defaultTime, 0 };
    settimeofday(&tv, nullptr);
}

void Env::tick() {

    if (!workEnabled) return;

    unsigned long now = millis();
    if (now - secondTimerStart >= 1000) {        
        secondTimerStart = now;  
        lastState.sleepTimeCurrent--;

        if (lastState.updateMinutes) {
            minuteTimer--;
            // Serial.println(minuteTimer);
        }

        if (lastState.cuiLoop) {
            lastState.cuiTimeCurrent--;
        }
    }

    if (lastState.cuiLoop && lastState.cuiTimeCurrent <= 0) {
        if (lastState.cuiFileIndex) lastState.cuiFileIndex++;
        lastState.cuiTimeCurrent = CUI_LOOP_INTERVAL;
    }

    if (lastState.updateMinutes && minuteTimer <= 0 && lastState.sleepTimeCurrent > 0) { // sleepTimeCurrent (full screen update ticker) in priority

        resetTimers(true);
        updateTime();
        screen->updatePartialClock();
        Serial.print(F("[updatePartialClock]"));

    } else if (lastState.sleepTimeCurrent <= 0) {
        
        resetTimers(true);
        lastState.sleepTimeCurrent = lastState.sleepTime;
        updateTelemetry();
        lastState.wakeUps++;
        
        if (isSyncRequired()) {
            updateExtSensorData();
            sync();
            mqttSendCurrentData();
        }
        
        updateScreen();
    } 
}

bool Env::isPartialUpdateRequired() {
  return partialUpdateRequired;
}

// load new external sensor info required every syncEvery wakeups

bool Env::isSyncRequired() {
  
    Serial.print(F("[isSyncRequired] : WakeUps : "));
    Serial.print(lastState.wakeUps); Serial.print(F(" / ")); Serial.print(syncEvery); 
    Serial.print(F(" | Telementry array size : ")); Serial.println(lastState.lastTelemetrySize);  

    // triggers ON :
    // - NTP time is incorrect
    // - first time start (wakeUps = 0)
    // - wakeuped needed times and its sync time (syncEvery - setuped by user config, default 6) 
    // [removed] telemetry cache size is overflow - lastState.lastTelemetrySize >= telemetryBufferMax

    if (lastState.wakeUps == 0 || lastState.wakeUps >= syncEvery || lastState.t < 1000000000) {        
      return true;
    } else {
      return false;
    } 
}

bool Env::setupNTP() {
    if (ntp) return true;

    String ntpServer = getConfig()->cfgValues[cNtpHosts];
    if (ntpServer.equals(F("off"))) {

      Serial.println(F("No NTP servers specified, setting time by default timestamp"));
      initDefaultTime();
      return false;
    }

    ntp = true;

    #if defined(ESP32)
      configTzTime(timezone.c_str(), ntpServer.c_str());
    #else 
      configTime(timezone.c_str(), ntpServer);
    #endif


    Serial.print(F("Waiting for NTP time sync: "));

    int i = 0;
    time_t now = time(nullptr);
    
    // timeout = 30sec
    
    while (now < 1000000000) {
        
      now = time(nullptr);
      i++;
      
      if (i > 60) {

        Serial.println(F("Time sync failed!"));
        lastError = "NTP server is not accessable. Default time setted (by config or firmware variable)";
        initDefaultTime();

        return false;
      };
      
      Serial.print(".");
      delay(500);
    }

    Serial.println(F("NTP ready!"));
    defaultTime = time(nullptr);
    
    lastState.t = defaultTime;
    lastState.timeConfigured = true;

    // lastState.syncT = defaultTime;
    
    return true;
}

// Call to reset internal sensors data buffer
// also we can send buffered telemetry somewhere if needed before clear
// reset telemetry buffer structure
// reset wakeup counter

void Env::sync() {

    if (lastState.lastTelemetrySize <= 0) {
      updateTelemetry();
    }

    lastState.wakeUps = 0;

    if (lastState.lastTelemetrySize > 1) {
        lastState.lastTelemetry[0] = lastState.lastTelemetry[lastState.lastTelemetrySize-1];
        lastState.lastTelemetrySize = 1;
    }
}

rtcData & Env::getCurrentState() {  
   return lastState;
}

void Env::saveCurrentState()  {    

#if defined(ESP32)

  writeRTCUserMemory(0, (uint32_t*)&lastState, sizeof(lastState));
#else
    ESP.rtcUserMemoryWrite (0, (uint32_t*) &lastState, sizeof(lastState));
#endif
}

void Env::sleep()  {
    
    Serial.println("[Deep sleep]");  
    lastState.t = time(nullptr);
    saveCurrentState();

    if (lastState.updateMinutes) {

      #if defined(PARTIAL_UPDATE_INTERVAL)
        ESP.deepSleep(PARTIAL_UPDATE_INTERVAL * 1000000);
      #else
         ESP.deepSleep(60 * 1000000);
      #endif

    } else {
      ESP.deepSleep(lastState.sleepTime * 1000000);
    }
}

void Env::keepTelemetry(int key)  {
        
    lastState.lastTelemetry[key].temperature = readTemperature();
    lastState.lastTelemetry[key].pressure = readPressure();
    lastState.lastTelemetry[key].humidity = readHumidity();
}

bool Env::isSleepRequired() {
    
    #if defined(BAT_NO) || defined(SLEEP_ALWAYS_ON)
      return false;
    #endif

    #if defined(SLEEP_SWITCH_PIN)

      if (digitalRead(SLEEP_SWITCH_PIN) == LOW) {
        return true;
      } else return false;

    #endif

    return isOnBattery();
}

String Env::getTelemetryJSON() {

    String json = "[";
    std::vector<String> paramList;
    for ( int b = 0; b < lastState.lastTelemetrySize; b++) {
      
      json += "{";
        json += "\"temperature\":" + String(lastState.lastTelemetry[b].temperature) + ",";
        json += "\"humidity\":" + String(lastState.lastTelemetry[b].humidity) + ",";
        json += "\"pressure\":" + String(lastState.lastTelemetry[b].pressure) + ",";
        json += "\"bat\":" + String(lastState.lastTelemetry[b].bat) + ",";
        json += "\"t\":" + String(lastState.lastTelemetry[b].t);
      json += "}";

      if (b < lastState.lastTelemetrySize-1) json += ",";
    }

  json += "]";

  return "{\"id\":\"4in2einkTermometr\",\"data\":" + json + "}";
}

float Env::validateExtTelemetryVal(String v) {

    if (v.length() <= 0) return -1000;

    float nv = -1000;
    if(sscanf(v.c_str(), "%f", &nv) != 1) {
        return -1000; 
    }

    return nv;
}

void Env::validateExtTelemetryData(externalSensorData & newData, String tt) {

  int len = tt.length();
  int itemN = 0;

  newData.temperature = -1000;
  newData.humidity = -1000;
  newData.bat = -1000;

  String sensorData = "";

  for (int i = 0; i < len; i++) {

      unsigned char chr = (unsigned char) tt[i];
      if (chr >= 45 && chr <= 57 && chr != 47) { // 0-9 . - 
          sensorData += tt[i];
      }

      if (tt[i] == ',' || i == len-1) {

        Serial.println(F("Telemetry item : "));
        Serial.println(sensorData);

        if (itemN == 0) {

            newData.temperature = validateExtTelemetryVal(sensorData);            
            Serial.println(newData.temperature);

        } else if (itemN == 1) {

            newData.humidity = validateExtTelemetryVal(sensorData);
            Serial.println(newData.humidity);

        } else if (itemN == 2) {

            newData.bat = -1000;
            if(sscanf(sensorData.c_str(), "%d", &newData.bat) != 1) {
                newData.bat = -1000;
            }

                 if (newData.bat > 100) newData.bat = 100;
            else if (newData.bat < 0) newData.bat = -1000;

            Serial.println(newData.bat);
        }

        itemN++;        
        sensorData = "";
        if (itemN <= 2) continue;else break;
      }
  }

  if (newData.temperature > -1000) {

      newData.isDataValid = true;      
      newData.t = time(nullptr);

  } else {      

      newData.isDataValid = false;
      Serial.println(F("validateExtTelemetryData fail"));
  }
}

void Env::mqttMessageReceivedCallback(char* topic, uint8_t* payload, unsigned int length) {
 
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

bool Env::mqttIsHAMode() {
  return cfg.cfgValues[cMqttHAPrefix].length() > 0;
}

void Env::mqttHAAutodetectionInit() {

  if (_mqttClient.connected() && mqttIsHAMode()) {
      
      // temporary increase max send buffer size (default = 256)
      if (_mqttClient.getBufferSize() < 768) {
        _mqttClient.setBufferSize(768);
      }

      Serial.println(F("Home Assistant auto-discovery device initialize ...")); 
      String tmp;
      String base;
      String comm;

      Serial.println(rawMqttIds[0]); 
      Serial.println(rawMqttIds[1]); 

      String tstate = "home/" + rawMqttIds[0];

      tmp = "{\"device_class\":\"temperature\",\"name\":\"temperature\",\"state_topic\":\"" + tstate + "/info\",\"unit_of_measurement\":\"°C\",\"value_template\":\"{{value_json.temperature}}\",\"unique_id\":\"" + rawMqttIds[0] + "_temperature\",\"device\":{\"identifiers\":[\"" + rawMqttIds[0] + "_device\"],\"name\":\"" + rawMqttIds[1] + "\",\"model\":\"KellyC42ESP8266\",\"manufacturer\":\"Nradiowave\"}}";
      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/temperature/config");
      Serial.println(comm); 
      Serial.println(tmp); 

      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - temperature info")); 
        lastError = F("Error in HA MQTT initialization. Possible small buffer size, check - setBufferSize");
      }

      tmp.replace(F("temperature"), F("humidity"));
      tmp.replace(F("°C"), F("%"));

      //tmp = "{\"device_class\":\"humidity\",\"name\":\"" + rawMqttIds[1]  + " Humidity\",\"state_topic\":\"" + tstate + "/info\",\"unit_of_measurement\":\"%\",\"value_template\":\"{{value_json.humidity}}\"}"; // , " + device + "
      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/humidity/config");      
      Serial.println(comm); 
      Serial.println(tmp); 

      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - humidity info")); 
      }

      tmp.replace(F("humidity"), F("battery"));

      //tmp = "{\"device_class\":\"humidity\",\"name\":\"" + rawMqttIds[1]  + " Humidity\",\"state_topic\":\"" + tstate + "/info\",\"unit_of_measurement\":\"%\",\"value_template\":\"{{value_json.humidity}}\"}"; // , " + device + "
      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/battery/config");      

      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - battery info")); 
      }

      _mqttClient.setBufferSize(256);
      // comm = tstate + "/state";
      // _mqttClient.publish(comm.c_str(), "online", true);
  }
}

void Env::mqttInit() {
    
    if (!mqtt) {
      if (getConfig()->cfgValues[cMqttHost].length() > 0) {

          _mqttClient.setClient(_wifiClient);
          _mqttClient.setServer(getConfig()->cfgValues[cMqttHost].c_str(), getConfig()->getInt(cMqttPort)); 
          _mqttClient.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {this->mqttMessageReceivedCallback(topic, payload, length);});

          Serial.println(F("MQTT server connection create ...")); 

      } else {
        Serial.println(F("MQTT Not configured ... skip")); 
        return;
      }
    }

    if (!_mqttClient.connected()) {
        
        Serial.print(F("generate clientId ---- "));
        // Create a random client ID
        String clientId = FPSTR(cfgMqttClientIdPrefix);
        clientId += String(random(0xffff), HEX);

        Serial.println(clientId);

        bool result = false;
        if (cfg.cfgValues[cMqttLogin].length() > 0) {
            Serial.print(F("MQTT connect... with pass")); 
            result = _mqttClient.connect(clientId.c_str(), cfg.cfgValues[cMqttLogin].c_str(), cfg.cfgValues[cMqttPassword].c_str());
        } else {
            Serial.print(F("MQTT connect... anon")); 
            result = _mqttClient.connect(clientId.c_str());
        }

        if (result) {

            Serial.println(F("...OK"));
            // client.subscribe("inTopic");

        } else {
            lastError = "Fail to connect MQTT (" + String(_mqttClient.state()) + ")";
            Serial.print(F("...FAIL, rc="));
            Serial.print(_mqttClient.state());
        }

      Serial.println();

    } else {
        Serial.print(F("MQTT already connected...")); 
    }

    if (!mqtt) {
        mqttHAAutodetectionInit();
    }
    
    mqtt = true;
}

/*
  send current internal sensors data to MQTT server if configured
  implemented Domoticz style & Home Assistant style
*/

bool Env::mqttSendCurrentData() {

    mqttSuccess = false;
    mqttInit();

    if (!mqtt) return false;
    if (!_mqttClient.connected()) {
      Serial.println(F("MQTT init failed...skip publish")); 
      return false;
    }

    Serial.println(F("MQTT check connection...")); 

    if (_mqttClient.connected()) {

       Serial.print(F("MQTT send...")); 

      // Domoticz connection mode

      if (!mqttIsHAMode()) {
        
        // /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT;BAR;BAR_FOR
        // HUM 0=Normal 1=Comfortable 2=Dry 3=Wet
        // BAR_FOR 0 = No Info 1 = Sunny 2 = Partly Cloudy 3 = Cloudy 4 = Rain
        // /json.htm?type=command&param=udevice&idx=<idx>&nvalue=<numeric value>&svalue=<string value>&rssi=<numeric value>&battery=<numeric value>
        // battery level 255 = no battery device, else 0-100
        // rssi=10&battery=89 signal level (default 12) 
        // https://www.domoticz.com/wiki/Domoticz_API/JSON_URL%27s

        float h = readHumidity();
        unsigned int hStat = 0;
        if (h <= 20) hStat = 2;
        else if (h >= 40 && h <= 60) hStat = 1;
        else if (h > 60) hStat = 3;

        String tmp;
        String metric;                
        int testMqttId;

        
        Serial.println(F("[Domoticz] Send data")); 
        
        for (int i = 2; i >= 0; i--) {
          
          if (i+1 > (int) rawMqttIds.size()) {
            testMqttId = -1;
          } else {
            testMqttId = cfg.sanitizeInt(rawMqttIds[i]);
            if (cfg.sanitizeError) testMqttId = -1;
          }
          
               if (i == 2) metric = String(readTemperature());  // temp
          else if (i == 1) metric += ";" + String(h) + ";" + String(hStat); // temp & hum
          else if (i == 0) metric += ";" + String((float) (readPressure() / 100.0)) + ";0"; // temp & hum & bar

          tmp = "{\"idx\":" + String(testMqttId) + ",\"nvalue\":0,\"svalue\":\"" + metric + "\"}";

          //    Serial.println(i); 
          //    Serial.println(metric); 

          if (testMqttId <= 0) {
              
              Serial.println(F("Bad or unsetted Domoticz device id --> skip send")); 
              Serial.println(i); Serial.println(testMqttId); 
              continue;

          } else {

              _mqttClient.publish(cfg.cfgValues[cMqttPrefixIn].c_str(), tmp.c_str());
              mqttSuccess = true;
          }
        }


      // Home assistant

      } else {
        
        String tstate; String payload;
        
        tstate = "home/" + rawMqttIds[0] + "/info";

        payload = "{\"temperature\":" + String(readTemperature()) + ",\"humidity\":" + String(readHumidity());

        if (isOnBattery()) {
          if (lastState.lastTelemetrySize > 0) {
              payload += ",\"battery\":" + String((int) round(getBatteryLvlfromV(lastState.lastTelemetrySize)));
          } else payload += ",\"battery\":100";
        } else payload += ",\"battery\":100";

        payload += "}";

        Serial.println(tstate); 
        Serial.println(payload); 
        if (!_mqttClient.publish(tstate.c_str(), payload.c_str())) {

           Serial.println("FAIL to send INFO");
        } else {
          mqttSuccess = true;
        }

        /*
        tstate = "home/" + rawMqttIds[0] + "/humidity";
        payload = String("{\"humidity\": ") + String(readHumidity()) + "}";
        Serial.println(tstate); 
        Serial.println(payload); 
        _mqttClient.publish(tstate.c_str(), payload.c_str()); 
          */

      }
    }

    return true;
}

bool Env::cuiIsEnabled() {
  return cuiEnabled;
}

void Env::resetTimers(bool minuteTimerOnly) {

  #if defined(PARTIAL_UPDATE_INTERVAL)
    minuteTimer = PARTIAL_UPDATE_INTERVAL;
  #else
    minuteTimer = 60;
  #endif

  if (minuteTimerOnly) return;  
  
  unsigned long now = millis();
  secondTimerStart = now;  
  lastState.sleepTimeCurrent = lastState.sleepTime;
}

// Reset needed on - landscape \ rotation change, display mode change, everything that can move clocks original position

void Env::resetPartialData() {
  Serial.println(F("Reinit partial position"));   
  lastState.lastPartialPos.xEnd = -1;
  lastState.lastPartialPos.xStart = -1;
}

void Env::cuiSetState(bool state, String sname) {

  if (sname.length() <= 0) sname = "default";

  cuiName = sname;
  cuiEnabled = state;
  resetTimers();
}

int Env::cuiGetIdByType(uiWidgetType key) {

  for (size_t i = 0; i < widgetsDefaultsSize; i++)  {
      if (key == widgetsDefaults[i].type) return widgetsDefaults[i].id;
  }

  return 0;
}

uiWidgetType Env::cuiGetTypeById(int key) {
  

  for (size_t i = 0; i < widgetsDefaultsSize; i++)  {
      if (key == (int) widgetsDefaults[i].id) return widgetsDefaults[i].type;
  }

  return uiNone;
/*
  if (key == "uiClock") return uiClock;
  if (key == "uiBat") return uiBat;  
  if (key == "uiBatRemote") return uiBat;
  if (key == "uiTemp") return uiTemp;
  if (key == "uiTempRemote") return uiTempRemote;
  if (key == "uiHum") return uiHum;
  if (key == "uiHumRemote") return uiHumRemote;
  if (key == "uiInfo") return uiInfo;

  return uiNone;
*/
}

bool Env::cuiSetWidget(uiWidgetStyle widget) {

    int key = -1;
    if (widget.type == uiNone) return false;
    for(unsigned int i=0; i < cuiWidgets.size(); i++) {
        if (cuiWidgets[i].type == widget.type) {
          key = i;
          break;
        }
    }

    if (key == -1) {

      if (cuiWidgets.size() > 10) return false;
      cuiWidgets.push_back(widget);

    } else {

      cuiWidgets[key].x = widget.x;
      cuiWidgets[key].y = widget.y;
      cuiWidgets[key].params = widget.params;          
    }

    return true;
}

bool Env::cuiIs4ColorsSupported() {
  
  #if defined(COLORMODE_2BIT_SUPPORT)
      return true;
  #endif

  return false;  
}

void Env::cuiResetWidgets() {
    cuiWidgets.clear();
}

bool Env::collectJSONFieldDataRaw(int paramStart, int len, String & data, String & storage) {

      // paramStart cursor must be at delimiter - ":" at start
      // param:"...."
      paramStart++;

      bool isString = false;
      for (int i = paramStart; i < len; i++) {
               if (data[i] == ' ' || data[i] == '\t' || data[i] == '\n' || data[i] == '\r') continue;
          else if (data[i] == '"') {
            isString = true;
            paramStart = i+1;
            break;  
          } else {
            paramStart = i;
            break;            
          }
      }

      unsigned char c;
      for (int i = paramStart; i < len; i++) {

        c = data[i];
        // Serial.print(c);

          if (isString) {

            if (c == '"') return true;
            if (storage.length() > 128) {
              return true;
            }

          } else {

            if (c == ' ') continue;
            if ((unsigned int) c < 45 || (unsigned int) c > 57) return true; // allowed chars 0123456789.-
            
            if (storage.length() > 16) {
              return true;
            }
          }

          storage += (char) c;
    }

    return true; 
}

bool Env::collectJSONFieldData(String fieldName, String & payload, String & storage) {
    storage = "";
    int len = payload.length();
    int dataPos = payload.indexOf("\"" + fieldName + "\"");

    if (dataPos != -1) {
        dataPos += fieldName.length();
        for (int i = dataPos; i < dataPos + 255; i++) {
            if (i < len-1 && payload[i] == ':') {  
                return collectJSONFieldDataRaw(i, len, payload, storage);
            }
        }
    }

    return false;
}

String Env::sanitizeResponse(String var) {
  
  int responseLength = var.length();
  var = var.substring(0, 128);
  String sanitized = "";
  bool valid = false;

  unsigned char c;

  for (int i = 0; i < responseLength; i++)  {

    c = var[i];
    
    if ((unsigned int) c == 34 || (unsigned int) c == 58 || (unsigned int) c == 47) {
      
      sanitized += "-";
      continue;

    } else {

      valid = false;
      
           if ((unsigned int) c == 32) valid = true; // space
      else if ((unsigned int) c >= 45 || (unsigned int) c <= 57) valid = true; // 0123456789.- 
      else if ((unsigned int) c >= 97 || (unsigned int) c <= 122) valid = true; // a-z
      else if ((unsigned int) c >= 65 || (unsigned int) c <= 90) valid = true;  // A-Z

      if (!valid) continue;
    }


    sanitized += (char) c;
  }

  return sanitized;
}

/*
  reciew data from remote sensor if configured
*/

bool Env::updateExtSensorData() {

    String &url = getConfig()->cfgValues[cExtSensorLink]; 
    String &login = getConfig()->cfgValues[cExtSensorLogin]; 
    String &pass = getConfig()->cfgValues[cExtSensorPassword]; 


    if (url.length() <= 0 || url.equals(F("off"))) {
      Serial.print(F("updateExtSensorData - external sensor not configured or disabled. INPUT : ")); Serial.println(url);
      return false;
    }

    WiFiClient client;
    HTTPClient http;

    externalSensorData newData;
    newData.isDataValid = false;

    http.setTimeout(5000);
    client.setTimeout(5000); // double check for esp32 - can be in seconds, not milisec

    http.begin(client, url);

    if (login.length() > 0) {

      http.setAuthorization(login.c_str(), pass.c_str());

      Serial.println(F("[AUTH] Basic auth mode"));

    } else if (pass.length() > 0) {
        
      http.setAuthorization("");
      String token = "Bearer " + pass;
      http.addHeader("Authorization", token.c_str());

      Serial.println(F("[AUTH] Bearer auth mode"));
    } else {
      
      Serial.println(F("[AUTH] Anonimous mode"));
    }

    Serial.println(F("updateExtSensorData"));

    int httpResponseCode = http.GET();
    String payload = http.getString(); 

    if (httpResponseCode > 0) {      
        String collectedData;
        String resultData = "";

        if (url.indexOf("/api/states") != -1) {
            
          Serial.println(F("Home Assist parser"));

          /*
            "battery": 87,
            "battery_low": false,
            "humidity": 40.85,
            "temperature": 26.32,
            "friendly_name": "Датчик почвы (каланхое спальня) Влага"
          */

          if (!collectJSONFieldData("temperature", payload, collectedData)) {
            
              Serial.print(F("Bad data : ")); Serial.println(payload.substring(0, 255));
              lastError = F("External sensor error : ");
              lastError += sanitizeResponse(payload);

          } else {
              
              resultData += collectedData + " C";
              if (collectJSONFieldData("humidity", payload, collectedData)) {
                resultData += F(", "); resultData += collectedData; resultData += F(" %");
              }

              if (collectJSONFieldData("battery", payload, collectedData)) {
                resultData += F(", "); resultData += collectedData;
              }

              Serial.print(F("[Home Assistant] collected data :")); Serial.println(resultData);  
              validateExtTelemetryData(newData, resultData);
          }

        } else {

          Serial.println(F("Domoticz parser"));

          /*
            "BatteryLevel" : 89,
            "Data" : "34.8 C, 36 %",
            "Name" : "\u0414\u0430\u0442\u0447\u0438\u043a ... \u043a (Temperature + Humidity)",
            "Humidity" : 36.0,
          */

          if (!collectJSONFieldData("Data", payload, collectedData)) {

            Serial.print(F("Bad data : ")); Serial.println(payload.substring(0, 255));
            lastError = "External sensor error : " + sanitizeResponse(payload);

          } else {

            resultData = collectedData;
            if (collectJSONFieldData("BatteryLevel", payload, collectedData)) {
              resultData += F(", "); resultData += collectedData;
            }

            Serial.print(F("[Domoticz] collected data :")); Serial.println(resultData);            
            validateExtTelemetryData(newData, resultData);
          }
        }

    } else {

      if (httpResponseCode == -1) {
         lastError = "External sensor error : Server not accessable";
      } else {
         lastError = "External sensor error : Error code: " + String(httpResponseCode) + ", Response : " + sanitizeResponse(payload);
      }

      Serial.print("Error code: "); Serial.println(httpResponseCode);
      // logReport("cant get ext sensor data " + String(httpResponseCode))
    }

    http.end();
    
    if (newData.isDataValid) {
      lastState.extData = newData;
      lastState.connectTimes++;
      // lastState.syncT = time(nullptr);
    }

    return newData.isDataValid;
}

// collect current internal sensors data to structure
// structure is limited by telemetryBufferMax and rewriten by loop
// lastState.lastTelemetrySize -- current size of lastState.lastTelemetry 

void Env::updateTelemetry()  {
      
    if (lastState.lastTelemetrySize >= telemetryBufferMax) {
        lastState.lastTelemetrySize = 0;
    }
    
    int key = lastState.lastTelemetrySize;
    keepTelemetry(key);

    if (tsensor && 
        lastState.lastTelemetry[key].temperature == 0 && 
        lastState.lastTelemetry[key].humidity == 0
    ) {
      delay(500);
      keepTelemetry(key); // cold start second attempt
    }

    lastState.lastTelemetry[key].bat = readBatteryV();
    lastState.lastTelemetry[key].t = defaultTime;
    lastState.lastTelemetrySize++;

    if (isOnBattery() && getBatteryLvlfromV(lastState.lastTelemetry[key].bat) < 10) {
      lastState.lowBatTick++; 
      Serial.println("[LOW BAT] tick +1 [" + String(lastState.lowBatTick) + "]");
    } else {
      lastState.lowBatTick = 0;
    }

    Serial.println(F("updateTelemetry"));
    Serial.println(lastState.lastTelemetry[key].temperature);
    Serial.println(lastState.lastTelemetry[key].humidity);
    Serial.println(lastState.lastTelemetry[key].bat);
}

float Env::readTemperature()  {
    
    if (tsensor) {
      return tempSensor.readTemperature() + tempOffset;
    } else return -1000;
}

// (1 °C × 9/5) + 32 = 33,8 °F

float Env::toFahrenheit(float celsius) {

    return (float) ((celsius * 1.8f) + 32.0f);
}

float Env::readPressure() {

    return tsensor ? tempSensor.readPressure() : -1000.0f;
}

float Env::readHumidity() {

    return tsensor ? tempSensor.readHumidity() : -1000.0f;
}

bool Env::isOnBattery() {
    // return false;
    if (batteryInit) return lastState.onBattery;

    batteryInit = true;

    #ifdef BAT_ADS1115

      if (!asensor) return false;
      
      float bv = readBatteryV();

      if (bv <= 0.4f) lastState.onBattery = false; // grounded
      else lastState.onBattery = true;

      // if used without ground wire on constant power
      // if (bv <= 3.0f) lastState.onBattery = false; // grounded

    #elif defined(BAT_A0)  

      #if defined(ESP32)

          if (analogRead(BAT_A0) <= 300.0) lastState.onBattery = false; // 0-4095  range, low value -> grounded
          else lastState.onBattery = true;

      #else

          if (analogRead(A0) <= 100.0) lastState.onBattery = false; // 0-1024 range, low value -> grounded
          else lastState.onBattery = true;

      #endif
      
      
      Serial.println("[TEST Battery] Analog pin read : " + String(analogRead(A0)));

    #else

      return false;

    #endif

    return lastState.onBattery;
}

void Env::updateScreen() {

    // if (lastState.partialUpdateTest) {
    //  Serial.println("[TEST PARTIAL MODE]");
    //  screen->drawTestPartial(true);
    //  return;
    // }

    if (!screen->displayBeasy) {

      updateTime();
      
      screen->drawUIToBuffer();
      
      if (!noScreenTest) screen->updateScreen();
      else Serial.println("Skip screen update by noScreenTest var!");

    }    
}

KellyCanvas * Env::getCanvas() {
    return canvas;
}

float Env::readBatteryV() {
    
    #ifdef BAT_ADS1115

      if (!asensor) return -1;
      int16_t adc0 = aInputSensor.readADC_SingleEnded(1); 
 
      // return aInputSensor.computeVolts(adc0);

      // return float(adc0) * 0.1875 / 1000.0;

      
      float R1V = 50.7f;
      float R2GND = 99.26f;
      // Vout = (4.2f * R2GND) / (R1V + R2GND) 
      // float rV = aInputSensor.computeVolts(adc0) * ((R1V + R2GND) / R2GND); // back devider val
      float rV2 = (float(adc0) * 0.1875 / 1000.0) * ((R1V + R2GND) / R2GND); // back devider val
      return rV2;

    #elif defined(BAT_A0)

      #if defined(ESP32)
          // todo - check method 
          float R1V = 50.7f;
          float R2GND = 99.26f;
          int adcValue = analogRead(BAT_A0); 
          float voltage = (adcValue / 4095.0) * 3.3; 
          // возможно соответствие 4095 - 3.3v будет не стабильным т.к. есп не будет делать корректировок, именно для ESP32, не тестировал
          return voltage * ((R1V + R2GND) / R2GND); 

      #else

          // thanks to reference code - https://github.com/SlimeVR/SlimeVR-Tracker-ESP/blob/main/src/batterymonitor.h
          // they optimize good for Wemos D1 Mini

          float BATTERY_SHIELD_RESISTANCE = 180.0;
          float BATTERY_SHIELD_R1 = 100.0;
          float BATTERY_SHIELD_R2 = 220.0;
          float ADCMultiplier = (BATTERY_SHIELD_R1 + BATTERY_SHIELD_R2 + BATTERY_SHIELD_RESISTANCE) / BATTERY_SHIELD_R1;

          float rV = ((float)analogRead(A0)) * 1.0 / 1023.0 * ADCMultiplier;
          Serial.print(F("[TEST voltage : ] ")); Serial.println(rV);
          Serial.print(F("[TEST analog read : ] ")); Serial.println(analogRead(A0));

          return rV;
      #endif  
      

    #else

      return -1;

    #endif
}

/*
float Env::readBatteryVtest() {
  
    int16_t adc0 = aInputSensor.readADC_SingleEnded(1); 
    // return aInputSensor.computeVolts(adc0);

    float R1V = 50.7f;
    float R2GND = 99.26f;
    // Vout = (4.2f * R2GND) / (R1V + R2GND) 
    float rV = aInputSensor.computeVolts(adc0) * ((R1V + R2GND) / R2GND); // back devider val
    float rV2 = (float(adc0) * 0.1875 / 1000.0) * ((R1V + R2GND) / R2GND); // back devider val
    // float voltage;
    // float bat_voltage;
 
    // voltage = adc0 * 0.0001875; // Convert the ADC value to voltage in volts
    // bat_voltage = voltage*(R1V + R2GND) / R1V; // Calculate the actual battery voltage using the voltage divider formula
  

     KellyCanvas * canv = getCanvas();
     canv->clear();
     canv->setRotate(90);
     canv->drawString(10, 10, "V cv : " + String(rV), true);

     canv->drawString(10, 25, "V RAW : " + String(float(adc0) * 0.1875 / 1000.0), true);
     canv->drawString(10, 55, "V alt : " + String(rV2), true);

     rV2 += 0.2f;
     canv->drawString(10, 75, "V percision : " + String(rV2), true);
   //  rV = (float(adc0) * 0.1875 / 1000.0) * ((R1V + R2GND) / R2GND);
   //  canv->drawString(10, 40, "V alt : " + String(rV), true);

   //  int sensorValue = analogRead(A0);
   //  canv->drawString(10, 55, "sensorValue " + String(sensorValue), true); 
   //  canv->drawString(10, 75, "bat_voltage : " + String(bat_voltage), true); 
     
     screen->updateScreen();


    Serial.println("V read : " + String(rV));
    return rV;
    // return float(adc0) * 0.1875 / 1000.0;
}
*/

float Env::getBatteryLvlfromV(float v) {
  
    // float min = 3.0;
    // ~3.25V [ESP8266] - подключен экран + BME280 + ADS1115 не смог выйти из сна в один момент, начинаются перебои в работе
    float min = 3.25;
    float max = 4.2;

    if (v <= min) return 0.0;
    if (v >= max) return 100.0;

    float percent = ((max - min) / 100.0); 
    return (float) ((v - min) / percent);
}

void Env::initSensors() {

    Wire.begin(DEFAULT_I2C_SDA, DEFAULT_I2C_SCL); 
    bool error = false;

    if (!tempSensor.begin(0x76)) {
        error = true;
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    } else {
        tsensor = true;
    }

    #if defined(BAT_ADS1115)

      // aInputSensor.setGain(GAIN_TWOTHIRDS); // ads.setGain(GAIN_TWOTHIRDS); | 2/3х | +/-6.144V | 1bit = 0.1875mV

      if (!aInputSensor.begin()) {
          error = true;
          Serial.println("Could not find a valid ADS1115 sensor, check wiring!");
      } else {
          asensor = true;
      }

    #endif

    if (!error) {
        Serial.println("Sensors [OK]");
    }
}

clockFormatted & Env::getFormattedTime() {
    return fTime;
}

String Env::getFormattedExtSensorLastSyncTime(bool full) {
    if (lastState.extData.isDataValid == false) {
        return "--:--";
    } else {
 
      struct tm resultT;
      localtime_r(&lastState.extData.t, &resultT);

      char buffer[128];

      if (full) {        
          strftime(buffer, sizeof(buffer), "%H:%M %d.%m.%Y", &resultT);
      } else {
          strftime(buffer, sizeof(buffer), "%H:%M %d.%m", &resultT);
      } 
      
      return buffer;
    }
}

void Env::updateTime(time_t dt) {

    struct tm stnow;
    if (dt == 0) {

      fTime.t = time(nullptr); // defaultTime
      localtime_r(&fTime.t, &stnow);
      
    } else {

      fTime.t = dt; 
      localtime_r(&dt, &stnow);
    }

    char buffer[128];
    String tmp = "";
    int tmpN;

    strftime(buffer, sizeof(buffer), "%d.%m.%y", &stnow);
    fTime.date = buffer;

    strftime(buffer, sizeof(buffer), "%d.%m.%Y", &stnow);
    fTime.dateShort = buffer;

    strftime(buffer, sizeof(buffer), "%H:%M:%S", &stnow);
    fTime.timeTextS = buffer;
   
    if (hour12) {
      strftime(buffer, sizeof(buffer), "%I:%M", &stnow);
      fTime.timeText = buffer;
    } else {
      strftime(buffer, sizeof(buffer), "%H:%M", &stnow);
      fTime.timeText = buffer;
    }

    strftime(buffer, sizeof(buffer), "%w", &stnow);
    tmp = buffer;

    sscanf(tmp.c_str(), "%d", &tmpN);

   // Serial.println("DAY");
   // Serial.println(dayN);
   // Serial.println(day);

    switch(tmpN) {
      case 0:
        fTime.dayText = FPSTR(locShortSunday);
        break;
      case 1:
        fTime.dayText = FPSTR(locShortMonday);
        break;
      case 2:
        fTime.dayText = FPSTR(locShortTuesday);
        break;
      case 3:
        fTime.dayText = FPSTR(locShortWednesday);
        break;
      case 4:
        fTime.dayText = FPSTR(locShortThursday);
        break;
      case 5:
        fTime.dayText = FPSTR(locShortFriday);
        break;
      case 6:
        fTime.dayText = FPSTR(locShortSaturday);
        break;
    }

    strftime(buffer, sizeof(buffer), "%m", &stnow);
    tmp = buffer;

    sscanf(tmp.c_str(), "%d", &tmpN );

    fTime.monthN = tmpN;

   // Serial.println("MONTH");
   // Serial.println(month);
   // Serial.println(fTime.dayOfMonth);

    switch(fTime.monthN) {
      case 1:
        fTime.monthText = FPSTR(locMonth1January);
        break;
      case 2:
        fTime.monthText = FPSTR(locMonth2February);
        break;
      case 3:
        fTime.monthText = FPSTR(locMonth3March);
        break;
      case 4:
        fTime.monthText = FPSTR(locMonth4April);
        break;
      case 5:
        fTime.monthText = FPSTR(locMonth5May);
        break;
      case 6:
        fTime.monthText = FPSTR(locMonth6June);
        break;
      case 7:
        fTime.monthText = FPSTR(locMonth7July);
        break;  
      case 8:
        fTime.monthText = FPSTR(locMonth8August);
        break; 
        case 9:
        fTime.monthText = FPSTR(locMonth9September);
        break;
      case 10:
        fTime.monthText = FPSTR(locMonth10October);
        break;  
      case 11:
        fTime.monthText = FPSTR(locMonth11November);
        break; 
      case 12:
        fTime.monthText = FPSTR(locMonth12December);
        break; 
    }

    strftime(buffer, sizeof(buffer), "%d", &stnow);
    tmp = buffer;
    fTime.monthText = tmp + " " + fTime.monthText;

    strftime(buffer, sizeof(buffer), "%H", &stnow);
    tmp = buffer;
    sscanf(tmp.c_str(), "%d", &tmpN);

    fTime.h = tmpN;

    fTime.pm = true;
    if (fTime.h > 0 && fTime.h < 12) fTime.pm = false;

    strftime(buffer, sizeof(buffer), "%M", &stnow);
    tmp = buffer;
    sscanf(tmp.c_str(), "%d", &tmpN);

    fTime.m = tmpN;
}

Config * Env::getConfig() {
    return &cfg;
}

void Env::restart(String reason) {

    Serial.println("[RESTART] Reason : " + reason);
    saveCurrentState();
    delay(500);
    ESP.restart();
}

bool Env::cuiInitFS(bool format) {

#if defined(ESP32)

    if (format) {
        if (cuiFSinited) SPIFFS.end();
        if (SPIFFS.format()) {
            Serial.println("SPIFFS Formatted");
        } else {
            Serial.println("Failed to Format SPIFFS");
        }
        cuiFSinited = false;
    }

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return false;
    }

    cuiFSinited = true;
    return true;

#else 

  if (format) {
    if (cuiFSinited) LittleFS.end();
    LittleFS.format();
    cuiFSinited = false;
  }

  if (cuiFSinited) return true;

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return false;
  }

  if (!LittleFS.exists("/cui")) {
    LittleFS.mkdir("/cui");     
  }

  cuiFSinited = true;
  return true;

#endif


}

String Env::cuiGetFSInfoJSON() {

  cuiInitFS();
      
  #if defined(ESP32)

    String json = "{";
    json += "\"usedBytes\":";
    json += String(SPIFFS.usedBytes());
    json += ',';
    json += "\"totalBytes\":";
    json += String(SPIFFS.totalBytes());
    json += '}';

  #else 

    FSInfo info;
    LittleFS.info(info);
    
    String json = "{";
    json += "\"usedBytes\":";
    json += String(info.usedBytes);
    json += ',';
    json += "\"totalBytes\":";
    json += String(info.totalBytes);
    json += '}';
  #endif

  return json;
}

String Env::cuiGetListFilesJSON() {

  cuiInitFS();
  
#if defined(ESP32)

  File root = SPIFFS.open("/cui");

  if (!root) {
      Serial.println("Failed to open directory /cui");
      return "[]";
  }
  if (!root.isDirectory()) {
      Serial.println("Not a directory /cui");
      return "[]";
  }

  String json = "[";
  String tmp;
  int total = 0;

  File file = root.openNextFile();
  while (file) {
      
      tmp = file.name();
      if (file.isDirectory() || tmp.indexOf(".ini") != -1) {

        file.close();
        file = root.openNextFile();
        
      } else {

        tmp = tmp.substring(0, tmp.lastIndexOf("."));
        if (total > 0) {
            json += ',';
        }
        json += '{';
        json += "\"filename\":";
        json += "\"" + tmp + "\",";

        if (file.size() > 15000) {
            json += "\"bits\":2,";
        } else {
            json += "\"bits\":1,";
        }
        json += "\"size\":";
        json += "\"" + String(file.size()) + "\",";
        json += "\"dt_create\":";
        json += "\"\",";
        json += "\"dt_update\":";
        json += "\"" + String(file.getLastWrite()) + "\"";

        json += '}';
        total++;
        
        file.close();
        file = root.openNextFile();
      }

  }
  json += "]";


#else

  Dir root = LittleFS.openDir("/cui/");
  
  String json = "[";
  String tmp;
  int total = 0;

  while (root.next()) {

    File file = root.openFile("r");
    tmp = root.fileName();
    if (tmp.indexOf(".ini") != -1) continue;

    tmp = tmp.substring(0, tmp.lastIndexOf("."));
    if (total > 0) {
      json += ',';
    }
    json += '{';
    json += "\"filename\":";
    json += "\"" + tmp + "\",";

    if (file.size() > 15000) {
      json += "\"bits\":2,";
    } else {
      json += "\"bits\":1,";
    }
    json += "\"size\":";
    json += "\"" + String(file.size()) + "\","; 
    json += "\"dt_create\":";
    json += "\"" + String(file.getCreationTime()) + "\","; 
    json += "\"dt_update\":";
    json += "\"" + String(file.getLastWrite()) + "\""; 

    json += '}';
    
    file.close();
    total++;
  }

  json += "]";

  #endif

  return json;
}

// only for transition from 1-bit image buffer to 2-bit on ESP8266
// on ESP8266 we cant dinamicly realloc big memory blocks
// return true & sets - env->lastState.cuiFileIndex if we need realloc memory on reboot
bool Env::cuiPrepareRebootIfNeeded() {

#if defined(ESP32)

    return false;

#else

    if (!cuiIsEnabled()) return false;
    if (!canvas->bufferBW) return false;

    cuiInitFS();
    String filename = cuiName + ".bit";
    String tmpFile;
    int index = -1;
    Dir dir = LittleFS.openDir("/cui/");

    while (dir.next()) {
        if (dir.isDirectory()) continue;
        tmpFile = dir.fileName();

        if (tmpFile.indexOf(".bit") == -1) continue;

        index++;
        if (dir.fileName() == filename) break;
    }

    Serial.print(F("[Reboot check] fileindex : "));
    Serial.println(index);

    if (index > -1) {

        File file = LittleFS.open("/cui/" + filename, "r");
        if (!file) {
            Serial.print(F("[Reboot check] not found name : "));
            Serial.println(cuiName);
            return false;
        }

        int fsize = (int) file.size();
        file.close();

        if (fsize > canvas->bufferWidth) {
            
            Serial.print(F("[Reboot check] Reboot needed - fileindex : "));
            Serial.println(index);

            getCurrentState().cuiFileIndex = index;
            saveCurrentState();

            return true;
        } else {

            Serial.print(F("[Reboot check] no reboot "));
            Serial.print(fsize);
            Serial.print(F("/"));
            Serial.println(canvas->bufferWidth);

            return false;
        }

    } else {
        
        Serial.print(F("[Reboot check] no Reboot, not found - fileindex :"));
        Serial.println(index);

        return false;
    }

#endif
}

void Env::cuiResetStateByConfig() {
  
    if (cfg.cfgValues[cImagePreset].length() > 0) {
      
      cuiSetState(true, cfg.cfgValues[cImagePreset]);

    } else if (cuiIsEnabled()) {
      cuiSetState(false);
    }
}

/* 
  loads cui by current loop cursor if enabled. if index is not exist, resets index to begin 
  disables custom ui mode if nothing found
*/
void Env::cuiApplyLoop() {
  if (lastState.cuiLoop) {
    
    Serial.println("[LOOP]" + String(lastState.cuiFileIndex)); 

    lastState.cuiFileIndex = cuiGetNameByIndex(lastState.cuiFileIndex, cuiName);
    if (lastState.cuiFileIndex == -1) {
      lastState.cuiFileIndex = cuiGetNameByIndex(-1, cuiName);
    }

    if (lastState.cuiFileIndex == -1) {
      lastState.cuiLoop = false;
      cuiSetState(false);
      
      Serial.println(F("[Custom UI][LOOP Mode] no data to display, disable mode")); 
    }

    resetPartialData();
    
    // Serial.println("[LOOP end]" + String(lastState.cuiFileIndex) + " file : " + cuiName); 
  }
} 

void Env::cuiLoopTestNext() {
  if (lastState.cuiLoop) {
    lastState.cuiFileIndex++;
    updateScreen();
  }
}

bool Env::cuiWriteStorageFile(bool append, int dataSize) {

  cuiInitFS();
  if (!cuiName) cuiName = "default";

  String filename = "/cui/" + cuiName + ".bit";
   
#if defined(ESP32)
  File file = SPIFFS.open(filename, append ? FILE_APPEND : FILE_WRITE);
#else
  File file = LittleFS.open(filename, append ? "a+" : "w");
#endif
  
  if (!file) {
    Serial.println(F("Failed to open [buffer file] file for writing"));
    return false;
  }

  if (dataSize < 0) dataSize = canvas->bufferWidth;
  if (dataSize > canvas->bufferWidth) dataSize = canvas->bufferWidth;
  
  // unsigned int webSize = 0;
  // String tmp = "";
  for (int i = 0; i < dataSize; i++) {
    // tmp = String((unsigned int) canvas->bufferBW[i]);
    // webSize += tmp.length() + 1; // 255,
    file.write(canvas->bufferBW[i]);
  }

  // webSize--;

  file.close();

  if (append) return true;

  // If we created NEW Custom UI file we also write widget settings to INI file
  // on Append we only write to .bit file additional image buffer bytes
   
#if defined(ESP32)
  file = SPIFFS.open("/cui/" + cuiName + ".ini", FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to open [widgets settings] file for writing"));
    SPIFFS.remove(filename);
    return false;
  }
#else
  file = LittleFS.open("/cui/" + cuiName + ".ini", "w");
  
  if (!file) {
    Serial.println(F("Failed to open [widgets settings] file for writing"));
    LittleFS.remove(filename);
    return false;
  }
#endif

  file.print(String(cuiBits) + ";");
  file.print(getConfig()->getString(cScreenRotate) + ";");
  file.print(getConfig()->getString(cScreenLandscape) + ";");
  // file.print(String(webSize) + ";"); 
  file.print('\n');

  String widgetParams = "";
  for(unsigned int i=0; i < cuiWidgets.size(); i++) {
      if (cuiWidgets[i].enabled && cuiWidgets[i].type != uiNone) {

          widgetParams = cuiGetIdByType(cuiWidgets[i].type);
          widgetParams += ';';
          widgetParams += String(cuiWidgets[i].x);
          widgetParams += ';';
          widgetParams += String(cuiWidgets[i].y);
          widgetParams += ';';
          widgetParams += cuiWidgets[i].params;
          widgetParams += ';';
          widgetParams += '\n';

          file.print(widgetParams);
          Serial.print(F("WRITE : ")); Serial.println(widgetParams);
      }
  }

  // delay(1000);  // CREATE and LASTWRITE times are different
  file.close();
  return true;
}

bool Env::cuiDeleteStorageFile(String name) {
  cuiInitFS();

  #if defined(ESP32)
    SPIFFS.remove("/cui/" + name + ".bit"); 
    SPIFFS.remove("/cui/" + name + ".ini"); 
  #else
    
    LittleFS.remove("/cui/" + name + ".bit"); 
    LittleFS.remove("/cui/" + name + ".ini"); 
  #endif

  return true;
}

/*
int16_t Env::cuiGetIndexByName(String &name) {
  
#if defined(ESP32)

    return -1;

#else

    cuiInitFS();
    String filename = name + ".bit";
    int index = -1;
    String tmpName;
    Dir dir = LittleFS.openDir("/cui/");

    while (dir.next()) {
        if (dir.isDirectory()) continue;

        tmpName = dir.fileName();
        if (tmpName.indexOf(".bit") == -1) continue;

        index++;
        if (dir.fileName() == filename) return index;
    }

    return -1;

#endif

}
*/

/* -1 get first one. return -1 if file with index not found  */
int16_t Env::cuiGetNameByIndex(int16_t searchIndex, String &name) {
    
    cuiInitFS();

    int index = -1;
    String tmpName;
    #if defined(ESP32)
      
    File root = SPIFFS.open("/cui/");
    if (!root) {
        Serial.println(F("Failed to open directory"));
        return index;
    }

    while (true) {
        File file = root.openNextFile();
        if (!file) {
            break;
        }
        if (file.isDirectory()) continue;

        tmpName = file.name();
        if (tmpName.indexOf(".bit") == -1) continue;

        index++;
        if (searchIndex == -1 || searchIndex == index) {
            
            name = tmpName;
            int dotIndex = name.lastIndexOf('.');
            if (dotIndex > -1) {
                name = name.substring(0, dotIndex); 
            }

            break;
        } 
        file.close();
    }
    #else

      Dir dir = LittleFS.openDir("/cui/");
      while (dir.next()) {

          if (dir.isDirectory()) continue;
          tmpName = dir.fileName();
          if (tmpName.indexOf(".bit") == -1) continue;

          index++;
          if (searchIndex == -1 || searchIndex == index) {
            
            name = tmpName;
            int dotIndex = name.lastIndexOf('.');
            if (dotIndex > -1) {
                name = name.substring(0, dotIndex); 
            }

            break;
          } 
      }

    #endif

    return index;
}


/*
  ToDo на ESP8266 нужна плановая перезагрузка при смене битности
  сейчас стоит проверка + рестарт с указанием инструкции к применению оформления, чтобы устройство не ушло memory overflow
*/

bool Env::cuiReadStorageFile(bool widgetsOnly) {

  if (cuiName.length() <= 0) {
    return false;
  }

  cuiInitFS();
  unsigned int bitsPerPixel;
  File file;
  int offset = 0;
 
#if defined(ESP32)
  file = SPIFFS.open("/cui/" + cuiName + ".ini", FILE_READ);
#else
  file = LittleFS.open("/cui/" + cuiName + ".ini", "r");
#endif
  
  if (!file) {
    Serial.println(F("Failed to open [widget ini] for reading"));
    return false;
  }

  uiWidgetStyle newWidget;

  String widgetParam = "";
  unsigned int paramN = 0; // 0 - key, 1 - x offset, 2 - y offset, 3 - params
  int intState = 0;
  bool screenSettings = false;

  // one line usually ~64 charactes long, max num of widets ~10

  cuiResetWidgets();
  while (file.available() && offset < 10000) {

    unsigned char c = (unsigned char) file.read();

    if (c == '\n') {

        if (!screenSettings) {

          screenSettings = true;

        } else {

          if (newWidget.type != uiNone) {
            cuiSetWidget(newWidget);
            // Serial.println("ENABLE widget " + cuiGetIdByType(newWidget.type));
          }

        }        

        newWidget.x = 0;
        newWidget.y = 0;
        newWidget.enabled = true;
        newWidget.type = uiNone;
        newWidget.params = "";

        paramN = 0;
        widgetParam = "";          

    } else if (c == ';') {
        
        // read screen settings
        // [bits per pixel];[width];[height];[flip screen 180];[landscape mode];

        if (!screenSettings) {

            if (!widgetParam) widgetParam = "0";
            if (paramN == 0) {

              bitsPerPixel = 1;
              if(sscanf(widgetParam.c_str(), "%d", &bitsPerPixel) != 1) {
                  bitsPerPixel = 1;
              }

              if (bitsPerPixel <= 0 || bitsPerPixel > 2) bitsPerPixel = 1;

              getCanvas()->setBitsPerPixel(bitsPerPixel);
              getCanvas()->setRotate(0);
              getCanvas()->clear();

              // getCanvas()->setBitsPerPixel(1);
              //cuiBits = bitsPerPixel;

            } else if (paramN == 1) {

                getConfig()->set(cScreenRotate, widgetParam);
                rotate = cfg.getBool(cScreenRotate);
                if (cfg.sanitizeError) rotate = false;

            } else if (paramN == 2) {

              getConfig()->set(cScreenLandscape, widgetParam);
              land = cfg.getBool(cScreenLandscape);
              if (cfg.sanitizeError) land = false;
                      
            } 

        } else {
        
        // read widget settings
        // [widget ID];[X];[Y];[custom text params];

            if (paramN == 0) {
             
                /*
                    // old format optional support 

                    if (widgetParam == "uiClock") newWidget.type = uiClock;
                    if (widgetParam == "uiBat") newWidget.type = uiBat;  
                    if (widgetParam == "uiBatRemote") newWidget.type = uiBatRemote;
                    if (widgetParam == "uiTemp") newWidget.type = uiTemp;
                    if (widgetParam == "uiTempRemote") newWidget.type = uiTempRemote;
                    if (widgetParam == "uiHum") newWidget.type = uiHum;
                    if (widgetParam == "uiHumRemote") newWidget.type = uiHumRemote;
                    if (widgetParam == "uiInfo") newWidget.type = uiInfo;

                    if (newWidget.type != uiNone) {
                      // continue
                    }
                */

                intState = 0;
                if(sscanf(widgetParam.c_str(), "%d", &intState) != 1) {
                    Serial.println("[set to zero] bad widget integer value during file read - " + widgetParam);
                    intState = 0;
                }

                newWidget.type = cuiGetTypeById(intState);

                if (newWidget.type == uiNone) {
                    Serial.println("[skip] unknown widget type during file read - " + widgetParam);
                }

            } else if (paramN == 1 || paramN == 2) {

              intState = 0;
              if(sscanf(widgetParam.c_str(), "%d", &intState) != 1) {
                  Serial.println("[set to zero] bad widget integer value during file read - " + widgetParam);
                  intState = 0;
              }

              if (paramN == 1) {
                  newWidget.x = intState;
              } else {
                  newWidget.y = intState;
              }

            } else if (paramN == 3) {
                newWidget.params = widgetParam;
            } else {
              Serial.println("[skip] too many widget params - " + widgetParam);
            }

        }

      Serial.print(F("READ : ")); Serial.println(widgetParam);
      widgetParam = "";
      paramN++;

    } else {
      widgetParam += (char) c;
    }

    offset++;
  }

  widgetParam = "";
  file.close();

  if (widgetsOnly) {
    Serial.println(F("Skip render"));
    return true;
  }

  offset = 0;
  //Serial.println("---------------------" + String(ESP.getFreeHeap()));

#if defined(ESP32)
  file = SPIFFS.open("/cui/" + cuiName + ".bit", FILE_READ);
#else
  file = LittleFS.open("/cui/" + cuiName + ".bit", "r"); 
#endif

  if (!file) {
    Serial.println(F("Failed to open [display buffer] for reading"));
    return false;
  }
  
  while (file.available() && offset < canvas->bufferWidth) {      
    canvas->bufferBW[offset] = file.read();
    offset++;
  }

  if (offset < canvas->bufferWidth) {
    Serial.println("stored data is less then current screen size " + String(offset) + "/" + String(canvas->bufferWidth));
    return false;
  }

  Serial.print(F("Successfull readed | Buffer size : ")); Serial.print(canvas->bufferWidth);
  Serial.print(F(" | Widgets : ")); Serial.println(cuiWidgets.size());
  //Serial.println("---------------------" + String(ESP.getFreeHeap()));

  file.close();
  return true;
}

// sync config sensetive variables stored in RTC

void Env::applyConfigToRTC() {

    Serial.println(F("[applyConfigToRTC] apply config values to RTC memory")); 

    if (cfg.cfgValues[cSleepTime].length() > 0) {

        lastState.sleepTime = cfg.getInt(cTempOffset) * 60;
        if (cfg.sanitizeError) lastState.sleepTime = 10 * 60;

    } else {
        lastState.sleepTime = 10 * 60;
    }

    lastState.sleepTimeCurrent = lastState.sleepTime;

    lastState.updateMinutes = false;

    #if defined(PARTIAL_UPDATE_SUPPORT)

      if (cfg.cfgValues[cUpdateMinutes].length() > 0) {

        lastState.updateMinutes = cfg.getBool(cUpdateMinutes);

        if (cfg.sanitizeError) {
          lastState.updateMinutes = false;
        }

      } else {
        
        cfg.cfgValues[cUpdateMinutes] = "0";
      }

    #endif

    const char* loopMarker = PSTR("-loop"); 
    lastState.cuiLoop = false;
    lastState.cuiTimeCurrent = 0;
    lastState.cuiFileIndex = -1;

    if (cuiIsEnabled() && strcmp_P(cuiName.c_str(), loopMarker) == 0) {
        lastState.cuiLoop = true;
        lastState.cuiTimeCurrent = CUI_LOOP_INTERVAL;

        Serial.println(F("[Custom UI][Loop mode] enabled"));
    }
}

void Env::validateConfig(unsigned int version, std::vector<cfgOptionKeys> * updatedKeys) {
    
    int tmp = cfg.getInt(cSleepTime);
    if (tmp <= 0 || tmp > 30) {
      cfg.cfgValues[cSleepTime] = "10";
    }

    tmp = cfg.getInt(cSyncEvery);
    if (tmp <= 0 || tmp > 30) {
      cfg.cfgValues[cSyncEvery] = "6";
    }
    
    if (cfg.cfgValues[cWifiNetwork].length() <= 0) {
        cfg.cfgValues[cWifiNetwork] = FPSTR(cfgWifiSSID);
        cfg.cfgValues[cWifiPassword] = FPSTR(cfgWifiP);
    }

    if (cfg.cfgValues[cExtSensorLink].length() <= 0) {
        cfg.cfgValues[cExtSensorLink] = FPSTR(cfgExtSensor);
        cfg.cfgValues[cExtSensorLogin] = FPSTR(cfgExtSensorL);
        cfg.cfgValues[cExtSensorPassword] = FPSTR(cfgExtSensorP);
    }

    if (cfg.cfgValues[cMqttHost].length() <= 0) {
        cfg.cfgValues[cMqttHost] = FPSTR(cfgMqttHost);
        cfg.cfgValues[cMqttLogin] = FPSTR(cfgMqttLogin);
        cfg.cfgValues[cMqttPassword] = FPSTR(cfgMqttPassword);
        cfg.cfgValues[cMqttHAPrefix] = FPSTR(cfgMqttHAPrefixDiscovery);
        cfg.cfgValues[cMqttHADevice] = FPSTR(cfgMqttHADevice);
        cfg.cfgValues[cMqttPrefixIn] = FPSTR(cfgMqttDomoticzPrefix);
        cfg.cfgValues[cMqttDevicesIds] = FPSTR(cfgMqttDomoticzDeviceIds);
    }

    cuiResetStateByConfig();
    rawMqttIds.clear();

    if (cfg.cfgValues[cMqttHost].length() > 0) {
      
      if (cfg.cfgValues[cMqttHADevice].length() > 0) {           

        cfg.getStringList(cfg.cfgValues[cMqttHADevice], rawMqttIds, ',', 2);
        if (rawMqttIds.size() <= 0 || rawMqttIds[0].length() <= 2) rawMqttIds.push_back("kelly_eink");
        if (rawMqttIds.size() <= 1 || rawMqttIds[1].length() <= 2) rawMqttIds.push_back("KellyC EINK");
        rawMqttIds[0].trim();
        rawMqttIds[1].trim();

      } else if (cfg.cfgValues[cMqttDevicesIds].length() > 0) {
          cfg.getStringList(cfg.cfgValues[cMqttDevicesIds], rawMqttIds, ',', 3);
      }
    }

    if (cfg.cfgValues[cMqttPort].length() <= 0) {
        cfg.cfgValues[cMqttPort] = "1883";
    }

    if (cfg.cfgValues[cTimezone].length() <= 0) {
        cfg.cfgValues[cTimezone] = FPSTR(cdTimezone);
    }  

    if (cfg.cfgValues[cNtpHosts].length() <= 0) {
       cfg.cfgValues[cNtpHosts] = FPSTR(cdNtpHosts);
    } 

    if (cfg.cfgValues[cTempOffset].length() > 0) {

        tempOffset = cfg.getFloat(cTempOffset);
        if (cfg.sanitizeError) tempOffset = 0.0;

    } else {
        tempOffset = 0.0;
    }

    if (cfg.cfgValues[cSyncEvery].length() > 0) {

        syncEvery = cfg.getInt(cSyncEvery);
        if (cfg.sanitizeError) syncEvery = 6;

    } else {
        syncEvery = 6;
    }

    if (cfg.cfgValues[cToFahrenheit].length() > 0) {

        celsius = !cfg.getBool(cToFahrenheit);
        if (cfg.sanitizeError) celsius = true;

    } else {
        celsius = true;
    }

    if (cfg.cfgValues[cTimeFormat12].length() > 0) {

        hour12 = cfg.getBool(cTimeFormat12);
        if (cfg.sanitizeError) hour12 = false;

    } else {
        hour12 = false;
    }

    if (cfg.cfgValues[cScreenLandscape].length() > 0) {

        land = cfg.getBool(cScreenLandscape);
        if (cfg.sanitizeError) land = false;

    } else {
        land = false;
    }

    if (cfg.cfgValues[cScreenRotate].length() > 0) {

        rotate = cfg.getBool(cScreenRotate);
        if (cfg.sanitizeError) rotate = false;

    } else {
        rotate = false;
    }
    
    // addition post-work if update keys

    if (updatedKeys != NULL) {

        cfgOptionKeys key;
        for (size_t b = 0; b < updatedKeys->size(); b++) {
            key = updatedKeys->at(b);

            if (
              key == cMqttHost || key == cMqttPort || key == cMqttLogin || key == cMqttPassword || // need reconnect
              key == cMqttHAPrefix || key == cMqttHADevice // need reinit publish HA config values for new devices
            ) {

                if (_mqttClient.connected()) _mqttClient.disconnect(); 
                mqtt = false;
                mqttSuccess = false;

                Serial.print(F("[Validate config] Reset MQTT connection"));         
                break;
            } else if (key == cExtSensorLink || key == cExtSensorLogin || key == cExtSensorPassword) {

                lastState.connectTimes = 0;
                lastState.wakeUps = 0;

            } else if (key == cScreenLandscape || key == cScreenRotate) {
                resetPartialData();
            }
        }
    }

    applyConfigToRTC();
}

bool Env::commitConfig() {

    cfg.commitEEPROM();
    return true;
}

bool Env::resetConfig(bool resetWifi) {

    String wifiNetwork = cfg.getString(cWifiNetwork);
    String wifiPass = cfg.getString(cWifiPassword);

    cfg.reset();

    if (!resetWifi) {
        cfg.set(cWifiNetwork, wifiNetwork);
        cfg.set(cWifiPassword, wifiPass);
    }

    validateConfig();
    commitConfig();

    return true;
}