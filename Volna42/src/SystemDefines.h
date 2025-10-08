/* 

  Global system constants, must be included after UserDefines.h 

  Системные констранты & константы отладки. Не редактировать. Некоторые параметры могут быть определены через UserDefines (с проверкой !defined)
*/

#define BAD_SENSOR_DATA -1000.0
#define AP_NETWORK_WIFI_CHANNEL 12  
#define ENV_TELEMETRY_MAX 5
#define NTP_ATTEMPT1_TIMEOUT 25
#define NTP_ATTEMPTN_TIMEOUT 60

#if defined(WAVESHARE_BW_42_SSD1683)
    #define DISPLAY_2BIT
#endif

#if defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_RY_BW_42_UC8176_B)
    #define DISPLAY_TYPE_42
#else
    #define DISPLAY_TYPE_154
#endif

#if defined(ESP32)
  #define CONFIG_TIME_FUNCTION configTzTime
#else
  #define CONFIG_TIME_FUNCTION configTime
#endif

// #define DISABLE_WEB_UPLOADER // reduce firmware size by 65kb (exclude webUI uploader packed files)
// #define DISABLE_SCREEN_UPDATE

// Если точка доступа не была найдена при запуске устройства и был включен режим точки доступа (Access Point) 
// то пробовать повторно ее найти и подключится через заданное кол-во секунд :

#if !defined(WIFI_AP_MODE_RECONNECT_TIMER)
  #define WIFI_AP_MODE_RECONNECT_TIMER 3 * 60 // -1 или не задано - не пытаться подключится
#endif