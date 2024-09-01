#include <EnvConfigTypes.h>
#include <UserDefines.h>
#include <Arduino.h>

const cfgOption cfgOptions[] = { 
    
    {cWifiNetwork, ctypeString, false, "wifiNetwork", true},
    {cWifiPassword, ctypeString, true, "wifiPassword", true},
    
    {cTimezone, ctypeString, false, "timezone", true},    
    {cTempOffset, ctypeFloat, false, "tempOffset", false},

    {cModeDuration, ctypeInt, false, "modeDuration", false},
    {cModeList, ctypeString, false, "modeList", false},
    {cModeListEnabled, ctypeString, false, "modeListEnabled", false},
    
    {cNtpHosts, ctypeString, false, "ntpHosts", true},
    
    {cMqttHost, ctypeString, false, "mqttHost", true},
    {cMqttPort, ctypeInt, false, "mqttPort", true},
    {cMqttLogin, ctypeString, false, "mqttLogin", true},
    {cMqttPassword, ctypeString, true, "mqttPassword", true},
    {cToFahrenheit, ctypeBool, false, "toFahrenheit", false},
    {cMqttPrefixIn, ctypeString, false, "mqttPrefixIn", false},     // Domoticz Prefix
    {cMqttDevicesIds, ctypeString, false, "mqttDevicesIds", false}, // Domoticz device ids ( T&H&B, T&H, T )

    {cScreenRotate, ctypeBool, false, "screenRotate", false},
    {cScreenLandscape, ctypeBool, false, "screenLandscape", false},
    {cSleepTime, ctypeInt, false, "sleepTime", false},
    {cTimestamp, ctypeString, false, "timestamp", false},

    {cWifiNetworkFallback, ctypeString, false, "wifiNetworkFallback", false},
    {cWifiPasswordFallback, ctypeString, false, "wifiPasswordFallback", false},

    {cImagePreset, ctypeString, false, "imagePreset", false},

    {cExtSensorLink, ctypeString, false, "extSensorLink", false},
    {cExtSensorLogin, ctypeString, false, "extSensorLogin", false},
    {cExtSensorPassword, ctypeString, true, "extSensorPassword"},

    {cMqttHAPrefix, ctypeString, false, "mqttHAPrefix", false}, // Home Assistant prefix
    {cMqttHADevice, ctypeString, false, "mqttHADevice", false}, // Home Assistant device description (Id, Name)
    {cSyncEvery, ctypeString, false, "syncEvery", false},
    {cTimeFormat12, ctypeBool, false, "timeFormat12", false},

    {cUpdateMinutes, ctypeBool, false, "updateMinutes", false},
};

const size_t cfgOptionsSize PROGMEM = sizeof(cfgOptions) / sizeof(cfgOptions[0]);

// Some opions exclude from web-ui list in Env::begin based on user defines

const uiWidgetDefaults widgetsDefaults[] PROGMEM = { 
    {0, uiNone},
    {1, uiClock},
    {2, uiInfo},
    {3, uiTemp},
    {4, uiHum},
    {5, uiTempRemote},     
    {6, uiHumRemote},     
    {7, uiBatRemote},     
    {8, uiBat},     
    {9, uiInfoIP},  
    {10, uiInfoVoltage},  
    {11, uiLastSyncRemote}, 
    {12, uiInfoSyncNumRemote}, 
    {13, uiInfoMessage}, 
    {14, uiShortInfoSyncRemote}, 
    {15, uiPressure}, 
    {16, uiDate}, 
};

const size_t widgetsDefaultsSize PROGMEM = sizeof(widgetsDefaults) / sizeof(widgetsDefaults[0]);
