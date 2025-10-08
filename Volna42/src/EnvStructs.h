
#if !defined(EnvStructsH)
#define EnvStructsH


#include <KellyOpenWeather.h>
#include <WidgetTypes.h>
#include <SystemDefines.h>

typedef struct {
    
    unsigned int size;
    String name;

} uiStyleFile;

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
// ~230 bytes \ 512 bytes max, decrease lastTelemetry buffer size (change also - telemetryBufferMax var) if need more space

typedef struct {
    
    time_t t = 0;                  // timestamp - initialized on ntp setup, refreshed before sleep & on wakeup (+adds number of sleep minutes)

    int wakeUps = 0; 
    int lastTelemetrySize = 0;     // size of actually filled elements in lastTelemetry array
    int connectTimes = 0;          // number of succesfull connections with external sensor
    bool onBattery = false;        // on battery check - setted once on first setup

    int8_t cuiBitsPerPixel;        // last CUI bits per pixel mode (last output mode)

    u_int8_t lowBatTick = 0;       // times battery sensor detected low battery state, will show sad hangry cat if count enough
    int16_t cuiFileIndex = -1;     // temporary file index - if we need to load once on reboot some specified custom user interface without save to permanent memory    
    int cuiTimeCurrent = 0;        // cui timer for loop mode (change custom ui every N seconds, use cuiFileIndex as cursor)
    bool cuiLoop = false;          // todo - exclude from loop [default] cui ?
    bool cuiResetOnReboot = false; // needed when we reboot manually for some reason, but not when move cui loop cursor and need to reboot for realloc memory by switch form 1-bit to 2-bit mode       

    telemetry lastTelemetry[ENV_TELEMETRY_MAX];   
    externalSensorData extData; 

    bool updateMinutes = false;
    bool timeConfigured = false;   // if true - defaultTime & .t was already initialized (First init NTP server | Manualy | correct Time var found in RTC)
    partialUpdateData lastPartialPos;

    int sleepTime = 60;            // wake-up every n seconds  --- check validateConfig for defaults
    int sleepTimeCurrent = 1000;   // [Used only in constant mode & if partial update is active] seconds timer for non-deep sleep mode or for update minutes mode
    
    unsigned int cfgVersion = 0;

} rtcData; 

#endif