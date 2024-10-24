/* Global system constants, must be included after UserDefines.h */

#define BAD_SENSOR_DATA -1000.0
#define AP_NETWORK_WIFI_CHANNEL 12  
#define ENV_TELEMETRY_MAX 5

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