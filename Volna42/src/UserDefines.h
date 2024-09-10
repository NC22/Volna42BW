#ifndef CfgDefines_h
#define CfgDefines_h

#if defined(ESP32)
#define DEFAULT_I2C_SCL 41 // I2C SCL [ESP32] Can be assigned any unused pin
#define DEFAULT_I2C_SDA 42 // I2C SDA [ESP32] Can be assigned any unused pin
#else
#define DEFAULT_I2C_SCL 5 // I2C SCL [ESP8266] dont edit, no variations
#define DEFAULT_I2C_SDA 4 // I2C SDA [ESP8266] dont edit, no variations
#endif

#define CUI_MAX_WIDGETS 20				   // Максимально возможное кол-во выводимых виджетов для кастомного интерфейса
#define CUI_LOOP_INTERVAL 14400            // Интервал смены оформления в режиме -loop - смена кастомных оформлений по порядку из того что загружено раз в 4 часа (тестовый функционал) 

#define PRESSURE_HPA false   // pressure in hPa (default - mmHg - мм.рт.ст)
// #define SAFE_MODE // игнорировать настройки сохраненные в EEPROM при запуске - на случай если по каким-то причинам конфиг вызывает зависания при запуске или иные проблемы
#define EXTERNAL_SENSOR_CONNECT_ATTEMPTS 3 // Reconnect attempts on HTTP GET external data fail | Кол-во попыток подключения (HA & Domoticz & Openweather)
#define EXTERNAL_SENSOR_CONNECT_TIMEOUT 5000 // Connection timeout in milliseconds | Максимальное время ожидания ответа сервера (HA & Domoticz & Openweather) 

// Поддержка датчика CO2 - SCD41 или аналогов. (Вывод данных через виджеты в кастомном интерфейсе и веб-панели)
// #define CO2_SCD41  // если используется, расскоментить библиотеку в platformio.ini

// [Language] - заголовки "улица" \ "дом" вынесен в настройки веб-интерфейса в раздел "Общие настройки"

#define LOCALE_RU  // Русский
// #define LOCALE_JA  // Japan
// #define LOCALE_EN  // English
// #define LOCALE_DE  // German

// [Display types] | [Типы дисплеев]

// #define WAVESHARE_BW_42_UC8176          // Waveshare 4.2inch, REV 2.1 [B&W]
#define WAVESHARE_BW_42_SSD1683            // Waveshare 4.2inch, REV 2.2 | WeAct 4.2inch [B&W, 4-colors grayscale displays]
// #define WAVESHARE_RY_BW_42_UC8176       // Waveshare 4.2inch, REV 2.1 [B&W + Red or B&W + Yellow, 3-colors (separate buffers)]
// #define HELTEC_BW_15_S810F              // Heltec 1.54inch 200x200 [B&W]

// [Battery sensor mode] | [Режим чтения показаний заряда батареи]

// #define BAT_NO                // Disable battery sensor - always ON | Выключить чтение показаний батареи - не засыпать
// #define BAT_ADS1115           // Read V by ADS1115 module | Чтение V батареи через I2C модуль ADS1115 [Input = A1, R1V=50.7kOm, R2GND=99.26kOm, Env::readBatteryV()]
#define BAT_A0 0                 // Read V by analog pin A0 (or by PIN N in ESP32) | Чтение V через аналоговый вход A0 (или через заданый пин в ESP32) [ESP8266, R1V=180kOm], [ESP32, R1V=50.7kOm, R2GND=99.26kOm, Env::readBatteryV()]

// [Optional sleep switch] | [Опциональный тригер режима сна]
// По умолчанию переход в режим сна осуществляется всегда при наличии данных от сенсора уровня батареи, но можно вынести на отдельную кнопку \ переключатель
// Если установлен SLEEP_SWITCH_PIN, переход в режим сна осуществляется только если SLEEP_SWITCH_PIN в состоянии digitalRead = LOW

// #define SLEEP_SWITCH_PIN -1    // [ESP32] [ESP8266 - not enough pins, if 4.2' display used]
// #define SLEEP_ALWAYS_ON        // Ignore sleep mode. Even if work from battery | Игнорировать режим сна. Не засыпать, даже если работаем от батареи. 

#if defined(HELTEC_BW_15_S810F)
/*
	[Дисплей 1.5'] Тестовый сетап
    /api/clocktest - демо отображение полноэкранных часов с секундами
*/
	#define PARTIAL_UPDATE_INTERVAL 120        
	#define PARTIAL_UPDATE_SUPPORT  
	#define COLORMODE_2BIT_SUPPORT  // Allow change image mode (1bit - 2-colors \ 2-bit 4-colors) from web panel if display supports 2-bit output 

  // [Deprecated, Dont use] Не используется - сейчас везде порядок вывода пикселей инициализируется командами на уровне контроллера
  // #define WEB_BUFFER_MOD_FLIPX  // для кастомных картинок экрана - приведение к единому начальному положению интерфейса и чистого буфера, для тестов ENVDATA.screen.bufferMod = "flipx";
  // #define WEB_BUFFER_MOD_FLIPX
  // #define WEB_BUFFER_MOD_FLIPXY

  // ESP8266

  	#define EPD_RST_PIN   -1 // D4 - GPIO - 2 или -1 если у дисплея нет такого вывода
  	#define EPD_DC_PIN    0  // D3 - GPIO - 0
  	#define EPD_CS_PIN    15 // D8 - GPIO - 15 (CS) или опционально GND (-1)
  	#define EPD_BUSY_PIN  12 // D6 - GPIO - 12 (MISO)
	#define EPD_CLK_PIN   -1 // Всегда GPIO 13 (MOSI) - SPI
	#define EPD_DIN_PIN   -1 // Всегда GPIO 14 (SCLK) - SPI  


#elif defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683)

/*
	[Дисплеи 4.2']  
		
	Выводы на плате Waveshare 4.2:
   
    [-1----2---3--------4-------------5-----------------6---------7---8--]
    [BUSY|RST|DC|CS (SPI - CS)|CLK (SPI - SCLK)|DIN (SPI - MOSI)|GND|3.3V]
   
*/

	// [Partial screen update] | [Частичное обновление экрана]

	// B\W displays WeAct - worked & tested
	// B\W displays WaveShare rev 2.2 - worked & tested
	// B\W\Red displays WaveShare rev 2.1 - kinda work, but possible artifacts
	// B\W displays WaveShare rev 2.1 - partial buffer push works, but no frame update availabe (orig examples also not works)

	// USE /api/partialtest link to test if partial update works on your display without issues, and comment defines if its not, to prevent unexpected behavior

	#define PARTIAL_UPDATE_INTERVAL 120          // Интервал частичного обновления экрана в секундах для обновления часов - полностью экран рекомендуют обновлять не чаще 1 раза в 3 минуты (не должно превышать период полных обновления экрана -- sleepTime)
	#define PARTIAL_UPDATE_SUPPORT               // Partial update is configurable in web UI and disabled by default. Used for update clock widget each minute
	// #define RESET_MINUTE_TIMER_ON_WEB_REQUEST // Display updates could stuck web UI during screen refresh, so this can be usefull if you enter web interface frequently,
	#define COLORMODE_2BIT_SUPPORT               // Allow change image mode (1bit - 2-colors \ 2-bit 4-colors) from web panel if display supports 2-bit output 

	#if defined(WAVESHARE_RY_BW_42_UC8176)
		//  #define DOUBLE_BUFFER_MODE      // часть элементов может быть окрашена в доп. цвет дисплея (Желтый или Красный), требовательно к оперативке 
	#endif

	#if defined(ESP32)

		// https://docs.platformio.org/en/latest/boards/espressif32/esp32-s3-devkitc-1.html
		// ESP32 мало проверял, но вроде ок. Там практически нет ограничений на какие пины что можно назначать, в тестовой сборке работало с такой распиновкой

		#define EPD_BUSY_PIN 4  
		#define EPD_RST_PIN  5 
		#define EPD_DC_PIN   6   
		#define EPD_CS_PIN   -1    // GND (-1)  
		#define EPD_CLK_PIN   15    
		#define EPD_DIN_PIN   7   

	#else
	
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

// [ 4.2' Default interface ] [Оформление интерфейса по умолчанию]

#define DUI_TECH_INFO true // Показывать техническую информацию мелким шрифтом (вольтаж батареи \ кол-во успешных синхронизаций с внешним датчиком, дата последней синх.) 
#define DUI_PRESSURE_HOME false  // Показывать давление вместо надписи "влажность" если датчик поддерживает 
#define DUI_PRESSURE_EXTERNAL true // тоже для улицы, если данные доступны

// Определение дождливой погоды по косвенным признакам (низкое давление + повышеная влажность)
// закомментить если не нужно, если данные с OpenWeather, то тоже не используется
#define ICON_RAIN_DETECT  
// облачно, пока не задействовано
#define ICON_RAIN_DETECT_CLOUDY_HPA 1000   	   
#define ICON_RAIN_DETECT_CLOUDY_HUM 60 	
// идет дождь [давление <= 900hPa (712 мм.рт.ст) + влажность >= 60%]		
#define ICON_RAIN_DETECT_RAINY_HPA 950   	   
#define ICON_RAIN_DETECT_RAINY_HUM 60 

#endif	