#if !defined(EnvCOnfigTypesH)
#define EnvCOnfigTypesH

enum cfgOptionType { ctypeString, ctypeBool, ctypeInt, ctypeFloat};

enum cfgOptionKeys {

    cWifiNetwork,
    cWifiPassword,
    cTimezone,
    cTempOffset,

    cModeDuration,
    cModeList, 
    cModeListEnabled,
        
    cNtpHosts,
    
    cMqttHost,
    cMqttPort,
    cMqttLogin,
    cMqttPassword,
    cToFahrenheit,
    cMqttPrefixIn,
    cMqttDevicesIds,

    cScreenRotate,
    cScreenLandscape, 
    cSleepTime, 
    cTimestamp,

    cWifiNetworkFallback,
    cWifiPasswordFallback,
    
    cImagePreset, 

    cExtSensorLink, 
    cExtSensorLogin,
    cExtSensorPassword,

    cMqttHAPrefix,
    cMqttHADevice,
    cSyncEvery,
    cTimeFormat12,
    cUpdateMinutes,

    cNull,
};

typedef struct {
  cfgOptionKeys key;
  cfgOptionType type;
  bool secured;
  const char * keyStr;
  bool rebootRequired;
} cfgOption;

#endif