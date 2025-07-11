// ftype - input field type - bool, action, text (default)

var ENVDATA = {
    
    cfg : {
              
      "_imageFeed" : null,
          
          "imageUpload": {"ftype" : "action", "location" : "/upload/image"},   
          "imagePreset": {"filter" : "string"},
      
      "_/imageFeed" : null,
      
      "_wifi" : null,
          
          "wifiNetwork": {"filter" : "string"},      
          "wifiPassword": {"secret" : true,},
          "wifiNetworkFallback": {"filter" : "string"},      
          "wifiPasswordFallback": {"secret" : true,},
      
      "_/wifi" : null,   
      "_extSensor" : null,
          
          "extSensorLink": {"filter" : "string"},      
          "extSensorLogin": {"filter" : "string"},
          "extSensorPassword": {"secret" : true,},    
      
      "_/extSensor" : null,
      "_general" : null,
      
          "ntpHosts": {"filter" : "string"},
          "timezone": {"filter" : "string"},
          "timestamp" : {"filter" : "string", "placeholder" : "2024-05-01 13:37:00"}, 
          "updateMinutes" : {"filter" : "string"},
          "tempOffset" : {"filter" : "string"},
          
          "humOffset" : {"filter" : "string"},
          "co2Offset" : {"filter" : "string"},
          "tempSource" : {"filter" : "string"},
          "humSource" : {"filter" : "string"},
          
          "sleepTime" : {"filter" : "string"},  
          "syncEvery" : {"filter" : "string"}, 
          "timeFormat12" : {"filter" : "string"},
		  "toFahrenheit" : {"filter" : "string"},
		  "titleIndoor" : {"filter" : "string"},
		  "titleExternal" : {"filter" : "string"},
		  
      "_/general" : null,  

      "_mqtt" : null,
          
          "mqttHost": {"filter" : "string"},      
          "mqttPort": {"filter" : "int" },
          "mqttLogin": {"filter" : "string"},      
          "mqttPassword": {"secret" : true, "filter" : "string"},

          "mqttSetDefaultsDomoticz": {"ftype" : "action"}, 
          "mqttSetDefaultsHA": {"ftype" : "action"}, 
		  
          "mqttPrefixIn": {"filter" : "string", "placeholder" : "domoticz/in" },
          "mqttDevicesIds": {"filter" : "string", "placeholder" : "31"},  
		  
          "mqttHAPrefix": {"filter" : "string", "placeholder" : "homeassistant" },
          "mqttHADevice": {"filter" : "string", "placeholder" : "kelly_einkscreen270524, Kelly E-Ink Screen"},    
		  
      "_/mqtt" : null,      
      "_screen" : null,
		  
          "screenRotate" : {"filter" : "int"},
          "screenLandscape" : {"filter" : "int"},
          "refresh" : {"ftype" : "action", "action" : "/api/update"},
          "clear" : {"ftype" : "action", "action" : "/api/clear"},
          "downloadBuffer1": {"ftype" : "action", "location" : "/api/buffer?id=buffer1"},
          "downloadBuffer2": {"ftype" : "action", "location" : "/api/buffer?id=buffer2"},

      "_/screen" : null,
      "_device" : null,
      
          "resetntp" : {"ftype" : "action", "action" : "/api/ntp/reset"},
          "reboot" : {"ftype" : "action"},
          "restore" : {"ftype" : "action"},

      "_/device" : null,
  },
  
   
};  