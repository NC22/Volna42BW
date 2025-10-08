
/* 
	Volna 2BW42 firmware
	nradiowave | https://42volna.com/

  Configuration files : UserDefines.h & UserSettings.h

*/

#include <Arduino.h>
#include <stdlib.h>
#include <WebServerEink.h>
#include <WifiManager.h>

#if defined(ESP32)
    #include <esp32/NVSMem.h>
#else
#endif

#include <EnvConfigOptions.h>
#include <UserDefines.h>
#include <SystemDefines.h>
#include <UserSettings.h>
#include <LocalData.h>
#include <KellyCanvas.h>
#include <Env.h>
Env env = Env();

#if defined(DISPLAY_TYPE_42)
KellyCanvas canvas = KellyCanvas(400, 300);
Screen4in2UI screenController = Screen4in2UI(&env);

#else

KellyCanvas canvas = KellyCanvas(200, 200);
Screen1in54UI screenController = Screen1in54UI(&env);
    
#endif

WebServerEink server = WebServerEink(&env);
WiFiManager * wifi = new WiFiManager();

/* 
  Attempt to connect to WiFi according to config 
  return 0 - if wifi ap is not accessable, 1 - main wifi, 2 - fallback
*/
uint8_t wifiConnect() {
   
  Serial.print(F("Connect to wifi..."));
  uint8_t result = 0;

  if (env.getConfig()->getString(cWifiNetwork).length()) {

    if (wifi->connect(env.getConfig()->getString(cWifiNetwork), env.getConfig()->getString(cWifiPassword), true) == WL_CONNECTED) {
        Serial.print(F("OK - ")); Serial.print(env.getConfig()->getString(cWifiNetwork));
        result = 1;
    } else {
        Serial.print(F("FAIL - ")); Serial.print(env.getConfig()->getString(cWifiNetwork));
    }
  }

  if (result == 0 && env.getConfig()->getString(cWifiNetworkFallback).length()) {

    if (wifi->connect(env.getConfig()->getString(cWifiNetworkFallback), env.getConfig()->getString(cWifiPasswordFallback), true) == WL_CONNECTED) {      
      Serial.print(F("...OK [Fallback] - ")); Serial.print(env.getConfig()->getString(cWifiNetworkFallback));
      result = 2;
    } else {
      Serial.print(F("...FAIL [Fallback] - ")); Serial.print(env.getConfig()->getString(cWifiNetworkFallback));
    }
  }

  if (result > 0) {

    Serial.print(F(" | IP: ")); Serial.print(wifi->getIP().toString());

    if (!env.isSleepRequired()) {
      env.wifiInfo = wifi->getIP().toString(); 
    }
  }

  Serial.println();
  return result;
}

void setup()
{

  Serial.begin(115200);

  #if defined(DEBUG_ESP_WIFI)

    Serial.setDebugOutput(true);
    WiFi.printDiag(Serial);
    delay(5000);
    
  #endif

  env.canvas = &canvas;
  env.screen = &screenController;
  env.begin(); 

  if (env.isPartialUpdateRequired()) {
    
    env.disableNTP();  
    env.initDefaultTime();
    env.updateTime();

    Serial.println(F("[Partial update requested] Update minutes only and continue sleep..."));
    env.screen->updatePartialClock();
    env.sleep();

  } else if (env.isSleepRequired() && !env.isSyncRequired()) {
    
    Serial.println(F("[Sleep requested]: no any addition actions required...update screen & goto sleep"));  
    env.disableNTP();  
    env.initDefaultTime(); 

    //  ToDo : датчик SCD требует 5сек. на переинициализацию т.к. шина I2C остается жить. Нужно выключать его по питанию отдельно 
    env.waitSCD4X();
    delay(300);

    env.updateTelemetry();

    env.updateScreen();
    env.sleep();

  } else {

    // Sync & Refresh data required
    // Connect to WiFi -> run full get data sequense (External sensor & update time by NTP, get battery info, update screen ... ) -> sleep if server mode disabled
    if (!env.lastState.timeConfigured) env.ledBlink(2); // clean load blink

    Serial.println(F("[Update data needed]"));
    uint8_t wifiConnectResult = wifiConnect();

    if (wifiConnectResult > 0) {
          
      env.setupNTP();

    } else if (!env.isSleepRequired()) {

      // Not on battery & WiFi connection fail -> Run as Access Point

      wifi->runAsAccesspoint(FPSTR(defaultWifiAP), FPSTR(defaultWifiAPP));
      env.wifiInfo = wifi->getIP().toString() + " AP"; 
      env.initDefaultTime();

      Serial.print(F("IP: ")); Serial.println(wifi->getIP().toString());

    } else if (env.lastState.connectTimes == 0) { 
      
      // WiFi connection fail -> Need to sleep -> Cant start AP mode

      // Ниразу не было успешных подключений по WiFi - мы не знаем даже примерного времени ни данных внешних датчиков
      // если на устройстве сохранено время по умолчанию, оно будет использовано

      env.wifiInfo = FPSTR(noWiFi);
      Serial.println(F("FAIL to connect Wifi - critical - device not configured"));    
    }

    env.waitSCD4X();        
    env.updateTelemetry();

    if (wifiConnectResult > 0) {
      env.updateExtSensorData();
      env.mqttSendCurrentData();
    }

    env.updateExtIconState();

    // [extra exit] on battery low power, lowBatTick - 10% от заряда
    // По наблюдениям - для запуска ESP8266 + экран + I2S датчики критическое напряжение аккумулятора ~3.57v-3.50v - после, в определенный цикл запуска устройство может выключится в процессе работы
    // или просто не проснутся

    Serial.println(F("[Screen update]"));

    if (env.isOnBattery() && env.lastState.lowBatTick > 3) {

      if (env.lastState.lowBatTick < 100) {
        env.lastState.lowBatTick = 100;
        env.screen->drawUILowBat();
        env.screen->updateScreen();
      }

      Serial.println(F("[On battery] LOW POWER or wrong detect voltage - [power off]"));
      if (wifiConnectResult > 0) wifi->prepareToSleep();
      env.sleep(); 
      return;
    }

    env.sync();
    env.updateScreen();

    if (!env.isSleepRequired()) {

      server.runConfigured();
      Serial.println(F("[Not on battery] Http server enabled | wait sync by timeout"));
    } else {
      Serial.println(F("[Sleep requested] goto sleep"));
      if (wifiConnectResult > 0) wifi->prepareToSleep();
      env.sleep();
    }
  }
}

void loop()
{  
  server.tick();

  /*
    WiFi controller keeped only for AP mode, to keep trying to reconnect by wifi config every
    WIFI_AP_MODE_RECONNECT_TIMER seconds (by default once per 3 min)

    in case of usual connection setAutoReconnect=true is enabled
  */
  if (wifi) {
    if (!wifi->isAPmode()) {

      delete wifi; wifi = NULL;

    } else if (wifi->stReconnectTick()) {

      if (wifiConnect() > 0) {

        Serial.println(F("[Successfull connect to WiFi] End AP mode, restart NTP"));
        env.restartNTP();
        env.forceRefreshAll();
        env.updateScreen();

        delete wifi; wifi = NULL;        
      } else if (!wifi->isAPmode()) {

        Serial.println(F("[Fail connect to WiFi] Restart AP mode"));
        wifi->runAsAccesspoint(FPSTR(defaultWifiAP), FPSTR(defaultWifiAPP));
      }
    }
  }

  if (!screenController.tick()) {
    env.tick();
  }

  delay(10);
}
