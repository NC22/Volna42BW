#include <EnvConfigTypes.h>
#include <UserDefines.h>
#include <SystemDefines.h>
#include <Arduino.h>

const char pgmCfgWifiNetwork[] PROGMEM = "wifiNetwork";
const char pgmCfgWifiPassword[] PROGMEM = "wifiPassword";
const char pgmCfgTimezone[] PROGMEM = "timezone";
const char pgmCfgTempOffset[] PROGMEM = "tempOffset";
const char pgmCfgModeDuration[] PROGMEM = "modeDuration";
const char pgmCfgModeList[] PROGMEM = "modeList";
const char pgmCfgModeListEnabled[] PROGMEM = "modeListEnabled";
const char pgmCfgNtpHosts[] PROGMEM = "ntpHosts";
const char pgmCfgMqttHost[] PROGMEM = "mqttHost";
const char pgmCfgMqttPort[] PROGMEM = "mqttPort";
const char pgmCfgMqttLogin[] PROGMEM = "mqttLogin";
const char pgmCfgMqttPassword[] PROGMEM = "mqttPassword";
const char pgmCfgToFahrenheit[] PROGMEM = "toFahrenheit";
const char pgmCfgMqttPrefixIn[] PROGMEM = "mqttPrefixIn";     
const char pgmCfgMqttDevicesIds[] PROGMEM = "mqttDevicesIds"; 

const char pgmCfgScreenRotate[] PROGMEM = "screenRotate";
const char pgmCfgScreenLandscape[] PROGMEM = "screenLandscape";
const char pgmCfgSleepTime[] PROGMEM = "sleepTime";
const char pgmCfgTimestamp[] PROGMEM = "timestamp";

const char pgmCfgWifiNetworkFallback[] PROGMEM = "wifiNetworkFallback";
const char pgmCfgWifiPasswordFallback[] PROGMEM = "wifiPasswordFallback";

const char pgmCfgImagePreset[] PROGMEM = "imagePreset";

const char pgmCfgExtSensorLink[] PROGMEM = "extSensorLink";
const char pgmCfgExtSensorLogin[] PROGMEM = "extSensorLogin";
const char pgmCfgExtSensorPassword[] PROGMEM = "extSensorPassword";

const char pgmCfgMqttHAPrefix[] PROGMEM = "mqttHAPrefix"; 
const char pgmCfgMqttHADevice[] PROGMEM = "mqttHADevice"; 
const char pgmCfgSyncEvery[] PROGMEM = "syncEvery";
const char pgmCfgTimeFormat12[] PROGMEM = "timeFormat12";

const char pgmCfgUpdateMinutes[] PROGMEM = "updateMinutes";
const char pgmCfgTitleIndoor[] PROGMEM = "titleIndoor";
const char pgmCfgTitleExternal[] PROGMEM = "titleExternal";

const cfgOption cfgOptions[] = {
    {cWifiNetwork, ctypeString, false, pgmCfgWifiNetwork, true},
    {cWifiPassword, ctypeString, true, pgmCfgWifiPassword, true},
    {cTimezone, ctypeString, false, pgmCfgTimezone, true},
    {cTempOffset, ctypeFloat, false, pgmCfgTempOffset, false},

    {cModeDuration, ctypeInt, false, pgmCfgModeDuration, false},
    {cModeList, ctypeString, false, pgmCfgModeList, false},
    {cModeListEnabled, ctypeString, false, pgmCfgModeListEnabled, false},

    {cNtpHosts, ctypeString, false, pgmCfgNtpHosts, true},
    
    {cMqttHost, ctypeString, false, pgmCfgMqttHost, true},
    {cMqttPort, ctypeInt, false, pgmCfgMqttPort, true},
    {cMqttLogin, ctypeString, false, pgmCfgMqttLogin, true},
    {cMqttPassword, ctypeString, true, pgmCfgMqttPassword, true},
    {cToFahrenheit, ctypeBool, false, pgmCfgToFahrenheit, false},
    {cMqttPrefixIn, ctypeString, false, pgmCfgMqttPrefixIn, false},  // Domoticz Prefix
    {cMqttDevicesIds, ctypeString, false, pgmCfgMqttDevicesIds, false},  // Domoticz device ids ( T&H&B, T&H, T )

    {cScreenRotate, ctypeBool, false, pgmCfgScreenRotate, false},
    {cScreenLandscape, ctypeBool, false, pgmCfgScreenLandscape, false},
    {cSleepTime, ctypeInt, false, pgmCfgSleepTime, false},
    {cTimestamp, ctypeString, false, pgmCfgTimestamp, false},

    {cWifiNetworkFallback, ctypeString, false, pgmCfgWifiNetworkFallback, false},
    {cWifiPasswordFallback, ctypeString, true, pgmCfgWifiPasswordFallback, false},

    {cImagePreset, ctypeString, false, pgmCfgImagePreset, false},

    {cExtSensorLink, ctypeString, false, pgmCfgExtSensorLink, false},
    {cExtSensorLogin, ctypeString, false, pgmCfgExtSensorLogin, false},
    {cExtSensorPassword, ctypeString, true, pgmCfgExtSensorPassword, false},

    {cMqttHAPrefix, ctypeString, false, pgmCfgMqttHAPrefix, false},   // Home Assistant prefix
    {cMqttHADevice, ctypeString, false, pgmCfgMqttHADevice, false},   // Home Assistant device description (Id, Name)
    {cSyncEvery, ctypeString, false, pgmCfgSyncEvery, false},
    {cTimeFormat12, ctypeBool, false, pgmCfgTimeFormat12, false},

    {cUpdateMinutes, ctypeInt, false, pgmCfgUpdateMinutes, false},
    {cTitleIndoor, ctypeString, false, pgmCfgTitleIndoor, false},
    {cTitleExternal, ctypeString, false, pgmCfgTitleExternal, false},
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
    {17, uiPressureRemote}, 
    {18, uiSCD4XCO2},
    {19, uiSCD4XTemp},
    {20, uiSCD4XHum},
    {21, uiCalendar},
};

const size_t widgetsDefaultsSize PROGMEM = sizeof(widgetsDefaults) / sizeof(widgetsDefaults[0]);
