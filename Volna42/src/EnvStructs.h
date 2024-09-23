
#if !defined(EnvStructsH)
#define EnvStructsH


#include <KellyOpenWeather.h>
#include <WidgetTypes.h>

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
    
    time_t t = 0;                      // timestamp - initialized on ntp setup, refreshed before sleep & on wakeup (+adds number of sleep minutes)

    int wakeUps; 
    int lastTelemetrySize;         // size of actually filled elements in lastTelemetry array
    int connectTimes;              // number of succesfull connections with external sensor
    bool onBattery;                // on battery check - setted once on first setup

    u_int8_t cuiBitsPerPixel;      // last CUI bits per pixel mode

    u_int8_t lowBatTick;           // times battery sensor detected low battery state, will show sad hangry cat if count enough
    int16_t cuiFileIndex;          // temporary file index - if we need to load once on reboot some specified custom user interface without save to permanent memory    
    int cuiTimeCurrent;            // cui timer for loop mode (change custom ui every N seconds, use cuiFileIndex as cursor)
    bool cuiLoop;                  // todo - exclude from loop [default] cui ?
    bool cuiResetOnReboot;         // needed when we reboot from web ui, but not when move cui loop cursor and need to reboot for realloc memory by switch form 1-bit to 2-bit mode       

    telemetry lastTelemetry[5];   
    externalSensorData extData; 

    bool updateMinutes;
    bool timeConfigured;           // if true - defaultTime & .t represents actual time (restored from RTC memory or recieved from NTP server)
    partialUpdateData lastPartialPos;

    int sleepTime;                 // wake-up every n seconds  --- check validateConfig for defaults
    int sleepTimeCurrent;          // [Used only in constant mode & if partial update is active] seconds timer for non-deep sleep mode or for update minutes mode
    
    unsigned int cfgVersion = 0;
} rtcData; 

#endif