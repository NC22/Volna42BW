#include "Env.h"



Env::Env() {

    lastState.sleepTimeCurrent = 1000;
    lastState.onBattery = false;
    partialUpdateRequired = false;
    
    secondTimerStart = millis();
    cuiWidgets.clear();
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

  #if !defined(SAFE_MODE) || SAFE_MODE == false
    validateConfig(getConfig()->loadEEPROM());
  #else     
      Serial.println(F("[CONFIG] IGNORED -> Run in SAFE MODE"));   
  #endif

  resetTimers(true);
  initSensors();

  #if defined(ESP32)
      esp_reset_reason_t reason = esp_reset_reason();
  #else

     int reason = -1;
     ESP.getResetReason();
     reason = resetInfo.reason;
    
    // Фикшеный метод сна выводит модуль из сна с причиной REASON_EXT_SYS_RST что нарушает общую логику. Мы можем перед началом основной логики инициализации
    // проверять сохранилась ли RTC память и если она корректна, то считать что мы вышли из сна, а не просто только что включили устройство
    #if defined(FIX_DEEPSLEEP)
     if (FIX_DEEPSLEEP > 0 && reason == REASON_EXT_SYS_RST && restoreRTCmem()) {
        reason = REASON_DEEP_SLEEP_AWAKE;
        Serial.println(F("[FIX_DEEPSLEEP] WakeUp STATE restored by RTC memory")); 
     }
    #endif

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
            
              int partialUpdateTime = getPartialSleepTime();
              lastState.sleepTimeCurrent -= partialUpdateTime;
              lastState.t += partialUpdateTime;
              defaultTime = lastState.t;

              if (lastState.sleepTimeCurrent > 0) {
                partialUpdateRequired = true;
              } else {
                partialUpdateRequired = false;
                lastState.sleepTimeCurrent = lastState.sleepTime;
              }
              
              Serial.print(F("PARTIAL_UPDATE : ")); 
              Serial.print(lastState.sleepTimeCurrent); 
              Serial.print(F(" / ")); Serial.println(lastState.sleepTime);

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
              lastState.cuiTimeCurrent = CUI_LOOP_INTERVAL;
              cuiLoopNext();
          } 
        }

        Serial.print(F("[Boot] : WakeUps : ")); Serial.print(lastState.wakeUps);
        Serial.print(F(" FullScreenUpdate sec : ")); Serial.print(lastState.sleepTimeCurrent);
        Serial.print(F(" | time : ")); Serial.println(lastState.t);
      }

    } else {

      // Софтверный рестарт (например через web интерфейс)

      if ( reason == REASON_SOFT_RESTART ) {

        Serial.println(F("[Boot] Software reboot -> reset stats & check cui index"));
        bool rtcMemPass = restoreRTCmem();

        // ToDo move to separate method - load next cui item by index from deepsleep (loop mode)
        // show something after reboot (for web ui actions without permanent save to flash)
        if (rtcMemPass && lastState.cuiFileIndex > -1) {

          String restoredName = "";
          cuiGetNameByIndex(lastState.cuiFileIndex, restoredName);
          
          if (restoredName.length() > 0) {

              Serial.print(F("[Direct input] Restore CUI file by index - fileindex : ")); Serial.println(lastState.cuiFileIndex);
              Serial.println(restoredName);

              cuiSetState(true, restoredName);

          } else {
            
            Serial.print(F("[Direct input] Restore CUI file by index - file NOT found by index : ")); Serial.println(lastState.cuiFileIndex);
            cuiSetState(false);
          }

          if (lastState.cuiResetOnReboot) {
            lastState.cuiResetOnReboot = false;
            lastState.cuiFileIndex = -1;
          }
        }

        
        if (rtcMemPass) {       
          
          // reset stats, except default time

          defaultTime = lastState.t;
          setDefaultLastStateData();  
           
          lastState.timeConfigured = true;
        }

      // чистый запуск
      } else {
        
        setDefaultLastStateData();
        Serial.println(F("[Boot] Device reseted -> reset stats"));
      }
      
    }

}

bool Env::restoreRTCmem() {

    if (RTCMemInit > 0) return RTCMemInit == 1 ? true : false;
    RTCMemInit = 1;

#if defined(ESP32)

    bool restoreResult = false;
    
    #if !defined(DEEPSLEEP_MEMORY) || DEEPSLEEP_MEMORY == 1
        restoreResult = readRTCUserMemoryActualRTC(lastState);        
    #else
        restoreResult = readRTCUserMemoryNVS((uint32_t*) &lastState, sizeof(lastState));
    #endif

    if (!restoreResult) {
        lastState.cfgVersion = 0;
        lastState.t = 0;
    }
    
#else

    if (!ESP.rtcUserMemoryRead(0, (uint32_t*) &lastState, sizeof(lastState))) {
        lastState.cfgVersion = 0;
        lastState.t = 0;
    }

#endif

    if (lastState.t < 100000 || lastState.cfgVersion != cdConfigVersion) {

      Serial.println(F("[restoreRTCmem] Bad time or Memory Version mismatch -> reset")); 

      setDefaultLastStateData();
      applyConfigToRTC();
      RTCMemInit = 2;

      return false;
    } else return true;
}

void Env::setDefaultLastStateData() {

    Serial.println(F("[Boot] Reset all stats"));

    /*
      default states for :

      -- sleepTime
      -- sleepTimeCurrent
      -- updateMinutes validated during config loading
      -- cuiFileIndex
      -- cuiLoop
      -- cuiTimeCurrent

      applyed during config validation by applyConfigToRTC or on RTC read fail when wake up in restoreRTCmem
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

/*
    Init default time

    Priority 

    1. defaultTime variable if RTC memory restored
    2. timestamp from user config
*/
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
        // Serial.println(defaultTime);
      }

    } else {

      setenv("TZ", cfg.cfgValues[cTimezone].c_str(), 1);
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
        lastState.cuiTimeCurrent = CUI_LOOP_INTERVAL;
        cuiLoopNext();
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
        
        updateExtIconState();
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
    // - wakeups counter reaches "syncEvery" config value (default 6) 
    // [removed] telemetry cache size is overflow - lastState.lastTelemetrySize >= telemetryBufferMax

    if (lastState.wakeUps == 0 || lastState.wakeUps >= syncEvery || lastState.t < 1000000000) {        
      return true;
    } else {
      return false;
    } 
}

void Env::restartNTP(bool resetOnly) {
    
    ntp = false;
    lastError = "";

    setenv("TZ", cfg.cfgValues[cTimezone].c_str(), 1);
    tzset();

    timeval tv = { 0, 0 };
    settimeofday(&tv, nullptr);

    yield();
    if (!resetOnly) setupNTP();
}

bool Env::setupNTP(unsigned int attempt) {

    if (ntp) return true;

    if (getConfig()->cfgValues[cNtpHosts].equals(F("off")) || getConfig()->cfgValues[cNtpHosts].length() <= 0) {

      Serial.println(F("No NTP servers specified, setting time by default timestamp"));
    
      setenv("TZ", "GMT0", 1);
      tzset();

      initDefaultTime();
      return false;
    }

    bool skipWait = false;

    #if defined(DEFAULT_TIME_BY_EXTERAL)

    if (getConfig()->cfgValues[cExtSensorLink].length() > 0 && getConfig()->cfgValues[cExtSensorLink].indexOf(F("/api/states")) != -1) {
  
      Serial.println(F("[HA] Get default time"));
      WiFiClient client;
      HTTPClient http;

      String url = getConfig()->cfgValues[cExtSensorLink].substring(0, getConfig()->cfgValues[cExtSensorLink].indexOf(F("/api/states")));
             url += "/api/template";

      http.begin(client, url);

      if (getConfig()->cfgValues[cExtSensorPassword].length() > 0) {
          
        http.setAuthorization("");
        String token = "Bearer " + getConfig()->cfgValues[cExtSensorPassword];
        http.addHeader("Authorization", token.c_str());
      }

      http.addHeader("Content-Type", "application/json");  

      if (http.POST(F("{\"template\": \"{{ now().strftime('%Y-%m-%d %H:%M:%S') }}\"}")) == 200) {

        lastState.timeConfigured = false;
        getConfig()->cfgValues[cTimestamp] = http.getString();
        setenv("TZ", cfg.cfgValues[cTimezone].c_str(), 1);
        tzset();
        initDefaultTime();

        skipWait = true;
        
      } else {
        lastError = "HA default time - bad response";
      }

      http.end();
    }

    #endif

    ntp = true;

    String &ntpHosts = getConfig()->cfgValues[cNtpHosts];
    if (ntpHosts.indexOf(',') != -1) {
      
      Serial.print(F("NTP list parsing..."));
      char ntpServers[3][64]; 
      unsigned int nSize = ntpHosts.length();
      uint8_t serverCount = 0; 
      int16_t ntpServersCursor = 0; 

      for (unsigned int i = 0; i < nSize; i++) { 
          if (ntpHosts[i] == ',' || i == nSize-1) {
            ntpServers[serverCount][ntpServersCursor] = '\0'; 
            serverCount++;
            ntpServersCursor = 0;
            if (serverCount >= 3) break;
          } else {            
            if (ntpServersCursor < 63 && ntpHosts[i] != ' ') {
              ntpServers[serverCount][ntpServersCursor] = ntpHosts[i];
              ntpServersCursor++;
            }
          } 
      }

      if (serverCount == 1) {
          CONFIG_TIME_FUNCTION(cfg.cfgValues[cTimezone].c_str(), ntpServers[0]);  
          Serial.println(ntpServers[0]);
          Serial.println(F("[1] server added"));
      } else if (serverCount == 2) {
          CONFIG_TIME_FUNCTION(cfg.cfgValues[cTimezone].c_str(), ntpServers[0], ntpServers[1]); 
          Serial.println(ntpServers[0]);Serial.println(ntpServers[1]);
          Serial.println(F("[2] servers added"));
      } else if (serverCount == 3) {
          CONFIG_TIME_FUNCTION(cfg.cfgValues[cTimezone].c_str(), ntpServers[0], ntpServers[1], ntpServers[2]);      
          Serial.println(ntpServers[0]);Serial.println(ntpServers[1]);Serial.println(ntpServers[2]);     
          Serial.println(F("[3] servers added"));
      }

    } else {
      CONFIG_TIME_FUNCTION(cfg.cfgValues[cTimezone].c_str(), ntpHosts.c_str());  
    }

    yield();

    if (lastState.timeConfigured) {
      Serial.print(F("Waiting for NTP time sync: [RTC time already exist -> reduce wait time, no retry]"));
    } else {
      Serial.print(F("Waiting for NTP time sync: "));
    }

    int i = 0;
    time_t now = time(nullptr);
    
    // timeout = 25sec, second try 10sec
    
    if (!skipWait) {
      
      while (now < 1000000000) {
          
        now = time(nullptr);
        i++;
        
        if (((attempt > 1 || lastState.timeConfigured) && i > 20) || i > 50) {

          Serial.println(F("Time sync failed!"));
          
          #if defined(NTP_CONNECT_ATTEMPTS)

            if (!lastState.timeConfigured) {

              if (attempt < NTP_CONNECT_ATTEMPTS) {
                  restartNTP(true);
                  Serial.println(F("Time sync - reattempt!"));
                  return setupNTP(attempt + 1);
              }

            }

          #endif

          lastError = "NTP server is not accessable. Default time setted (by config or firmware variable)";


          initDefaultTime();

          return false;
        };
        
        Serial.print(".");
        delay(500);
      }

    }
    
    Serial.println(F("NTP ready!"));
    defaultTime = time(nullptr);
    
    lastState.t = defaultTime;
    lastState.timeConfigured = true;

    // lastState.syncT = defaultTime;
    
    return true;
}

// Reset WakeUp counter
// Optionaly we can reset telemetry pool if we have some sync methods for send data to remote server (currently used sync via MQTT)
void Env::sync() {

    lastState.wakeUps = 0;

/*
    if (lastState.lastTelemetrySize <= 0) {
      updateTelemetry();
    }

    if (lastState.lastTelemetrySize > 0) {
        lastState.lastTelemetry[0] = lastState.lastTelemetry[lastState.lastTelemetrySize-1];
        lastState.lastTelemetrySize = 1;
    }
*/
}

rtcData & Env::getCurrentState() {  
   return lastState;
}

void Env::saveCurrentState()  {    

#if defined(ESP32)

  #if !defined(DEEPSLEEP_MEMORY) || DEEPSLEEP_MEMORY == 1
      writeRTCUserMemoryActualRTC(lastState);
  #else
      writeRTCUserMemoryNVS((uint32_t*)&lastState, sizeof(lastState));
  #endif

#else
    ESP.rtcUserMemoryWrite (0, (uint32_t*) &lastState, sizeof(lastState));
#endif
}

void Env::sleep()  {
    
    lastState.t = time(nullptr);
    saveCurrentState();

    #if defined(ESP8266) && defined(FIX_DEEPSLEEP) && FIX_DEEPSLEEP > 0

        Serial.print(F("[Deep sleep] FIX_DEEPSLEEP "));  

        if (lastState.updateMinutes) {
          int partialSleepTime = getPartialSleepTime();
          Serial.println(partialSleepTime); 
          delay(300); // may be can be removed, just serial output will be broken
          nkDeepsleep(partialSleepTime * 1000000, FIX_DEEPSLEEP);
        } else {
          Serial.println(lastState.sleepTime); 
          delay(300);
          nkDeepsleep(lastState.sleepTime * 1000000, FIX_DEEPSLEEP);
        }

    #else

      Serial.print(F("[Deep sleep] "));  

      if (lastState.updateMinutes) {
        int partialSleepTime = getPartialSleepTime();
        Serial.println(partialSleepTime); 
        ESP.deepSleep(partialSleepTime * 1000000);
      } else {
        Serial.println(lastState.sleepTime); 
        ESP.deepSleep(lastState.sleepTime * 1000000);
      }
    #endif
  }

void Env::keepTelemetry(int &key)  {
        
    lastState.lastTelemetry[key].temperature = readTemperature();
    lastState.lastTelemetry[key].pressure = readPressure();
    lastState.lastTelemetry[key].humidity = readHumidity();
}

bool Env::isSleepRequired() {
        
    #if defined(SLEEP_ALWAYS_SLEEP)
      return true;
    #elif defined(BAT_NO) || defined(SLEEP_ALWAYS_IGNORE)
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
      String comm;

      Serial.println(rawMqttIds[0]); 
      Serial.println(rawMqttIds[1]); 

      String tstate = "home/" + rawMqttIds[0];

      tmp = "{\"device_class\":\"temperature\",\"name\":\"temperature\",\"state_topic\":\"" + tstate + "/info\",\"unit_of_measurement\":\"°C\",\"value_template\":\"{{value_json.temperature}}\",\"unique_id\":\"" + rawMqttIds[0] + "_temperature\",\"device\":{\"identifiers\":[\"" + rawMqttIds[0] + "_device\"],\"name\":\"" + rawMqttIds[1] + "\",\"model\":\"KellyC42ESP8266\",\"manufacturer\":\"Nradiowave\"}}";
      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/temperature/config");
      // Serial.println(comm); 
      // Serial.println(tmp); 

      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - temperature info")); 
        lastError = F("Error in HA MQTT initialization. Possible small buffer size, check - setBufferSize");
      }

      tmp.replace(F("temperature"), F("pressure"));
      tmp.replace(F("°C"), F("hPa"));

      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/pressure/config");
      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - pressure info")); 
      }

      #if defined(CO2_SCD41) 
      
      String tmp2 = tmp;
      tmp2.replace(F("\"name\":\"pressure\""), F("\"name\":\"CO2\""));
      tmp2.replace(F("pressure"), F("carbon_dioxide"));
      tmp2.replace(F("hPa"), F("ppm"));
      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/carbon_dioxide/config");   

      if (!_mqttClient.publish(comm.c_str(), tmp2.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - carbon_dioxide info")); 
      }
      tmp2 = "";
      // tmp.replace(F("carbon_dioxide"), F("pressure"));
      // tmp.replace(F("ppm"), F("hPa"));
      #endif      

      tmp.replace(F("pressure"), F("humidity"));
      tmp.replace(F("hPa"), F("%"));

      comm = cfg.cfgValues[cMqttHAPrefix] + String("/sensor/" + rawMqttIds[0] + "/humidity/config");      
      // Serial.println(comm); 
      // Serial.println(tmp); 

      if (!_mqttClient.publish(comm.c_str(), tmp.c_str(), true)) {
        Serial.println(F("[mqttHAAutodetectionInit] Fail send - humidity info")); 
      }

      tmp.replace(F("humidity"), F("battery"));
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
        
        Serial.print(F("[Domoticz]...")); 
        
        for (int i = 3; i >= 0; i--) {
          
          if (i+1 > (int) rawMqttIds.size()) {
            testMqttId = -1;
          } else {
            testMqttId = cfg.sanitizeInt(rawMqttIds[i]);
            if (cfg.sanitizeError) testMqttId = -1;
          }
          
               if (i == 2) metric = String(readTemperature());  // temp
          else if (i == 1) metric += ";" + String(h) + ";" + String(hStat); // temp & hum
          else if (i == 0) metric += ";" + String((float) (readPressure() / 100.0)) + ";0"; // temp & hum & bar
          else if (i == 3) { // air quality
            #if defined(CO2_SCD41) 
              updateSCD4X();
              metric += String(scd4XCO2);
            #else 
              metric += "0";
              testMqttId = -1;
            #endif
          }

          if (testMqttId <= 0) {
              
              Serial.println(F("Bad or unsetted Domoticz device id --> skip send")); 
              Serial.println(i); Serial.println(testMqttId); 
              continue;

          } else {

              // air quality - reads from nvalue
              if (i == 3) {
                tmp = "{\"idx\":" + String(testMqttId) + ",\"nvalue\":" + metric +",\"svalue\":\"" + metric + "\"}";
              } else {
                tmp = "{\"idx\":" + String(testMqttId) + ",\"nvalue\":0,\"svalue\":\"" + metric + "\"}";
              }

              // Serial.println(tmp); 
              _mqttClient.publish(cfg.cfgValues[cMqttPrefixIn].c_str(), tmp.c_str());
              mqttSuccess = true;
          }
        }


      // Home assistant

      } else {
        
        String tstate; String payload;
        
        Serial.print(F("[Home Assistant]...")); 
        tstate = "home/" + rawMqttIds[0] + "/info";

        payload = "{\"temperature\":" + String(readTemperature()) + ",\"humidity\":" + String(readHumidity()) + ",\"pressure\":" + String((float) (readPressure() / 100.0));

        if (isOnBattery()) {
          if (lastState.lastTelemetrySize > 0) {
              payload += ",\"battery\":" + String((int) round(getBatteryLvlfromV(lastState.lastTelemetrySize)));
          } else payload += ",\"battery\":100";
        } else payload += ",\"battery\":100";

        #if defined(CO2_SCD41) 
          updateSCD4X();
          payload += ",\"carbon_dioxide\":" + String(scd4XCO2);
        #endif 

        payload += "}";

        // Serial.println(tstate); 
        // Serial.println(payload); 

        if (!_mqttClient.publish(tstate.c_str(), payload.c_str())) {
           Serial.println("FAIL to publish INFO via MQTT");
        } else {
          mqttSuccess = true;            
          Serial.println("OK");
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
  
  minuteTimer = getPartialSleepTime();
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

      if (cuiWidgets.size() > CUI_MAX_WIDGETS) return false;
      cuiWidgets.push_back(widget);

    } else {

      cuiWidgets[key].x = widget.x;
      cuiWidgets[key].y = widget.y;
      cuiWidgets[key].params = widget.params;          
    }

    return true;
}

void Env::cuiResetWidgets() {
    cuiWidgets.clear();
}

void Env::updateExtIconState() {

    // опционально наследуем данные по давлению с внутреннего датчика если данные по давлению не доступны
    #if defined(DUI_PRESSURE_COPY_TO_EXT)
      if (lastState.extData.isDataValid && lastState.extData.pressure <= BAD_SENSOR_DATA) {
          lastState.extData.pressure = lastState.lastTelemetry[lastState.lastTelemetrySize-1].pressure;
      }
    #endif

    if (lastState.extData.icon != kowUnknown) {

      // already detected some accurate state
        
    } else {
      
      if (lastState.extData.isDataValid && lastState.lastTelemetrySize > 0) {
          
        #if defined(ICON_RAIN_DETECT)
              
          float priorityPressure = lastState.extData.pressure;
          if (priorityPressure <= BAD_SENSOR_DATA) {
              priorityPressure = lastState.lastTelemetry[lastState.lastTelemetrySize-1].pressure;
          } 

          if (priorityPressure <= BAD_SENSOR_DATA) { 

            // skip icon update, no pressure data
            Serial.print(F("priorityPressure skip icon update, no pressure data"));

          } else {
            
            if (lastState.extData.temperature > 0) {

              #if defined(ICON_RAIN_DETECT_RAINY_HPA)   
              if ((priorityPressure / 100.0f) <= ICON_RAIN_DETECT_RAINY_HPA ) {

                  if (lastState.extData.humidity >= ICON_RAIN_DETECT_RAINY_HUM) {
                    lastState.extData.icon = kowRain;
                    return;
                  }

              }
              #endif
              
              #if defined(ICON_RAIN_DETECT_CLOUDY_HPA)   
              if ((priorityPressure / 100.0f) <= ICON_RAIN_DETECT_CLOUDY_HPA ) {

                  if (lastState.extData.humidity >= ICON_RAIN_DETECT_CLOUDY_HUM) {
                    lastState.extData.icon = kowFewClouds;
                    return;
                  }
              }
              #endif
              
            } else {

              #if defined(ICON_SNOW_DETECT_SNOW_HPA)              
              if ((priorityPressure / 100.0f) <= ICON_SNOW_DETECT_SNOW_HPA ) {

                  if (lastState.extData.humidity >= ICON_SNOW_DETECT_SNOW_HUM) {
                    lastState.extData.icon = kowSnow;
                    return;
                  }

              }
              #endif
                
              #if defined(ICON_SNOW_DETECT_CLOUDY_HPA)
              if ((priorityPressure / 100.0f) <= ICON_SNOW_DETECT_CLOUDY_HPA ) {

                  if (lastState.extData.humidity >= ICON_SNOW_DETECT_CLOUDY_HUM) {
                    lastState.extData.icon = kowFewClouds;
                    return;
                  }
              }
              #endif
            }
          }
        #endif

      }
    }
}

/*
  Request data from remote sensor if configured
*/
bool Env::updateExtSensorData() {

  #if defined(ENV_INDOOR_EXTERNAL_SUPPORT) 
    if (pgm_read_byte(&cfgExtLocal) > 0) {
      
      Serial.print(F("Load External sensor data for INDOOR"));
      
      String url = FPSTR(cfgExtSensorLocal);
      String login = FPSTR(cfgExtSensorLocalL);
      String pass = FPSTR(cfgExtSensorLocalP);

      externalSensorData newData;
      if (ExternalSensor::requestData(
        url, 
        login, 
        pass, 
        newData, 
        lastError
      )) {

        lastState.lastTelemetrySize = 1;
        int index = 0;
        updateBattery(index);
        lastState.lastTelemetry[index].t = defaultTime;
        lastState.lastTelemetry[index].humidity = newData.humidity;        
        lastState.lastTelemetry[index].pressure = newData.pressure;        
        lastState.lastTelemetry[index].temperature = newData.temperature;
      }
    }
  #endif

    if (getConfig()->cfgValues[cExtSensorLink].length() < 0) return false;

    if (ExternalSensor::requestData(
      getConfig()->cfgValues[cExtSensorLink], 
      getConfig()->cfgValues[cExtSensorLogin], 
      getConfig()->cfgValues[cExtSensorPassword], 
      lastState.extData, 
      lastError
    )) {
      lastState.connectTimes++;
      return true;
    } else {
      return false;
    }
}

void Env::updateBattery(int &telemetryIndex) {
  
  lastState.lastTelemetry[telemetryIndex].bat = readBatteryV();

  if (isOnBattery() && getBatteryLvlfromV(lastState.lastTelemetry[telemetryIndex].bat) < 10) {

    lastState.lowBatTick++;      
    Serial.println("[LOW BAT] tick +1 [" + String(lastState.lowBatTick) + "]");
      
  } else {

    lastState.lowBatTick = 0;
  }
}

// collect current internal sensors data to structure
// structure is limited by telemetryBufferMax and rewriten by loop
// lastState.lastTelemetrySize -- current size of lastState.lastTelemetry 

void Env::updateTelemetry()  {

  #if defined(CO2_SCD41) 
    if (updateSCD4X()) {
        Serial.print(F("[SCD4X] CO2: "));
        Serial.println(scd4XCO2);
        Serial.print(F("[SCD4X] Temperature: "));
        Serial.println(scd4XTemp);
        Serial.print(F("[SCD4X] Humidity: "));
        Serial.println(scd4XHumidity);
    }
  #endif

  #if defined(ENV_INDOOR_EXTERNAL_SUPPORT) 
    if (pgm_read_byte(&cfgExtLocal) > 0) {      
      Serial.println(F("[updateTelemetry] - Internal sensor data will be taken from External source - skip"));
      return;
    }
  #endif

  lastState.lastTelemetrySize++;

  if (lastState.lastTelemetrySize > ENV_TELEMETRY_MAX) {
      lastState.lastTelemetrySize = 1; // reinit pool with set size to 1 element (current new one)
  }

  int key = lastState.lastTelemetrySize-1;
  keepTelemetry(key);

  if (tsensor && 
      lastState.lastTelemetry[key].temperature == 0 && 
      lastState.lastTelemetry[key].humidity == 0
  ) {
    delay(500);
    keepTelemetry(key); // fail, cold start second attempt
  }

  updateBattery(key);
  lastState.lastTelemetry[key].t = defaultTime;

  Serial.println(F("[updateTelemetry] - Internal sensors data : "));
  Serial.println(lastState.lastTelemetry[key].temperature);
  Serial.println(lastState.lastTelemetry[key].humidity);
  Serial.println(lastState.lastTelemetry[key].pressure);
  Serial.println(lastState.lastTelemetry[key].bat);
}

bool Env::updateSCD4X() {

  #if !defined(CO2_SCD41) 

      return false;

  #else

    // sensor api return errors when read faster -> return cached value
    if (scd4XLastRead > 0 && millis() - scd4XLastRead < 1000) {  
        Serial.println(F("[updateSCD4X] Reading delay. Prevent read faster then once per second"));
        return (scd4XCO2 > 0) ? true : false;
    }

    scd4XLastRead = millis();
    uint16_t error; 
    char errorMessage[256];
    error = scd4x.readMeasurement(scd4XCO2, scd4XTemp, scd4XHumidity);
    if (error) {

        Serial.println(F("[updateSCD4X] Error trying to execute readMeasurement(): "));
        errorToString(error, errorMessage, 256);

        Serial.println(errorMessage);
        scd4XerrorTick++;

        if (scd4XerrorTick > 3) {
          scd4XCO2 = 0;
        } 

        return (scd4XCO2 > 0) ? true : false;

    } else if (scd4XCO2 == 0) {

        Serial.println(F("[updateSCD4X] Invalid sample detected, skipping."));
        scd4XerrorTick++;

        if (scd4XerrorTick > 3) {
          scd4XCO2 = 0;
        }

        return (scd4XCO2 > 0) ? true : false;

    } else {

        scd4XerrorTick = 0;
        return true;
    }
  #endif
}

float Env::readTemperature()  {

    // Optional one wire temperature sensor

    #if defined(INTERNAL_SENSOR_DS18B20)

      float tempC = dsSensors->getTempC(dsTermometr);
      if(tempC == DEVICE_DISCONNECTED_C) {
        Serial.println(F("[DS18B20] Error: Could not read temperature data"));        
      } else return tempC + tempOffset;

    #endif

    // Default BME280 or replacements

    #if defined(INTERNAL_SENSOR_BME280) && INTERNAL_SENSOR_BME280 == false

        return -1000;
    #else

      if (tsensor) {
        return tempSensor.readTemperature() + tempOffset;
      } else return -1000;
    
    #endif
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

/* 
  Full UI redraw

   1. Update formatted time 
   2. Draw current UI layout according to this time to buffer
   3. Send buffer to screen
*/
void Env::updateScreen() {

    // if (lastState.partialUpdateTest) {
    //  Serial.println("[TEST PARTIAL MODE]");
    //  screen->drawTestPartial(true);
    //  return;
    // }

    // Serial.println("[updateScreen]");

    // if (cuiIsEnabled()) Serial.println(cuiName);
    // else {        
    //  Serial.println("[updateScreen][cui disabled]");
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

    #if defined(BAT_MIN_V) && defined(BAT_MAX_V)
    float min = BAT_MIN_V;
    float max = BAT_MAX_V;
    #else
    float min = 3.25;
    float max = 4.2;
    #endif

    if (v <= min) return 0.0;
    if (v >= max) return 100.0;

    float percent = ((max - min) / 100.0); 
    return (float) ((v - min) / percent);
}

bool Env::initSensors() {

    Wire.begin(DEFAULT_I2C_SDA, DEFAULT_I2C_SCL); 
    bool error = false;

    // One wire датчики

    #if defined(INTERNAL_SENSOR_DS18B20)

      oneWire = new OneWire(INTERNAL_SENSOR_DS18B20);
      dsSensors = new DallasTemperature(oneWire);
      Serial.print(F("[DS18B20] Locating devices..."));

      dsSensors->begin();
      Serial.print(F("Found "));
      Serial.print(dsSensors->getDeviceCount(), DEC);
      Serial.println(F(" devices."));  
      
      if (!dsSensors->getAddress(dsTermometr, 0)) {
        Serial.println(F("Unable to find address for Device 0")); 
        error = true;
      }

    #endif

    // I2C

    #if defined(INTERNAL_SENSOR_BME280) && INTERNAL_SENSOR_BME280 == false

        // replacement sensors can be placed here

    #else

      if (!tempSensor.begin(0x76)) {
          error = true;
          Serial.println("Could not find a valid BME280 sensor, check wiring!");
      } else {
          tsensor = true;
      }

    #endif

    #if defined(BAT_ADS1115)

      // aInputSensor.setGain(GAIN_TWOTHIRDS); // ads.setGain(GAIN_TWOTHIRDS); | 2/3х | +/-6.144V | 1bit = 0.1875mV

      if (!aInputSensor.begin()) {
          error = true;
          Serial.println("Could not find a valid ADS1115 sensor, check wiring!");
      } else {
          asensor = true;
      }

    #endif

    #if defined(CO2_SCD41) 

      uint16_t terror;
      char errorMessage[256];

      scd4x.begin(Wire);

      // stop potentially previously started measurement
      terror = scd4x.stopPeriodicMeasurement();
      if (terror) {
          Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
          errorToString(terror, errorMessage, 256);
          Serial.println(errorMessage);
      }

      uint16_t serial0;
      uint16_t serial1;
      uint16_t serial2;
      terror = scd4x.getSerialNumber(serial0, serial1, serial2);
      if (terror) {
          Serial.print("Error trying to execute getSerialNumber(): ");
          errorToString(terror, errorMessage, 256);
          Serial.println(errorMessage);
      } else {
          Serial.print("Serial: 0x");
          Serial.print(serial0 < 4096 ? "0" : "");
          Serial.print(serial0 < 256 ? "0" : "");
          Serial.print(serial0 < 16 ? "0" : "");
          Serial.print(serial0, HEX);
          Serial.print(serial1 < 4096 ? "0" : "");
          Serial.print(serial1 < 256 ? "0" : "");
          Serial.print(serial1 < 16 ? "0" : "");
          Serial.print(serial1, HEX);
          Serial.print(serial2 < 4096 ? "0" : "");
          Serial.print(serial2 < 256 ? "0" : "");
          Serial.print(serial2 < 16 ? "0" : "");
          Serial.print(serial2, HEX);
          Serial.println();
      }

      // Start Measurement
      terror = scd4x.startPeriodicMeasurement();
      if (terror) {
          Serial.print("Error trying to execute startPeriodicMeasurement(): ");
          errorToString(terror, errorMessage, 256);
          Serial.println(errorMessage);
      }

    #endif

    if (!error) {
        Serial.println("Sensors [OK]");
    }

    return error;
}

clockFormatted & Env::getFormattedTime() {
    return fTime;
}

String Env::getFormattedSensorTitle(bool indoor) {
  
  String title = getConfig()->getString(indoor ? cTitleIndoor : cTitleExternal);
  if (title.length() <= 0) title = FPSTR(indoor ? locIndoor : locOutdoor);
  if (title.equals(F("off"))) title = "";
  
  return title;
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

    tmp = FPSTR(longDateFormat);
    strftime(buffer, sizeof(buffer), tmp.c_str(), &stnow);
    fTime.date = buffer;

    tmp = FPSTR(shortDateFormat);
    strftime(buffer, sizeof(buffer), tmp.c_str(), &stnow); 
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
    if (pgm_read_byte(&textDateFormat) == 0) {

      fTime.monthText = tmp + " " + fTime.monthText;

    } else if (pgm_read_byte(&textDateFormat) == 1) {

      fTime.monthText = fTime.monthText + " " + tmp;

    } else if (pgm_read_byte(&textDateFormat) == 2) {

      fTime.monthText = fTime.monthText + tmp;
      fTime.monthText += "日";
    }

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

    #if defined(DISPLAY_2BIT) && defined(COLORMODE_2BIT_SUPPORT_RAM_FRIENDLY)
        return false;
    #endif

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
  Loads custom UI name by current loop cursor if enabled. If index is not exist, resets index to begin 
  Disables custom ui mode if nothing found
  
  Needs to be called during draw process to init current custom UI loop name if enabled
  Also resets cuiBitsPerPixel to be reinited during draw \ cuiRead process
*/
void Env::cuiApplyLoop() {

  // reset cui info before next output to keep actual info, if cui still enabled, cuiBitsPerPixel will updated in current row during drawUIToBufferCustom 
  lastState.cuiBitsPerPixel = -1; 

  if (lastState.cuiLoop) {    
    
    Serial.print(F("[LOOP] ")); Serial.println(String(lastState.cuiFileIndex)); 

    // applys cuiName by cursor index, originaly cui mode inited in cuiResetStateByConfig()
    lastState.cuiFileIndex = cuiGetNameByIndex(lastState.cuiFileIndex, cuiName);

    if (lastState.cuiFileIndex == -1) { // probably was removed by web-ui
      lastState.cuiFileIndex = cuiGetNameByIndex(-1, cuiName);
      resetPartialData();
      Serial.print(F("[Custom UI][LOOP Mode] reached end. Reset to index : ")); Serial.println(String(lastState.cuiFileIndex)); 
    }

    if (lastState.cuiFileIndex == -1) {
      lastState.cuiLoop = false;
      cuiSetState(false);
      
      Serial.println(F("[Custom UI][LOOP Mode] no data to display, disable mode")); 
    }
    
    // Serial.println("[LOOP end]" + String(lastState.cuiFileIndex) + " file : " + cuiName); 
  }
} 

void Env::cuiLoopNext() {
  if (lastState.cuiLoop) {
    lastState.cuiFileIndex++;
    resetPartialData();
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
  file.print(rotate ? "1;" : "0;");
  file.print(land ? "1;" : "0;");
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

int16_t Env::cuiGetIndexByName(String searchName) {

  cuiInitFS();

  searchName += ".bit";
  int index = -1;
  String tmpName;
  #if defined(ESP32)
      
    File root = SPIFFS.open("/cui");
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
        if (tmpName == searchName) {
            return index;
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
        if (searchName == tmpName) { 
          return index;           
          break;
        } 
    }

  #endif

  return -1;
}


/* -1 get first one. return -1 if file with index not found  */
int16_t Env::cuiGetNameByIndex(int16_t searchIndex, String &name) {
    
    cuiInitFS();

    int index = -1;
    name = "";
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

    if (name.length() == 0) index = -1;
    return index;
}

bool Env::cuiStepReadStart(bool close) {

  if (close && cuiStepFile) {
    Serial.println(F("[cuiStepReadStart] END"));
    cuiStepFile->close();
    delete cuiStepFile;
    cuiStepFile = NULL;
    return false;
  }

  if (cuiName.length() <= 0) {
    return false;
  }

  cuiInitFS();
  if (!cuiStepFile) {
    Serial.println(F("[cuiStepReadStart] START"));
    #if defined(ESP32)
      cuiStepFile = new File(SPIFFS.open("/cui/" + cuiName + ".bit", FILE_READ));
    #else
      cuiStepFile = new File(LittleFS.open("/cui/" + cuiName + ".bit", "r"));
    #endif  

    if (!cuiStepFile) {
      Serial.println(F("[cuiStepReadStart] Steper Enabled - FAIL"));
      return false;
    }

  } else {
    cuiStepFile->seek(0);
    Serial.println(F("[cuiStepReadStart] RESET"));
  }

  Serial.println(F("[cuiStepReadStart] Steper Enabled - OK"));
  return true;
}

unsigned char Env::cuiStepRead() {

  if (!cuiStepFile) {
    return 0;
  }

  if (!cuiStepFile->available()) {
    return 0;
  }
  return (unsigned char) cuiStepFile->read();
}

/*
  на ESP8266 нужна плановая перезагрузка при смене битности - см cuiPrepareRebootIfNeeded - проверка и подготовка к перезагрузке,
  на этапе рендера через драйвер в Screen4in2UI

  widgetsOnly - ignore canvas init, load only enabled cui widgets and screen orientation config params
  
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

              lastState.cuiBitsPerPixel = bitsPerPixel;

              if (!widgetsOnly) {

                  #if defined(DISPLAY_2BIT) && defined(COLORMODE_2BIT_SUPPORT_RAM_FRIENDLY)

                    Serial.println(F("[cuiReadStorageFile] [RAM Friendly mode] Skip 2-Bit render & display mode activation"));
                    // image will be rendered by step read from FLASH during update screen
                    getCanvas()->setBitsPerPixel(1);

                    if (bitsPerPixel > 1) {
                      widgetsOnly = true;
                    }

                  #else

                    if (screen->is4ColorsSupported() && bitsPerPixel == 2) {

                      getCanvas()->setBitsPerPixel(2);

                    } else {

                      if (bitsPerPixel != 1) {                        
                        Serial.print(F("Default color mode used. Unsupported color mode - ")); Serial.println(bitsPerPixel);
                      }

                      getCanvas()->setBitsPerPixel(1);
                    }

                  #endif

                  getCanvas()->setRotate(0);
                  getCanvas()->clear();
              }

              // getCanvas()->setBitsPerPixel(1);
              //cuiBits = bitsPerPixel;

            } else if (paramN == 1) {

              rotate = KellyOWParserTools::validateIntVal(widgetParam) > 0 ? true : false;

            } else if (paramN == 2) {

              land = KellyOWParserTools::validateIntVal(widgetParam) > 0 ? true : false;
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

      // Serial.print(F("READ : ")); Serial.println(widgetParam);
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
  Serial.print(F(" | Widgets found : ")); Serial.println(cuiWidgets.size());
  //Serial.println("---------------------" + String(ESP.getFreeHeap()));

  file.close();
  return true;
}

/*
  Get Partial update time based on user config.
  If user setted cUpdateMinutes = 1, default PARTIAL_UPDATE_INTERVAL or 60sec will be used, if setted any correct integer seconds value > 60 it will be accepted instead
*/
int Env::getPartialSleepTime() {
  
    int partialSleepTime = cfg.getInt(cUpdateMinutes);
    if (cfg.sanitizeError || partialSleepTime < 60 || partialSleepTime > lastState.sleepTime) {
        
        #if defined(PARTIAL_UPDATE_INTERVAL)
          partialSleepTime = PARTIAL_UPDATE_INTERVAL;
        #else
          partialSleepTime = 60;
        #endif
    }

    return partialSleepTime;
}

// sync config sensetive variables stored in RTC

void Env::applyConfigToRTC(bool configUpdate) {

    Serial.println(F("[applyConfigToRTC] apply config values to RTC memory")); 

    if (cfg.cfgValues[cSleepTime].length() > 0) {
        
        lastState.sleepTime = cfg.getInt(cSleepTime) * 60;
        if (cfg.sanitizeError || lastState.sleepTime <= 0) lastState.sleepTime = 10 * 60;

    } else {
        lastState.sleepTime = 10 * 60;
    }

    lastState.sleepTimeCurrent = lastState.sleepTime;
    lastState.updateMinutes = false;

    #if defined(PARTIAL_UPDATE_SUPPORT)

      if (cfg.cfgValues[cUpdateMinutes].length() > 0) {
        
        lastState.updateMinutes = cfg.getBool(cUpdateMinutes);

        if (cfg.sanitizeError) {
          lastState.updateMinutes = pgm_read_byte(&cfgUpdateMinutes) > 0;
        }

      } else {
        lastState.updateMinutes = pgm_read_byte(&cfgUpdateMinutes) > 0;
      }

    #endif

    const char* loopMarker = PSTR("-loop"); 
    lastState.cuiLoop = false;
    lastState.cuiTimeCurrent = 0;
    if (!configUpdate) {
      lastState.cuiFileIndex = -1;
      lastState.cuiBitsPerPixel = -1;
    }

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
          cfg.getStringList(cfg.cfgValues[cMqttDevicesIds], rawMqttIds, ',', 4);
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

        if (cfg.sanitizeError) {

          celsius = pgm_read_byte(&cfgCelsius) > 0;

        }

    } else {
        celsius = pgm_read_byte(&cfgCelsius) > 0;
    }

    if (cfg.cfgValues[cTimeFormat12].length() > 0) {

        hour12 = cfg.getBool(cTimeFormat12);
        if (cfg.sanitizeError) hour12 = pgm_read_byte(&cfg12HourFormat) > 0;

    } else {
        hour12 = pgm_read_byte(&cfg12HourFormat) > 0;
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

            } else if (key == cScreenLandscape || key == cScreenRotate || key == cImagePreset) {
              
                resetPartialData();

            } else if (key == cTimestamp) {
              
                #if defined(ESP32)
                  if(sntp_enabled()) sntp_stop();
                #else 
                  sntp_stop();
                #endif  
                yield();
                
                Serial.println(F("Force manual timestamp set. NTP connection stopped."));    
                lastState.timeConfigured = false;

                setenv("TZ", "GMT0", 1);
                tzset();

                initDefaultTime();

                lastState.timeConfigured = true;

                yield();
                break;
            }
        }
    }

    applyConfigToRTC(updatedKeys != NULL);
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