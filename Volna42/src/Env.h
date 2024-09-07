#ifndef Env_h
#define Env_h

#include <Arduino.h>

#include <time.h>      // time() ctime()
#include "sys/time.h"


#include <UserDefines.h>
#include <WidgetTypes.h>
#include "Config.h"

#include <KellyCanvas.h>
#include <KellyOpenWeather.h>

#if defined(ESP32)
    #include <HTTPClient.h>
    #include "esp_sntp.h"
    // #include <thirdparty/TZ.h>
#else 
    #include <ESP8266HTTPClient.h>
    #include "sntp.h"
    // #include <TZ.h>
#endif

#include <WiFiClient.h>
#include <PubSubClient.h>


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

extern const uint8_t textDateFormat PROGMEM;	
extern const char longDateFormat[] PROGMEM;
extern const char shortDateFormat[] PROGMEM;

typedef struct {
    
    unsigned int size;
    String name;

} uiStyleFile;

// todo - поддержка переворота экрана на 180 - нужно поменять порядок вывода полного буфера и делать правки для частичного обновления

#if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683)
#include <Screen4in2UI.h>
class Screen4in2UI;
#elif defined(HELTEC_BW_15_S810F)
#include <Screen1in54UI.h>
class Screen1in54UI;
#endif

#if defined(ESP32)

    #include <esp_system.h>
    #include <esp_sleep.h>
    #include "soc/rtc.h"

    #define REASON_DEEP_SLEEP_AWAKE ESP_RST_DEEPSLEEP
    #define REASON_SOFT_RESTART ESP_RST_SW

    extern bool readRTCUserMemory(uint32_t index, uint32_t* data, size_t len);
    extern bool writeRTCUserMemory(uint32_t index, uint32_t* data, size_t len);
#else

    extern "C" {
    #include "user_interface.h"

    extern struct rst_info resetInfo;
    }

#endif

typedef struct {
    
    bool isDataValid;
    float temperature;
    float pressure;
    float humidity;
    int bat;
    KellyOWIconType icon;
    time_t t; // last successfull sync time

} externalSensorData;

typedef struct {
    
  String timeTextS;
  String date; 
  String dateShort; 
  String timeText;
  String dayText;
  String monthText;
  u_int8_t monthN;
  u_int8_t h;
  u_int8_t m;
  time_t t;
  bool pm;
  
} clockFormatted;

typedef struct {
    
    float temperature;
    float pressure;
    float humidity;
    
    float bat;
    
    time_t t;
  
} telemetry;


// RTC memory struct 
// ~298 bytes \ 512 bytes max, decrease lastTelemetry buffer size (change also - telemetryBufferMax var) if need more space

typedef struct {
    
    time_t t;                      // timestamp - initialized on ntp setup, refreshed before sleep & on wakeup (+adds number of sleep minutes)

    int wakeUps; 
    int lastTelemetrySize;         // size of actually filled elements in lastTelemetry array
    int connectTimes;              // number of succesfull connections with external sensor
    bool onBattery;                // on battery check - setted once on first setup

    u_int8_t lowBatTick;           // times battery sensor detected low battery state, will show sad hangry cat if count enough
    int16_t cuiFileIndex;          // temporary file index - if we need to load once on reboot some specified custom user interface without save to permanent memory
    int cuiTimeCurrent;            // cui timer for loop mode (change custom ui every N seconds, use cuiFileIndex as cursor)
    bool cuiLoop;                  // todo - exclude from loop [default] cui ?
    bool cuiResetOnReboot;         // needed when we reboot from web ui, but not when move cui loop cursor and need to reboot for realloc memory by switch form 1-bit to 2-bit mode       

    telemetry lastTelemetry[10];   
    externalSensorData extData; 

    bool updateMinutes;
    bool timeConfigured;           // if true - defaultTime & .t represents actual time (restored from RTC memory or recieved from NTP server)
    partialUpdateData lastPartialPos;

    int sleepTime;                 // wake-up every n seconds  --- check validateConfig for defaults
    int sleepTimeCurrent;          // [Used only in constant mode & if partial update is active] seconds timer for non-deep sleep mode or for update minutes mode
    
    unsigned int cfgVersion;
} rtcData; 


class Env {
    private :

        Config cfg;

        WiFiClient _wifiClient;
        PubSubClient _mqttClient;
        bool mqtt = false; // connected (init stage passed)
        std::vector<String> rawMqttIds;
        
        unsigned long secondTimerStart;

        int telemetryBufferMax = 10;
        
        int minuteTimer;

        // todo - show connectfailsnum
        // todo - night mode isNight
        // todo - ntpUpdateEvery = 144; // sync time by ntp every
        // int syncEveryNight = 12; // bufferMax = 12; 

        int syncEvery = 6;  // external data update \ data sync by wifi every n wake-ups --- check validateConfig for defaults
        
        bool tsensor = false;
        bool asensor = false;
        bool ntp = false;
        bool partialUpdateRequired = false;
        
        bool cuiEnabled;
        bool cuiFSinited = false;
                    
        GyverBME280 tempSensor;    
        
        #ifdef BAT_ADS1115
        Adafruit_ADS1115 aInputSensor;
        #endif
        
        void keepTelemetry(int key);

        String sanitizeResponse(String var);
        void setDefaultLastStateData();
        bool restoreRTCmem();
        void applyConfigToRTC();
        
        void mqttMessageReceivedCallback(char* topic, uint8_t* payload, unsigned int length);

        clockFormatted fTime;
        float tempOffset = 0.0;
        KellyOWIconType lastOWstate = kowUnknown;

    public:
        String timezone = "MSK-3";
        time_t defaultTime = 1510592825;

        bool batteryInit = false;
        bool mqttSuccess = false; // connected & succesfull data send
		
		// config depended variables, check validateConfig for defaults
        bool celsius;  // Celsius or Fahrenheit
        bool hour12;   // 12-hour format
        bool land;     // landscape screen orientation mode
        bool rotate;     // flip screen by 180deg
		
        bool noScreenTest = false;
        bool workEnabled = true; // redraw screen in constant mode - false - pause until webapi signal (/api/update)

        String wifiInfo = "";
        String lastError = "";        

        String cuiName = "";
        unsigned int cuiBits = 1;

        std::vector<uiWidgetStyle> cuiWidgets;
        KellyCanvas * canvas;
        #if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683)
        Screen4in2UI * screen;
        #elif defined(HELTEC_BW_15_S810F)
        Screen1in54UI * screen;
        #endif
        rtcData lastState;

        Env();
        
        bool setupNTP();

        bool mqttSendCurrentData();
        void mqttInit();
        void mqttHAAutodetectionInit();
        bool mqttIsHAMode();

        void initSensors();

        void updateTelemetry();
        void updateScreen();
        bool updateExtSensorData();
        void initDefaultTime();
        void resetTimers(bool minuteTimerOnly = false);
        void resetPartialData();
        void updateExtIconState();

        void sleep();
        void begin();
        void sync();
        void tick();

        float toFahrenheit(float celsius);

        float readTemperature();
        float readPressure();
        float readHumidity();
        
        bool isPartialUpdateRequired();
        bool isSyncRequired();
        bool isOnBattery();
        bool isSleepRequired();
        
        float readBatteryV();
        float readBatteryVtest();
        float getBatteryLvlfromV(float v) ;
        
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
        void validateConfig(unsigned int version = -1, std::vector<cfgOptionKeys> * updatedKeys = NULL);
        void restart(String reason = "");
        bool resetConfig(bool resetWifi = false);
        void restartNTP();
        
        // custom UI

        void cuiApplyLoop();
        bool cuiPrepareRebootIfNeeded();
        int16_t cuiGetNameByIndex(int16_t index, String &name);
        // int16_t cuiGetIndexByName(String &name);
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

        uiWidgetType cuiGetTypeById(int key);
        int cuiGetIdByType(uiWidgetType type);
        bool cuiSetWidget(uiWidgetStyle widget);
        void cuiResetWidgets();
        void cuiResetStateByConfig();

        void cuiLoopTestNext();

        Config * getConfig();
};

#endif	