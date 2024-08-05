var ENVLOCALE = {
   
  "cfg__wifi" : "Доступ к WiFi",
  "cfg_wifiNetwork" : "Название сети (SSID)",
  "cfg_wifiPassword" : "Пароль",
  "cfg_wifiNetworkFallback" : "[Резервный] Название сети (SSID)",
  "cfg_wifiPasswordFallback" : "[Резервный] Пароль",
  "cfg__general" : "Общие настройки",
  "cfg__general_help" : "Формат поля даты и времени : 2024-05-01 13:37:00",
  "cfg_timezone" : "Часовой пояс",
  "cfg_toFahrenheit" : "Единицы измерения температуры (0 - Цельсий, 1 - Фаренгейт)",
  "cfg_timeFormat12" : "12-часовой формат (0-1)",
  "cfg_updateMinutes" : "Обновлять минуты (0-1)",
  "cfg_timestamp" : "Дата & Время",
  "cfg_tempOffset" : "Корректировка температуры (от -10℃ до 10℃)",  
  "cfg_ntpHosts" : "NTP сервер (off - не использовать)",
  
  "cfg__extSensor" : "Внешний датчик (Domoticz & Home Assistant)",      
  "cfg__extSensor_help" : "<b>DZ (Domoticz)</b><br>http:/" + "/[IP или домен сервера]:8080/json.htm?type=devices&rid=[ID устройства]<br>Опционально требуется доступ по логину и паролю, можно создать пользователя в разделе Настройка -> Users (/#/Users)<br><br><b>HA (Home Assistant)</b><br>http:/" + "/[IP или домен сервера]:8123/api/states/[ID устройства]<br>Токен можно получить в HA в разделе Безопасность (/profile/security) -> Долгосрочные токены доступа",
  "cfg_extSensorLink" : "Url-адрес",
  "cfg_extSensorLogin" : "Логин",
  "cfg_extSensorPassword" : "Пароль или Token",
  
  "cfg__mqtt" : "MQTT сервер",
  "cfg__mqtt_help" : "Порядок проверки настроек [HA] -> [DZ]<br>Заполните поля в зависимости от серверного ПО<br>Для Domoticz укажите Префикс IN : <b>domoticz/in</b> и создайте виртуальные сенсоры и укажите их id здесь.<br> Для HA<br>1. Укажите Discovery префикс для автообнаружения (обычно по умолчанию <b>homeassistant</b>)<br>2. Придумайте уникальный идентификатор устройства в системе и имя устройства. Например : <b>kelly_einkscreen270524, Kelly E-Ink Screen</b>",
  "cfg_mqttHost" : "Адрес сервера",
  "cfg_mqttPort" : "Порт",
  "cfg_mqttLogin" : "Логин",
  "cfg_mqttPassword" : "Пароль",
  "cfg_mqttPrefixIn" : "[DZ] Префикс Topic In",
  "cfg_mqttDevicesIds" : "[DZ] Id устройств (T&H&B, T&H, T)",
        
  "cfg_mqttHAPrefix": "[HA] Префикс Discovery",
  "cfg_mqttHADevice": "[HA] Device id, Device name",  
  
  "cfg_sleepTime" : "Глубокий сон экрана (1-30 минут)",
  "cfg_syncEvery" : "Синхронизация после n кол-ва пробуждений (1-30)",
  
  "cfg__screen" : "Управление экраном",
  "cfg_screenRotate" : "Перевернуть экран (0-1)",
  "cfg_screenLandscape" : "Ландшафтный режим (0-1)",
  
  "cfg__imageFeed" : "Свое оформление",
  "cfg_imagePreset" : "Пресет (-loop - по порядку)",
  
  "form_text_send" : "Отправить",   
  
  "help" : "Подсказка",
  "title" : "Метеостанция Волна 42 2BW42",
  "wifiSearch" : "Поиск WiFi сети",
  "save" : "Сохранить изменения",
  "save_skip" : "Без записи в постоянную память",
  
  "cfg__device" : "Управление устройством",
  
  "reboot" : "Перезагрузить",
  "restore" : "Сбросить настройки",    
  "refresh" : "Обновить экран",
  "clear" : "Очистить экран",
  
  "imageUpload" : "Загрузить свою картинку",
  
  "downloadBuffer1" : "Скачать основной буфер экрана",
  "downloadBuffer2" : "Скачать вспомогательный буфер",  
  
  "show_wifi_list" : "Показать список WiFi сетей",
  "reboot_ok" : "Перезагрузка выполняется",
  "command_ok" : "Команда отправлена успешно",
  "conn_lost" : "Устройство не доступно",
  
  "notifications_empty" : "Оповещений нет",
  
  "commit_reset_confirmed" : "Выполняю сброс настроек",
  "commit_reset_confirm" : "Настройки будут сброшены. Для подтверждения <b>нажмите на кнопку сброса еще раз</b>",
  "commit_reset_ok" : "Настройки сброшены",
  "commit_nothing" : "Нет изменений",
  "commit_save" : "Сохраняю изменения...",
  "commit_save_ok" : "Изменения сохранены.",      
  "commit_reboot_required" : "Требуется перезагрузка устройства",
  
  "getinfo_fail" : "Ошибка обновления",
  "getinfo_sensors_empty" : "Датчики не инициализированы",
  "getinfo_sensors" : "Датчики",
  "getinfo_sensor" : "датчик",
  "getinfo_sensor_pres" : "Давление",
  "getinfo_sensor_temp" : "Температура",
  "getinfo_sensor_hum" : "Влажность",
  "getinfo_sensor_co2" : "Углекислый газ (CO2)",
  "getinfo_sensor_loc" : "Орг. вещества TVOC (ЛОС)",
  "getinfo_wifi_lvl" : "Уровень сигнала",
  "getinfo_wifi_fail" : "Точка не доступна",
  "getinfo_wifis1" : "Сбой подключения",
  "getinfo_wifis2" : "Неправильный пароль",
  "getinfo_wifis3" : "Нет сети с указанным именем",
  
 
  "getinfo_external_sens_sync_times" : "Успешных подключений",
  "getinfo_external_sens_sync_last" : "Дата последней синхронизации",
  "getinfo_external_sens" : "Внешний датчик",
  "getinfo_bat_percent" : "Заряд",
  "getinfo_v" : "Напряжение",
  "getinfo_bat" : "Аккумулятор",
  "getinfo_connected" : "Подключено",
  "getinfo_not_connected" : "Не подключено",
  
  
  "notifications_duration_sec" : "сек.",
  "notifications_remove_process" : "Удаляю оповещение",
  
  
  "widget_uiClock" : "Дата & Время",
  "widget_uiBat" : "Состояние аккумулятора",
  "widget_uiBatRemote" : "[Внешний] Состояние аккумулятора",
  "widget_uiInfo" : "Доп. информация & Wifi",
  "widget_uiTemp" : "Температура",
  "widget_uiHum" : "Влажность",
  "widget_uiTempRemote" : "[Внешний] Температура",
  "widget_uiHumRemote" : "[Внешний] Влажность",
  
  "widget_uiInfoIP": "IP адрес (в режиме сервера)",
  "widget_uiInfoVoltage": "Напряжение аккумулятора",
  "widget_uiLastSyncRemote": "[Внешний] Дата последней синхронизации",
  "widget_uiInfoSyncNumRemote": "[Внешний] Успешных подключений",
  "widget_uiShortInfoSyncRemote": "[Внешний] Краткая информация",
  "widget_uiInfoMessage": "Подпись",
  
  "cui_restart_required" : "Перезагружаюсь для переинициализации буфера экрана!",
  "cui_done" : "Готово!",
  "cui_bytes_sended" : "байт отправлено...",
  "cui_device_is_not_responding" : "Устройство не отвечает",
  "cui_accept_widgets_fail" : "Ошибка на этапе инициализации настроек виджетов. Вывод на экран не выполнен",
  "cui_image_data_send_fail" : "Ошибка обработки пакетных данных на этапе отправки картинки. ",
  "cui_file_not_selected" : "Файл не выбран",
  "cui_parse_fail_json" : "Некорректная структура json файла",
  "cui_parse_fail_2bit" : "Экран не поддерживает 2-битную графику",
  "cui_parse_fail_proportions" : "Пропорции экрана сохраненные в json файле не соответствуют экрану устройства ",
  "cui_parse_fail_buffer_size" : "Некорректный размер файла буфера экрана. Возможно вам нужно поменять настройки битности графики. [Размер файла буфера \ Фактический размер буфера экрана дисплея] - ",
  "cui_parse_fail_image_read_error" : "Ошибка чтения изображения",
  
  "cui_upload_nothing" : "Сперва выберите готовое оформление или создайте на основе картинки",
  "cui_cancel" : "Процесс отменен",
  "cui_process" : "Отправляю данные",
  "cui_bytes" : "байт",
  
  "cui_general_options" : "▼ Основные настройки и картинка",
  "cui_image" : "Картинка:",
  "cui_resize_fitImage" : "Подогнать картинку под размеры экрана",
  "cui_resize_fitCover" : "По большей стороне (Cover)",
  "cui_resize_width" : "По ширине",
  "cui_resize_height" : "По высоте",
  "cui_resize_no" : "Вывести без изменений",
  
  
  "cui_convert_method" : "Метод преобразования",
  "cui_convert_mthreshhold" : "Пороговое значение",
  "cui_convert_invert_color" : "Инвертировать цвет картинки",
  "cui_convert_matkinson_auto" : "Ненастраиваемый дизеринг ",
  
  "cui_widget_options" : "▼ Настройки виджетов",
  "cui_saved_cui" : "▼ Сохраненное",
  "cui_send_to_screen_options" : "▼ Отправка на экран",
  "cui_download_preview" : "▼ Скачать предпросмотр",
  
  "cui_send_to_screen" : "Вывести на экран",
  "cui_preview" : "Предпросмотр",
  
  "mqttSetDefaultsDomoticz": "Настройки по умолчанию для Dmoticz", 
  "mqttSetDefaultsHA": "Настройки по умолчанию для Home Assistant", 
  
};