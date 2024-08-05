#ifndef CfgDefines_h
#define CfgDefines_h


// Default pins for ESP8266 I2C (Temperature & Hum sensor and additions), change for ESP32 if needed

#if defined(ESP32)
#define DEFAULT_I2C_SCL 41
#define DEFAULT_I2C_SDA 42
#else
#define DEFAULT_I2C_SCL 5
#define DEFAULT_I2C_SDA 4
#endif

#define CUI_LOOP_INTERVAL 14400            // Интервал смены оформления в режиме -loop - смена кастомных оформлений по порядку из того что загружено раз в 4 часа (тестовый функционал) 

// [Типы дисплеев]

// #define WAVESHARE_BW_42_UC8176          // Waveshare 4.2inch B&W only. Default revision boards <= 2.1
#define WAVESHARE_BW_42_SSD1683            // Новые платы с revision >= 2.2, поддерживают вывод 2-bitных картинок (4 цвета)
// #define WAVESHARE_RY_BW_42_UC8176       // Waveshare 4.2inch (B&W + Red or B&W + Yellow)
// #define HELTEC_BW_15_S810F              // Heltec 1.54inch 200x200 B&W display 

// [Режим чтения показаний заряда батареи]

// #define BAT_NO                // Не считывать данные батареи - всегда в режиме сервера \ без засыпания ESP 
// #define BAT_ADS1115           // Чтение аналогового значения напряжения батареи через I2C модуль ADS1115, активно не использовался, только в тестах. делитель R1V = 50.7 кОм  R2GND = 99.26 кОм - Env::readBatteryV()
#define BAT_A0 0                 // Чтение аналогового значения напряжения батареи через встроенный аналоговый вход ESP8266 (через резистор 180кОм) или любой аналоговый пин если это ESP32 (не проверено, заточено под делитель R1V = 50.7 кОм  R2GND = 99.26 кОм - см. Env::readBatteryV)

// [Опциональный триггер режима сна]
// По умолчанию переход в режим сна осуществляется всегда при наличии данных от сенсора уровня батареи, но можно вынести на отдельную кнопку \ переключатель
// Если установлен SLEEP_SWITCH_PIN, переход в режим сна осуществляется только если SLEEP_SWITCH_PIN в состоянии digitalRead = LOW

// #define SLEEP_SWITCH_PIN -1    // для ESP32 - ок, в ESP8266 некуда назначить при использовании 4.2 дисплея, не остается свободного пина
// #define SLEEP_ALWAYS_ON        // игнорировать режим сна

#if defined(HELTEC_BW_15_S810F)
/*
	[Дисплей 1.5'] Тестовый сетап
*/
	#define PARTIAL_UPDATE_INTERVAL 120        
	#define PARTIAL_UPDATE_SUPPORT  

  // /api/clocktest - демо отображение полноэкранных часов с секундами
  
  // не используется - сейчас везде порядок вывода пикселей инициализируется командами на уровне контроллера
  // #define WEB_BUFFER_MOD_FLIPX  // для кастомных картинок экрана - приведение к единому начальному положению интерфейса и чистого буфера, для тестов ENVDATA.screen.bufferMod = "flipx";
  // #define WEB_BUFFER_MOD_FLIPX
  // #define WEB_BUFFER_MOD_FLIPXY

  // int DEPG0150BxS810FxX_BW::EPD_Init - убрал постоянную переинициализацию SPI
  // после инициализации библиотеки ардуиновской либы SPI переопределяется EPD_BUSY_PIN тк он занимает неиспользуемый пин SPI (MOSI), освобождая шину I2C
  // EPD_RST_PIN - убран \ не используется т.к. в Heltec 1.5 он не распаян

  // ESP8266

  	#define EPD_RST_PIN   -1 // unused
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

	// [Частичное обновление экрана]

	// B\W displays
	// rev 2.2 - worked & tested
	// rev 2.1 - partial buffer push works, but no frame update availabe (orig examples also not works)
	// B\W\Red displays not supported

	#define PARTIAL_UPDATE_INTERVAL 120          // Интервал частичного обновления экрана в секундах для обновления часов - полностью экран рекомендуют обновлять не чаще 1 раза в 3 минуты (не должно превышать период полных обновления экрана -- sleepTime)
	#define PARTIAL_UPDATE_SUPPORT               // Partial update is configurable in web UI and disabled by default. Used for update clock widget each minute
	// #define RESET_MINUTE_TIMER_ON_WEB_REQUEST // Display updates could stuck web UI during screen refresh, so this can be usefull if you enter web interface frequently,
	#define COLORMODE_2BIT_SUPPORT               // Display supports 2-bit output mode - 4 colors (used for custom UI images) 

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
	
		// [https://42volna.ru/en/scheme/] Основная распиновка, с освобождением ножки CS
	
		#define EPD_BUSY_PIN 12  // D6 - GPIO - 12 (MISO) 
		#define EPD_RST_PIN  15  // D8 - GPIO - 15 (CS)
		#define EPD_DC_PIN   0   // D3 - GPIO - 0
		#define EPD_CS_PIN   -1  // GND (-1)    
		#define EPD_CLK_PIN  -1  // Всегда GPIO 13 (MOSI) - SPI
		#define EPD_DIN_PIN  -1  // Всегда GPIO 14 (SCLK) - SPI  

	/*
		// НЕ Основная
		// моя старая алт. распиновка, через резистор
		
		#define EPD_BUSY_PIN 2   // D4 - GPIO - 2 (+20кОм резистор на VCC)
		#define EPD_RST_PIN  12  // D6 - GPIO - 12 (MISO) 
		#define EPD_DC_PIN   0   // D3 - GPIO - 0
		#define EPD_CS_PIN   15  // D8 - GPIO - 15 (CS) или опционально GND (-1)
		#define EPD_CLK_PIN  -1  // Всегда GPIO 13 (MOSI) - SPI
		#define EPD_DIN_PIN  -1  // Всегда GPIO 14 (SCLK) - SPI  
	*/
		
	#endif

#endif

#endif	