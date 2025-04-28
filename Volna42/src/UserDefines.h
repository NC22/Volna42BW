#ifndef CfgDefines_h
#define CfgDefines_h

#if defined(ESP32)

#define DEFAULT_I2C_SCL SCL // I2C SCL [ESP32] Can be assigned any unused pin
#define DEFAULT_I2C_SDA SDA // I2C SDA [ESP32] Can be assigned any unused pin

#define DEEPSLEEP_MEMORY 1  // 1 - Использовать RTC (энергозависимая память) при уходе в режим глубокого сна, 2 - Использовать NVS память (постоянная память - рекомендуется использовать если есть проблемы с RTC)

#define BATTERY_SENSOR_PIN -1 // аналоговый пин для замеров уровня заряда батареи -1 - без батареи

// Делитель (BATTERY_R1V и BATTERY_R2GND) подбирается исходя из верхнего порога чтения V аналогового входа вашей ESP32 (зависит от модели) и максимального V источника питания (4.2v)
// Разные ESP32 имеют разный порог чтения V. Из тех что проверял - ESP32-S3 SuperMini понимает входное напряжение до 3.1v, ESP-C3 Zero ~2.5v, ESP-S2 Mini ~2.6v
// Вы можете найти документацию по своей плате или провести тест используя Serial.println(analogRead(BATTERY_SENSOR_PIN)) для разных входных напряжений и вычислить соответствие V для значения 4095
#define BATTERY_R1V 32.6f            
#define BATTERY_R2GND 47.2f
#define BATTERY_INPUT_MAXRANGE 4095.0 // максимально возможное значение в условных единицах analogRead(BATTERY_SENSOR_PIN)
#define BATTERY_INPUT_MAXRANGEV 2.6   // соответствующее условному значению напряжение на входе BATTERY_SENSOR_PIN

// #define WIFI_TX_POWER 40    // Переопределить мощность wifi датчика поумолчанию (в случае с проблемами с подключением к сети)

#else

#define DEFAULT_I2C_SCL 5 // I2C SCL [ESP8266] dont edit, no variations
#define DEFAULT_I2C_SDA 4 // I2C SDA [ESP8266] dont edit, no variations


// Workaround wakeup from deepsleep issue - https://github.com/esp8266/Arduino/issues/6318
// Решение для исправления проблемы дешевых клонов плат ESP8266 D1 Mini с зависанием при выходе из сна 
#define FIX_DEEPSLEEP   0                     // 0 - OFF, 1 - первый метод - непроверен \ first method - untested, 2 - второй, проверен в долгих тестах \ second, tested in long tests
#define BATTERY_SENSOR_PIN 0
#endif

#define PRESSURE_HPA false                    // pressure in hPa (default - mmHg - мм.рт.ст)
#define PARTIAL_UPDATE_INTERVAL 120           // Интервал частичного обновления экрана по умолчанию (если поддерживается) в секундах для обновления часов - полностью экран рекомендуют обновлять не чаще 1 раза в 3 минуты (не должно превышать период полных обновления экрана -- sleepTime)

// [Main temperature sensor] | [Внутренний сенсор температуры]

#define INTERNAL_SENSOR_BME280   true         // BME280, подхватывается по I2C 
// #define INTERNAL_SENSOR_DS18B20  2         // DS18B20 - сенсор температуры - указать пин подключения (2 - GPIO2 - D4). если используется, добавить библиотеку в platformio.ini из типовых конфигов, если BME280 так же присутствует на I2C шине то c него будут взяты Hum & Pressure

// [Addition internal sensors] | [Дополнительные внутренние датчики]

// Поддержка датчика CO2 - SCD41 (Вывод данных через виджеты в кастомном интерфейсе и веб-панели)
// #define CO2_SCD41  // если используется, добавить библиотеку в platformio.ini из типовых конфигов ~требует +3-4кб оперативки (может не хватать памяти для некоторых действий в web-ui если используется 2-bit графика на ESP8266)

// [Language] | [Язык интерфейса] - заголовки "улица" \ "дом" вынесен в настройки веб-интерфейса в раздел "Общие настройки"

#define LOCALE_RU  // Русский
// #define LOCALE_JA  // Japan
// #define LOCALE_EN  // English
// #define LOCALE_DE  // German

// [Display types] | [Типы дисплеев]

// #define WAVESHARE_BW_42_UC8176       // Waveshare 4.2inch, REV 2.1 [B&W]
#define WAVESHARE_BW_42_SSD1683         // Waveshare 4.2inch, REV 2.2 | WeAct 4.2inch [B&W, 4-colors grayscale displays]
// #define WAVESHARE_RY_BW_42_UC8176    // Waveshare 4.2inch, REV 2.1 [B&W + Red or B&W + Yellow, 3-colors (separate buffers)]
// #define WAVESHARE_RY_BW_42_UC8176_B  // Waveshare 4.2inch, REV ??? - если при выводе WAVESHARE_RY_BW_42_UC8176 инвертирует цвета (фон красным \ отдельные элементы белым \ черным)
// #define HELTEC_BW_15_S810F           // SSD1683 Heltec 1.54inch 200x200 or WeAct 1.54 200x200 [B&W]
// #define WAVESHARE_R_BW_15_SSD1683    // Waveshare 1.54inch 200x200 [B&W + Red, 3-colors (separate buffers)]

// [Battery sensor mode] | [Режим чтения показаний заряда батареи]

// #define BAT_NO                 // Disable battery sensor - always ON | Выключить чтение показаний батареи - не засыпать
// #define BAT_ADS1115            // Read V by ADS1115 module | Чтение V батареи через I2C модуль ADS1115 [Input = A1, R1V=BATTERY_R1V, R2GND=BATTERY_R2GND, Env::readBatteryV()]
#define BAT_A0 BATTERY_SENSOR_PIN // Read V by analog pin A0 (or by PIN N in ESP32) | Чтение V через аналоговый вход A0 (или через заданый пин в ESP32) [ESP8266, R1V=180kOm], [ESP32, R1V=BATTERY_R1V, R2GND=BATTERY_R2GND, Env::readBatteryV()]

#if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)
/*
	Пресет для дисплея 1.54 [200x200] - WeAct, Heltec, WaveShare 1.54' and compatible displays
    /api/clocktest - демо отображение полноэкранных часов с секундами
*/
	// #define DOUBLE_BUFFER_MODE   // часть элементов может быть окрашена в доп. цвет дисплея (Желтый или Красный), требовательно к оперативке [если поддерживается дисплеем]
	// #define PARTIAL_UPDATE_METHOD_INVERT 
	#define PARTIAL_UPDATE_SUPPORT  

  // ESP8266

  	#define EPD_RST_PIN   -1 // D4 - GPIO - 2 или -1 если у дисплея нет такого вывода
  	#define EPD_DC_PIN    0  // D3 - GPIO - 0
  	#define EPD_CS_PIN    15 // D8 - GPIO - 15 (CS) или опционально GND (-1)
  	#define EPD_BUSY_PIN  12 // D6 - GPIO - 12 (MISO)
	#define EPD_CLK_PIN   -1 // Всегда GPIO 14 (SCLK) - SPI  
	#define EPD_DIN_PIN   -1 // Всегда GPIO 13 (MOSI) - SPI
	

#elif defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176_B)
/*
	Пресет для дисплея 4.2' [400x300]  - WeAct, WaveShare 4.2' and compatible displays   
*/

	// [Partial screen update] | [Частичное обновление экрана]

	// B\W displays WeAct - worked & tested
	// B\W displays WaveShare rev 2.2 - worked & tested
	// B\W\Red displays WaveShare rev 2.1 - not work, revision (B) - work 
	// B\W\Yelow displays WaveShare rev 2.1 - kinda work, but possible artifacts
	// B\W displays WaveShare rev 2.1 - partial buffer push works, but no frame update availabe (orig examples also not works)

	// USE /api/partialtest link to test if partial update works on your display without issues, and comment defines if its not, to prevent unexpected behavior

	// #define PARTIAL_UPDATE_METHOD_INVERT      // только для SSD1683 4.2 - частичное обновление экрана через инвертирование области, а не через зачистку (может оставатся больше визуальных артифактов)
	#define PARTIAL_UPDATE_SUPPORT               // Partial update is configurable in web UI. Default value - cfgUpdateMinutes in UserSettings.h. Used for update clock widget each minute
	// #define RESET_MINUTE_TIMER_ON_WEB_REQUEST // Display updates could stuck web UI during screen refresh, so this can be usefull if you enter web interface frequently,

	#if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_RY_BW_42_UC8176_B)
		// #define DOUBLE_BUFFER_MODE      // часть элементов может быть окрашена в доп. цвет дисплея (Желтый или Красный), требовательно к оперативке 
	#endif

	#if defined(ESP32)

		// WEMOS LOLIN S2 Mini - протестировано, наиболее оптимальный вариант замены ESP8266 по размерам, функционалу
		// клон ESP32-S3-DevKitC - протестировано
		// В ESP32 практически нет ограничений на какие пины что можно назначать, в тестовой сборке работало с такой распиновкой, избегайте использования пинов SPI RAM (PSRAM), RX, TX, 0

		#define EPD_BUSY_PIN 4  
		#define EPD_RST_PIN  5 
		#define EPD_DC_PIN   6   
		#define EPD_CS_PIN   -1    // GND (-1)  
		#define EPD_CLK_PIN   15    
		#define EPD_DIN_PIN   7    

	#else
		
		// режим экономии оперативки при отрисовке 2-бит картинок, освобождается 15кб (нет возможности очищать пространство под виджетами - необходимо заранее подготовить картинку-фон)
		#define COLORMODE_2BIT_SUPPORT_RAM_FRIENDLY 

		// [https://42volna.com/scheme/] Основная распиновка, с освобождением ножки CS
	
		#define EPD_BUSY_PIN 12  // D6 - GPIO - 12 (MISO) 
		#define EPD_RST_PIN  15  // D8 - GPIO - 15 (CS)
		#define EPD_DC_PIN   0   // D3 - GPIO - 0
		#define EPD_CS_PIN   -1  // GND (-1)    
		#define EPD_CLK_PIN  -1  // Всегда D5 - GPIO 14 (SCLK) - SPI  
		#define EPD_DIN_PIN  -1  // Всегда D7 - GPIO 13 (MOSI) - SPI
	
		// [Optional, my old setup] | НЕ Основная
		// моя старая алт. распиновка, через резистор
	/*
		#define EPD_BUSY_PIN 2   // D4 - GPIO - 2 (+20кОм резистор на VCC)
		#define EPD_RST_PIN  12  // D6 - GPIO - 12 (MISO) 
		#define EPD_DC_PIN   0   // D3 - GPIO - 0
		#define EPD_CS_PIN   15  // D8 - GPIO - 15 (CS) или опционально GND (-1)
		#define EPD_CLK_PIN  -1  // Всегда D5 - GPIO 14 (SCLK) - SPI  
		#define EPD_DIN_PIN  -1  // Всегда D7 - GPIO 13 (MOSI) - SPI
	*/
	
	#endif

#endif

// [ 4.2' \ 1.54' Default interface ] [Оформление интерфейса по умолчанию]

#define DUI_CLOCK_FONT 2   // 1 - 44px, 2 - 54px, 3 - 64px
#define DUI_TECH_INFO true // Показывать техническую информацию мелким шрифтом (вольтаж батареи \ кол-во успешных синхронизаций с внешним датчиком, дата последней синх.) 
#define DUI_PRESSURE_HOME true  // Показывать давление вместо надписи "влажность" если датчик поддерживает 
#define DUI_PRESSURE_EXTERNAL true // тоже для улицы, если данные доступны
// #define DUI_PRESSURE_COPY_TO_EXT // наследовать данные давления с внутреннего датчика BME280 если данные улицы не доступны (не поддерживает измерение давления)
// #define DUI_CO2 // Показывать CO2 вместо надписи "влажность" в блоке "дом" если датчик есть (приоритетней DUI_PRESSURE_HOME если так же активно)

// [ 4.2' Default interface ] [Оформление интерфейса по умолчанию]

// #define DUI_CALENDAR	   // Показать календарь вместо часов (только для портретного режима 4.2')
// Определение дождливой \ снежной погоды по косвенным признакам (низкое давление + повышеная влажность)
// закомментить если не нужно, если данные с OpenWeather, то тоже не используется
#define ICON_RAIN_DETECT  

// облачно t >= 0 + [давление <= 1030hPa (~772 мм.рт.ст) + влажность >= 62%]		
#define ICON_RAIN_DETECT_CLOUDY_HPA 1030
#define ICON_RAIN_DETECT_CLOUDY_HUM 62

// идет дождь t >= 0 + [давление <= 1009hPa (~757 мм.рт.ст) + влажность >= 70%]		
#define ICON_RAIN_DETECT_RAINY_HPA 1009  	   
#define ICON_RAIN_DETECT_RAINY_HUM 70 

// облачно t < 0 + [давление <= 1030hPa (~772 мм.рт.ст) + влажность >= 64%]		
#define ICON_SNOW_DETECT_CLOUDY_HPA 1030  	   
#define ICON_SNOW_DETECT_CLOUDY_HUM 64

// идет снег t < 0 + [давление <= 1030hPa (~772 мм.рт.ст) + влажность >= 68%]		
#define ICON_SNOW_DETECT_SNOW_HPA 1030  	   
#define ICON_SNOW_DETECT_SNOW_HUM 68

// [Other, system vars] [Разное, системное]

#define CUI_MAX_WIDGETS 20                    // Максимально возможное кол-во выводимых виджетов для кастомного интерфейса
#define CUI_LOOP_INTERVAL 14400               // Интервал смены оформления в режиме -loop - смена кастомных оформлений по порядку из того что загружено раз в 4 часа (тестовый функционал) 

#define EXTERNAL_SENSOR_CONNECT_ATTEMPTS 3    // Reconnect attempts on HTTP GET external data fail | Кол-во попыток подключения (HA & Domoticz & Openweather)
#define EXTERNAL_SENSOR_CONNECT_TIMEOUT 15000 // Connection timeout in milliseconds | Максимальное время ожидания ответа сервера (HA & Domoticz & Openweather) 
#define ENV_INDOOR_EXTERNAL_SUPPORT			  // Поддержка функции загрузки данных для внутреннего датчика из внешнего источника (опция для совместимости со старыми конфигами)
#define NTP_CONNECT_ATTEMPTS 3

#define BAT_MIN_V 3.25           // 0%
#define BAT_MAX_V 4.2            // 100%, low bat tick 10%

#define DEFAULT_TIME_BY_EXTERAL  // подгружать время по умолчанию с внешнего датчика если доступно. работает для Home Assistant (скип ожидания синхронизации с NTP)

// [Optional sleep switch] | [Опциональный тригер режима сна]
// По умолчанию переход в режим сна осуществляется всегда при наличии данных от сенсора уровня батареи, но можно вынести на отдельную кнопку \ переключатель
// Если установлен SLEEP_SWITCH_PIN, переход в режим сна осуществляется только если SLEEP_SWITCH_PIN в состоянии digitalRead = LOW, если параметр не установлен то уходим в сон если работаем от батареи (есть данные на сенсоре A0)

// #define SLEEP_SWITCH_PIN -1    // [ESP32] [ESP8266 - not enough pins, if 4.2' display used]

// #define SLEEP_ALWAYS_IGNORE    // [DEBUG OPTION] Ignore sleep mode. Even if work from battery | Игнорировать режим сна. Не засыпать, даже если работаем от батареи. 
// #define SLEEP_ALWAYS_SLEEP     // [DEBUG OPTION] Always go to sleep mode | Всегда уходить в режим сна 
#define SAFE_MODE false           // [DEBUG OPTION] игнорировать настройки сохраненные в EEPROM при запуске - на случай если по каким-то причинам конфиг вызывает зависания при запуске или иные проблемы

#endif	