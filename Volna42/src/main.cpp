
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
    #include <NVSMem.h>
#else
#endif

#include <EnvConfigOptions.h>
#include <UserDefines.h>
#include <UserSettings.h>
#include <LocalData.h>
#include <KellyCanvas.h>
#include <Env.h>

Env env = Env();

#if defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_RY_BW_42_UC8176_B)
KellyCanvas canvas = KellyCanvas(400, 300);
Screen4in2UI screenController = Screen4in2UI(&env);

#elif defined(HELTEC_BW_15_S810F)

KellyCanvas canvas = KellyCanvas(200, 200);
Screen1in54UI screenController = Screen1in54UI(&env);
    
#endif

WebServerEink server = WebServerEink(&env);
WiFiManager wifi = WiFiManager();

void setup()
{
  Serial.begin(115200);

  env.canvas = &canvas;
  env.screen = &screenController;
  env.begin(); 

  if (env.isPartialUpdateRequired()) {
      
    env.initDefaultTime();
    env.updateTime();

    Serial.println(F("[Partial update requested] Update minutes only and continue sleep..."));
    env.screen->updatePartialClock();
    env.sleep();

  } else if (env.isSleepRequired() && !env.isSyncRequired()) {
    
    Serial.println(F("[Sleep requested]: no any addition actions required...update screen & goto sleep"));  
    
    delay(300);
    env.updateTelemetry();

    env.initDefaultTime();
    env.updateScreen();
    env.sleep();

  } else {

    // Sync & refresh data required
    // Connect to WiFi -> run full get data sequense (External sensor & update time by NTP, get battery info, update screen ... ) -> sleep if server mode disabled

    Serial.println(F("[Update data needed] connect to wifi..."));
    bool fallBackWifi = false;

    wl_status_t wifiStatus = wifi.connect(env.getConfig()->getString(cWifiNetwork), env.getConfig()->getString(cWifiPassword)); 
    if (wifiStatus != WL_CONNECTED && env.getConfig()->getString(cWifiNetworkFallback).length()) {
        wifiStatus = wifi.connect(env.getConfig()->getString(cWifiNetworkFallback), env.getConfig()->getString(cWifiPasswordFallback)); 
        fallBackWifi = true;
        Serial.println(F("Fail to connect main Wifi. Fallback wifi used"));
    }

    if (wifiStatus == WL_CONNECTED) {
      
        Serial.print(F("connected to Wifi - IP: ")); Serial.println(wifi.getIP().toString());

        env.setupNTP();
        env.updateExtSensorData();
        env.updateExtIconState();
        env.mqttSendCurrentData();
        env.wifiInfo = env.isSleepRequired() ? "" : wifi.getIP().toString(); 

    } else if (!env.isSleepRequired()) {

      // Not on battery & WiFi connection fail -> Run as Access Point

      Serial.print(F("fail to connect Wifi - ")); 
      if (!fallBackWifi) Serial.println(env.getConfig()->getString(cWifiNetwork)); 
      else {
        Serial.print(F("Fallback - ")); 
        Serial.println(env.getConfig()->getString(cWifiNetworkFallback)); 
      }

      wifi.runAsAccesspoint(FPSTR(defaultWifiAP), FPSTR(defaultWifiAPP));     
      env.wifiInfo = wifi.getIP().toString() + " AP"; 
      Serial.print(F("IP: ")); Serial.println(wifi.getIP().toString());

    } else if (env.lastState.connectTimes == 0) { 
      
      // WiFi connection fail

      // Ниразу не было успешных подключений по WiFi - мы не знаем даже примерного времени ни данных внешних датчиков
      // если на устройстве сохранено время по умолчанию, оно будет использовано

      env.wifiInfo = FPSTR(noWiFi);
      Serial.println(F("fail to connect Wifi - critical - device not configured"));    
    }

    Serial.println(F("Screen update"));    
    env.updateTelemetry();

    // [extra exit] on battery low power
    // Из за неправильной каллибровки не разу не доходил до этого статуса - todo вынести переменную в конфиг для ручной калибровки порога срабатывания тригера
    // По наблюдениям - для запуска ESP8266 + экран + I2S датчики критическое напряжение аккумулятора ~3.57v-3.50v - после, в определенный цикл запуска устройство может выключится в процессе работы
    // или просто не проснутся

    if (env.isOnBattery() && env.lastState.lowBatTick > 3) {

      if (env.lastState.lowBatTick < 100) {
        env.lastState.lowBatTick = 100;
        env.screen->drawUILowBat();
        env.screen->updateScreen();
      }

      Serial.println(F("[On battery] LOW POWER or wrong detect voltage - [power off]"));
      env.sleep(); // todo - maybe increase sleep time in this mode too
      return;
    }

    if (wifiStatus == WL_CONNECTED) {
      env.sync();
    }

    env.updateScreen();

    if (!env.isSleepRequired()) {

      server.runConfigured();
      Serial.println(F("[Not on battery] Http server enabled | wait sync by timeout"));
    } else {
      Serial.println(F("[Sleep requested] goto sleep"));
      env.sleep();
    }
  }

  // testTick = millis();
}

void loop()
{  
  server.tick();

  if (!screenController.tick()) env.tick();
  delay(10);
}
