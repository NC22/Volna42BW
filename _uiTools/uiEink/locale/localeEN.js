var ENVLOCALE = {
  
  "cfg__wifi" : "Access to WiFi",
  "cfg_wifiNetwork" : "Network name (SSID)",
  "cfg_wifiPassword" : "Password",
  "cfg_wifiNetworkFallback" : "[Fallback] Network name (SSID)",
  "cfg_wifiPasswordFallback" : "[Fallback] Password",
  "cfg__general" : "General options",
  "cfg__general_help" : "Default date & time format : 2024-05-01 13:37:00",
  "cfg_timezone" : "Timezone",
  "cfg_toFahrenheit" : "Temperature units (0 - Celsius, 1 - Fahrenheit)",
  "cfg_timeFormat12" : "12-hours format (0-1)",
  "cfg_updateMinutes" : "Update minutes (0-1)",
  "cfg_timestamp" : "Date & Time",
  "cfg_tempOffset" : "Temperature offset (from -10℃ to 10℃)",
  "cfg_ntpHosts" : "NTP Server (off - for disable)",
  
  "cfg__extSensor" : "External sensor (Domoticz & Home Assistant)",      
  "cfg__extSensor_help" : "<b>DZ (Domoticz)</b><br>http:/" + "/[IP or domain name]:8080/json.htm?type=devices&rid=[device id]<br> Optionaly user login and password required, you can create user in DZ Options -> Users (/#/Users)<br><br><b>HA (Home Assistant)</b><br>http:/" + "/[IP or domain name]:8123/api/states/[device id]<br>Token can be created in HA Security section (/profile/security) -> Long access token",
  "cfg_extSensorLink" : "Link",
  "cfg_extSensorLogin" : "Login",
  "cfg_extSensorPassword" : "Password",
  
  "cfg__mqtt" : "MQTT Server",
  "cfg__mqtt_help" : "Config checked in order [HA] -> [DZ]<br>Complete fields depends on what server side software you use.<br>For Domoticz Type IN prefix : <b>domoticz/in</b> and create virtual devices, specify they ids here<br>For HA <br>1. Type Discovery prefix for autodetection (usually default is <b>homeassistant</b>)<br>2. Use unique device Id and Name, for ex. : <b>kelly_einkscreen270524, Kelly E-Ink Screen</b>",
  "cfg_mqttHost" : "Address",
  "cfg_mqttPort" : "Port",
  "cfg_mqttLogin" : "Login",
  "cfg_mqttPassword" : "Password",
  "cfg_mqttPrefixIn" : "[DZ] Prefix Topic In",
  "cfg_mqttDevicesIds" : "[DZ] Id list devices (T&H&B, T&H, T)",
  
  "cfg_mqttHAPrefix": "[HA] Prefix",
  "cfg_mqttHADevice": "[HA] Device id, Device name",    
      
  "cfg_sleepTime" : "Deep sleep time (1-30 minutes)",      
  "cfg_syncEvery" : "Sync external data every n wakeups (1-30)",
  
  "cfg__screen" : "Screen controll",
  "cfg_screenRotate" : "Screen flip",
  "cfg_screenLandscape" : "Landscape mode (0-1)",
  
  "cfg__imageFeed" : "Custom display",
  "cfg_imagePreset" : "Preset (manual, random, 1-10)",
        
  "form_text_send" : "Send",
  
  "help" : "Tip",
  "title" : "Metestation Volna 42 2BW42",
  "wifiSearch" : "Network search",
  "save" : "Save changes",
  "save_skip" : "Dont save to constant memory",
  
  "cfg__device" : "Device control",
  
  "reboot" : "Restart device",
  "restore" : "Restore defaults",  
  "refresh" : "Refresh screen",
  "clear" : "Clear screen",
  
  "imageUpload" : "Upload custom image",
  
  "downloadBuffer1" : "Download main buffer",
  "downloadBuffer2" : "Download second buffer",
  
  // todo - move to clock - common for E ink & clock
  
  "show_wifi_list" : "Show WiFi networks list",
  "reboot_ok" : "Reboot in progress",
  "command_ok" : "Command successfully sended",
  "conn_lost" : "Device not responding",
  
  "notifications_empty" : "Notifications empty",
  
  "commit_reset_confirmed" : "Reset in progress",
  "commit_reset_confirm" : "Options will be setted to default. Confirm by clicking <b>reset button again</b>",
  "commit_reset_ok" : "Options is reseted",
  "commit_nothing" : "No changes",
  "commit_save" : "Save changes...",
  "commit_save_ok" : "Saved. ",      
  "commit_reboot_required" : "Reboot device is required",
  
  "getinfo_fail" : "Get info fail",
  "getinfo_sensors_empty" : "No any sensors detected",
  "getinfo_sensors" : "Sensors",
  "getinfo_sensor" : "sensor",
  "getinfo_sensor_pres" : "Pressure",
  "getinfo_sensor_temp" : "Temperature",
  "getinfo_sensor_hum" : "Humidity",
  "getinfo_sensor_co2" : "Carbon dioxide (CO2)",
  "getinfo_sensor_loc" : "VOC (volatile organic compounds)",
  "getinfo_wifi_lvl" : "Signal level",
  "getinfo_wifi_fail" : "Access point not found",
  "getinfo_wifis1" : "WiFi connection fail",
  "getinfo_wifis2" : "Wrong password",
  "getinfo_wifis3" : "WiFi Network with specified name not found",    


  "getinfo_external_sens_sync_times" : "Succesfull conections",
  "getinfo_external_sens_sync_last" : "Last sync time",
  "getinfo_external_sens" : "External sensor",
  "getinfo_bat_percent" : "Battery percentage",
  "getinfo_v" : "Voltage",
  "getinfo_bat" : "Battery",
  "getinfo_connected" : "Connected",
  "getinfo_not_connected" : "Not connected",
      
  "notifications_duration_sec" : "sec.",
  "notifications_remove_process" : "Removing notification",      
  
  "widget_uiClock": "Date & Time",
  "widget_uiBat": "Battery",
  "widget_uiBatRemote": "Battery (External Sensor)",
  "widget_uiInfo": "Additional Information & Wifi",
  "widget_uiTemp": "Temperature",
  "widget_uiHum": "Humidity",
  "widget_uiTempRemote": "[External] Temperature",
  "widget_uiHumRemote": "[External] Humidity",
  
  "widget_uiInfoIP": "IP address info (in constant mode)",
  "widget_uiInfoVoltage": "Battery voltage",
  "widget_uiLastSyncRemote": "[External] Last successfull sync date",
  "widget_uiInfoSyncNumRemote": "[External] Connect sessions times",
  "widget_uiShortInfoSyncRemote": "[External] Short info",
  "widget_uiInfoMessage": "Sign",
  
  "cui_restart_required": "Restarting to reinitialize screen buffer!",
  "cui_done": "Done!",
  "cui_bytes_sended": "bytes sent...",
  "cui_device_is_not_responding": "Device is not responding",
  "cui_accept_widgets_fail": "Error during widget settings initialization. Display output not executed",
  "cui_image_data_send_fail": "Packet data processing error during image sending.",
  "cui_file_not_selected": "File not selected",
  "cui_parse_fail_json": "Invalid JSON file structure",
  "cui_parse_fail_2bit": "Screen does not support 2-bit graphics",
  "cui_parse_fail_proportions": "Screen proportions saved in the JSON file do not match the device screen",
  "cui_parse_fail_buffer_size": "Incorrect screen buffer file size. You might need to change the graphics bit settings. [Buffer file size \ Actual screen buffer size] - ",
  "cui_parse_fail_image_read_error": "Image read error",
  
  "cui_upload_nothing": "First select a ready-made layout or create one based on an image",
  "cui_cancel": "Process canceled",
  "cui_process": "Sending data",
  "cui_bytes": "bytes",
  
  "cui_general_options": "▼ General Settings and Image",
  "cui_image": "Image:",
  "cui_resize_fitImage": "Fit image to screen size",
  "cui_resize_fitCover": "Fit to larger side (Cover)",
  "cui_resize_width": "By width",
  "cui_resize_height": "By height",
  "cui_resize_no": "Display without changes",
  
  "cui_convert_method": "Conversion Method",
  "cui_convert_mthreshhold": "Threshold value",
  "cui_convert_invert_color": "Invert image color",
  "cui_convert_matkinson_auto": "Non-adjustable dithering",
  
  "cui_widget_options": "▼ Widget Settings",
  "cui_saved_cui": "▼ Saved",
  "cui_send_to_screen_options": "▼ Send to Screen",
  "cui_download_preview" : "▼ Download preview",
  
  "cui_send_to_screen": "Display on Screen",
  "cui_preview": "Preview",
  
  "mqttSetDefaultsDomoticz": "Default options for Dmoticz", 
  "mqttSetDefaultsHA": "Default options for Home Assistant", 
};