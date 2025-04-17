#ifndef Cfg_h
#define Cfg_h

/* 
    Volna 2BW42 firmware
    nradiowave | https://42volna.com/
*/

/* 
    Translation. Use defines in UserDefines.h for select language

    New translations is also welcome, but can require additionaly convert font with proper set of symbols
    Actual version of font converter tool can be found here https://42volna.com/tools/fontconverter

    ui/out/locale_[name] files is autogenerated from _uiTools\uiEink\locale\*.js locale files
*/

#if defined(LOCALE_RU)
// [Russian]
#include <locale/LocaleRu.h>
#include <ui/out/locale_ru/locale.h>
#elif defined(LOCALE_EN)
// [English]
#include <locale/LocaleEn.h>
#include <ui/out/locale_en/locale.h>
#elif defined(LOCALE_JA)
// [Japan]
#include <locale/LocaleJa.h>
#include <ui/out/locale_en/locale.h>
#elif defined(LOCALE_DE)
// [German]
#include <locale/LocaleDe.h>
#include <ui/out/locale_en/locale.h>
#endif

const char productVersion[] PROGMEM = "0.95";

// [Режим точки доступа в случае отсутствия возможности подключится к Wifi]

// WiFi AP, default web-interface address - http://192.168.1.1/
const char defaultWifiAP[] PROGMEM = "VOLNA42-EINK";  // SSIDs can reach up to 32 characters in length
const char defaultWifiAPP[] PROGMEM = "volnaaccess";  // A Valid Password is 8 - 63 Characters long


const char shortDateFormat[] PROGMEM = "%d.%m.%y";  // date & time string date part format for 1.54'
const char longDateFormat[] PROGMEM = "%d.%m.%y";   // Date string format (2-string of [Clock & Date widget] used in 4.2')

/*
 Short text date format : 
 
 0 - [15 August, Thu], 
 1 - [August 15, Thu] (3-string of [Clock & Date widget] used in 4.2')
 2 - [8月15日(木)]
*/

#if defined(LOCALE_JA)
    const uint8_t textDateFormat PROGMEM = 2; 
#elif defined(LOCALE_RU)
    const uint8_t textDateFormat PROGMEM = 0; 
#else
    const uint8_t textDateFormat PROGMEM = 1; 
#endif

// [Config defaults] | [Настройки по умолчанию]

const uint8_t cfgCelsius PROGMEM = 1;                  // 0-1 | Use Celsius instead of Fahrenheit by default
const uint8_t cfg12HourFormat PROGMEM = 0;             // 0-1 | Use 12 hour PM / AM clock format by default 
const uint8_t cfgUpdateMinutes PROGMEM = 0;            // 0-1 or number of seconds | Update clock widget interval (if = 1, PARTIAL_UPDATE_INTERVAL will be used as defaul update interval time)

const char cdTimezone[] PROGMEM = "MSK-3";             // timezone - list https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
const char cdNtpHosts[] PROGMEM = "pool.ntp.org";      // NTP сервер - старайтесь не отправлять слишком много запросов, в случае если есть проблемы с доступом к серверу, найдите другой или задайте значение - off, очистить поле timezone, в таком случае время можно определить вручную через панель управления
const char cfgWifiSSID[] PROGMEM = "";                 // Default WiFi network name to connect to (you can setup it from web interface by connecting to default AP)
const char cfgWifiP[] PROGMEM = "";                    // Default Wifi password

const char cfgOTALogin[] PROGMEM = "admin";            // Логин и пароль для входа на страницу загрузки прошивки через веб интерфейс (/update)
const char cfgOTAPassword[] PROGMEM = "admin";

const char cfgExtSensor[] PROGMEM = "";                // API ссылка на внешний датчик (Home Assistant или Domoticz) - например типовой формат для Domoticz - http://127.0.0.1:8080/json.htm?type=devices&rid=[ID устройства] и для HA - http://127.0.0.1:8123/api/states/[ID устройства]
const char cfgExtSensorL[] PROGMEM = "";               // Логин (Domoticz) - требуется в зависимости от настроек Domoticz
const char cfgExtSensorP[] PROGMEM = "";               // Пароль (Domoticz) или Токен авторизации (Home Assistant)

const uint8_t cfgExtLocal PROGMEM = 0;                 // [Удаленные данные вместо встроенного датчика BME280] - Использовать внешние данные для домашнего датчика (тестовый \ эксперементальный метод)  
const char cfgExtSensorLocal[] PROGMEM = "";           // [Домашний датчик - опционально] API ссылка на внешний датчик
const char cfgExtSensorLocalL[] PROGMEM = "";          // [Домашний датчик - опционально] Логин - требуется в зависимости от настроек
const char cfgExtSensorLocalP[] PROGMEM = "";          // [Домашний датчик - опционально] Пароль или Токен авторизации

// [MQTT server]
const char cfgMqttHost[] PROGMEM = "";
const char cfgMqttLogin[] PROGMEM = "";
const char cfgMqttPassword[] PROGMEM = "";
const char cfgMqttClientIdPrefix[] PROGMEM = "ESP8266ClockClient-";  // Префикс для динамического идентификатора сессии подключения к MQTT серверу, можно поменять если много однотипных устройств

// Два подключения одновременно не поддерживается. Настройки проверяются по порядку Home Assistant -> Domoticz 
// [Home Assistant]
const char cfgMqttHAPrefixDiscovery[] PROGMEM = "";            // usually "homeassistant"
const char cfgMqttHADevice[] PROGMEM = "";                     // any unique device id and name. Exmp. "kelly_einkscreen270524, Kelly E-Ink Screen"

// [Domoticz]
const char cfgMqttDomoticzPrefix[] PROGMEM = "";               // usually "domoticz/in"
const char cfgMqttDomoticzDeviceIds[] PROGMEM = "";            // numeric ids from Domoticz controll panel

const unsigned int cdConfigVersion PROGMEM = 1013;             // [1003-20000] Увеличить значение для сброса кэша веб-версии \ корректного обновления без полной перезагрузки

#endif    