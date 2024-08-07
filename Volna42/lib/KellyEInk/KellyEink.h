/*
   @name           KellyEInk
   @description    Library for work with E-INK displays
   @author         Rubchuk Vladimir <torrenttvi@gmail.com> aka nradiowave
   @license        GPLv3
   @version        v0.6
	
	Part of Volna 42 project  |  https://42volna.ru/

	Light weight library for use in my personal projects based on collected examples from WaveShare, Good Display, Heltec, GxEPD2 for compiling 
	with only E-INK displays that I have and can test

	Display list : https://42volna.ru/ru/displays/

	WeAct 4.2'          - OK
	WaveShare 4.2'      - OK
	Good Display 4.2'   - (?)
	Heltec 1.5'         - OK

	Controller types : SSD1683, UC8176
*/

#ifndef __KellyEInk_H_
#define __KellyEInk_H_

#include <Arduino.h>
#include <SPI.h>

class KellyEInk {

	private:	
	protected :
		unsigned long limit; // read busy timeout
		void sendCommand(unsigned char reg);
		void sendData(unsigned char data);
		void sendDataPGM(const uint8_t* data, uint16_t n, int16_t fillZeros);
		void writeByte(unsigned char data);

		unsigned char FILL_BACKGROUND = 0xFF;
		unsigned char FILL_FRONT = 0x00;
		
		virtual void readBusy();
		
    public:

		bool pinsInit = false;
		bool deepSleep = true;
		unsigned int bitMode = 1;

		unsigned int displayWidth;
		unsigned int displayHeight;

		bool colorMode2bitSupport = false;
		bool partialModeSupport = false;

		unsigned int initPinsDelay = 300;
		unsigned int dispInitDelay = 100;
		unsigned int resetDelay = 2;

		int16_t ENK_PIN_BUSY = -1;
		int16_t ENK_PIN_RST = -1;
		int16_t ENK_PIN_DC = -1;
		int16_t ENK_PIN_CS = -1;
		
		// ESP8266 unchangable - SCLK, MOSI
		// todo - test & make configurable on ESP32
		int16_t ENK_PIN_CLK = SCK;		
		int16_t ENK_PIN_DIN = MOSI;
		
		KellyEInk(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din, unsigned int swidth, unsigned int sheight, unsigned long limitBusy = 10000);
		
		virtual void spiBegin();
		virtual bool initPins();
		virtual void endPins();	

		virtual void displayInit(unsigned int bitMode = 1, bool partial = false) = 0;
		virtual void displayClear() {
			display(NULL, NULL);
		}

		virtual void displayPartial(const unsigned char *Image, unsigned int  Xstart, unsigned int  Ystart, unsigned int  Xend, unsigned int  Yend, bool update = true, bool inverse = false) = 0;
		virtual void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true) = 0;
		
		virtual void displaySleep() = 0;
		virtual void displayUpdate() = 0;
		virtual void displayReset();
};


#endif
