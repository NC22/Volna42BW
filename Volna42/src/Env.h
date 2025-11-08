#ifndef Env_h
#define Env_h

#include <Arduino.h>

#include <time.h>      // time() ctime()
#include "sys/time.h"


#include <UserDefines.h>
#include <SystemDefines.h>

#include <WidgetTypes.h>
#include "Config.h"

#include <KellyCanvas.h>
#include <KellyOpenWeather.h>

#if defined(ESP32)

    #include <HTTPClient.h>
    #include "esp_sntp.h"

    #include <FS.h>
    #include <SPIFFS.h>

#else 
    #include <ESP8266HTTPClient.h>
    #include "sntp.h"
    // #include <TZ.h>
    #if defined(FIX_DEEPSLEEP) && FIX_DEEPSLEEP > 0
        #include "esp8266/deepsleepFix.h"
    #endif

    #include <FS.h>
    #include <LittleFS.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ExternalSensor.h>


#if defined(CO2_SCD41) 
#include <SensirionI2CScd4x.h>
#endif

#include "GyverBME280.h" 

#if defined(BAT_ADS1115)
#include "Adafruit_ADS1X15.h" 
#endif

extern const char cdTimezone[] PROGMEM;
extern const char cdNtpHosts[] PROGMEM;
extern const char cfgWifiSSID[] PROGMEM;
extern const char cfgWifiP[] PROGMEM;
extern const char cfgOTALogin[] PROGMEM;
extern const char cfgOTAPassword[] PROGMEM;
extern const char cfgExtSensor[] PROGMEM;
extern const char cfgExtSensorL[] PROGMEM;
extern const char cfgExtSensorP[] PROGMEM;
extern const char cfgMqttHost[] PROGMEM;
extern const char cfgMqttLogin[] PROGMEM;
extern const char cfgMqttPassword[] PROGMEM;
extern const char cfgMqttClientIdPrefix[] PROGMEM;
extern const char cfgMqttHAPrefixDiscovery[] PROGMEM;
extern const char cfgMqttHADevice[] PROGMEM;
extern const char cfgMqttDomoticzPrefix[] PROGMEM;
extern const char cfgMqttDomoticzDeviceIds[] PROGMEM;
extern const unsigned int cdConfigVersion PROGMEM;

extern const uiWidgetDefaults widgetsDefaults[] PROGMEM;
extern const size_t widgetsDefaultsSize PROGMEM;

extern const uint8_t cfgCelsius PROGMEM;
extern const uint8_t cfg12HourFormat PROGMEM;
extern const uint8_t cfgUpdateMinutes PROGMEM;
extern const uint8_t cfgNightStart PROGMEM;
extern const uint8_t cfgNightEnd PROGMEM;

extern const uint8_t cfgExtLocal PROGMEM;                 
extern const char cfgExtSensorLocal[] PROGMEM;
extern const char cfgExtSensorLocalL[] PROGMEM;
extern const char cfgExtSensorLocalP[] PROGMEM;

extern const uint8_t textDateFormat PROGMEM;	
extern const char longDateFormat[] PROGMEM;
extern const char shortDateFormat[] PROGMEM;

#include <EnvStructs.h>
// todo - поддержка переворота экрана на 180 - нужно поменять порядок вывода полного буфера и делать правки для частичного обновления

#if defined(DISPLAY_TYPE_42)
#include <Screen4in2UI.h>
class Screen4in2UI;
#elif defined(DISPLAY_TYPE_154)
#include <Screen1in54UI.h>
class Screen1in54UI;
#endif

#if defined(ESP32)

    #include <esp_system.h>
    #include <esp_sleep.h>
    #include "soc/rtc.h"

    #define REASON_DEEP_SLEEP_AWAKE ESP_RST_DEEPSLEEP
    #define REASON_SOFT_RESTART ESP_RST_SW

    // RTC_DATA_ATTR extern int64_t rtcSleepStart;

    extern bool readRTCUserMemoryNVS(uint32_t* data, size_t len);
    extern bool writeRTCUserMemoryNVS(uint32_t* data, size_t len);
    extern bool readRTCUserMemoryActualRTC(rtcData &lastState);
    extern bool writeRTCUserMemoryActualRTC(rtcData &lastState);

    extern "C" {
    #include "esp32/clk.h"
    }
#else

    extern "C" {
    #include "user_interface.h"

    extern struct rst_info resetInfo;
    }

#endif

#if defined(INTERNAL_SENSOR_DS18B20)
    #include <OneWire.h>
    #include <DallasTemperature.h>
#endif

class Env {
    private :

        Config cfg;

        WiFiClient _wifiClient;      
        PubSubClient _mqttClient;
        bool mqtt = false; // connected (init stage passed)
        std::vector<String> rawMqttIds;
        
        unsigned long secondTimerStart; // initialized in constructor, by default = current millis()

        int telemetryBufferMax = 5;
        
        int minuteTimer;

        int syncEvery = 6;  // external data update \ data sync by wifi every n wake-ups --- check validateConfig for defaults
        
        bool tsensor = false; /* main Temperature & Humidity sensor is detected & initialized */
        bool asensor = false;
        bool ntp = false;
        bool partialUpdateRequired = false;
        uint8_t RTCMemInit = 0;
        
        bool cuiEnabled;
        bool cuiFSinited = false;
        File * cuiStepFile = NULL;
                    
        GyverBME280 tempSensor;    
        
        #ifdef BAT_ADS1115
        Adafruit_ADS1115 aInputSensor;
        #endif
        
        void keepTelemetry(int &key);

        #if defined(CO2_SCD41) 
        SensirionI2CScd4x scd4x;
        #endif

        #if defined(INTERNAL_SENSOR_DS18B20)
            OneWire * oneWire;
            DallasTemperature * dsSensors;
            DeviceAddress dsTermometr;
        #endif

        bool requestTimeByDomoticz(u_int8_t tryn = 1, u_int8_t attempts = 2);
        bool requestTimeByHA(u_int8_t tryn = 1, u_int8_t attempts = 2);
        void setDefaultLastStateData();
        bool restoreRTCmem();
        void applyConfigToRTC(bool configUpdate = false);
        int getPartialSleepTime();
        
        void mqttMessageReceivedCallback(char* topic, uint8_t* payload, unsigned int length);

        clockFormatted fTime;

        float tempOffset = 0.0;
        uint8_t humOffset = 0;
        uint8_t co2Offset = 0.0;
        uint8_t tempSource = 0;
        uint8_t humSource = 0;


    public:
        time_t defaultTime = 1510592825;

        #if defined(CO2_SCD41) 
            uint16_t scd4XCO2 = 0;
            bool scd4XCalibrated = false; // prevent EPPROM damage on accident HTTP rePOST - write only once during run cycle
            float scd4XTemp = -1000;
            float scd4XHumidity = -1000;
            unsigned int scd4XerrorTick = 0;
            unsigned long scd4XLastRead = 0;
        #endif

        bool batteryInit = false;
        bool mqttSuccess = false; // connected & succesfull data send
        
        bool batteryReadOnce = false;		
        float lastBat = BAD_SENSOR_DATA;

		// config depended variables, check validateConfig for defaults
        bool celsius;  // Celsius or Fahrenheit
        bool hour12;   // 12-hour format
        bool land;     // landscape screen orientation mode
        bool rotate;   // flip screen by 180deg
        
        uint8_t nightStart;
        uint8_t nightEnd;
		
        bool workEnabled = true; // redraw screen in constant mode - false - pause until webapi signal (/api/update)

        String wifiInfo = "";
        String lastError = "";        

        String cuiName = "";
        unsigned int cuiBits = 1;

        std::vector<uiWidgetStyle> cuiWidgets;
        KellyCanvas * canvas;
        #if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176_B)
        Screen4in2UI * screen;
        #elif defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)
        Screen1in54UI * screen;
        #endif
        rtcData lastState;

        Env();
        
        void ledBlink(int n=1);

        bool setupNTP(unsigned int attempt = 1);
        void disableNTP(); 

        bool mqttSendCurrentData();
        void mqttInit();
        void mqttHAAutodetectionInit();
        bool mqttIsHAMode();

        bool initSensors();

        // void bootScreen();
        void updateTelemetry();
        void updateScreen();
        bool updateExtSensorData();
        void initDefaultTime(bool compensateDelay = true);
        void resetTimers(bool minuteTimerOnly = false);
        void resetPartialData();
        void updateExtIconState();

        void forceRefreshAll();

        void sleepSensors();
        void sleep();
        void begin();
        void sync();
        void tick();

        float toFahrenheit(float celsius);
        
        bool updateSCD4X();
        bool waitSCD4X();
        int32_t calibrateSCD4X(int value);
        void updateBattery(int &telemetryIndex);

        float readTemperature(bool defaultSource = false);
        float readPressure();
        float readHumidity(bool defaultSource = false);
        
        bool isPartialUpdateRequired();
        bool isSyncRequired();
        bool isOnBattery();
        bool isSleepRequired();
        
        float readBatteryV();
        float readBatteryVtest();
        float getBatteryLvlfromV(float v);
        
        // externalSensorData readExternalData();
        
        String getFormattedExtSensorLastSyncTime(bool full = false);
        String getFormattedSensorTitle(bool indoor);
         
        clockFormatted & getFormattedTime();
        // telemetry & getLatestTelemetry();
        rtcData & getCurrentState();
        void saveCurrentState();
        String getTelemetryJSON();

        KellyCanvas * getCanvas();
        void updateTime(time_t dt = 0);
        
        bool isBattery();

        bool commitConfig();
        void validateConfig(unsigned int version = 0, std::vector<cfgOptionKeys> * updatedKeys = NULL);
        void restart(String reason = "", bool skipRTCSanitize = false);
        bool resetConfig(bool resetWifi = false);
        void restartNTP(bool resetOnly = false);
        
        // custom UI

        void cuiApplyLoop();
        bool cuiPrepareRebootIfNeeded();
        int16_t cuiGetNameByIndex(int16_t index, String &name);
        int16_t cuiGetIndexByName(String name);
        bool cuiIsEnabled();
        void cuiSetState(bool state, String sname = "");
        
        String cuiGetListFilesJSON();
        String cuiGetFSInfoJSON();
        bool cuiInitFS(bool format = false);
        // bool cuiWebReadStorageFile(String name = "");
        // unsigned char cuiWebRead();
        bool cuiReadStorageFile(bool widgetsOnly = false);
        bool cuiDeleteStorageFile(String name);
        bool cuiWriteStorageFile(bool append = false, int dataSize = 0);
        bool cuiStepReadStart(bool close);
        unsigned char cuiStepRead();

        uiWidgetType cuiGetTypeById(int key);
        int cuiGetIdByType(uiWidgetType type);
        bool cuiSetWidget(uiWidgetStyle widget);
        void cuiResetWidgets();
        void cuiResetStateByConfig();

        void cuiLoopNext();

        Config * getConfig();
};

#endif	