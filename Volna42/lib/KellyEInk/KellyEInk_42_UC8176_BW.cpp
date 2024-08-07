#include <KellyEInk_42_UC8176_BW.h>

KellyEInk_42_UC8176_BW::KellyEInk_42_UC8176_BW(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din): KellyEInk(busy, rst, dc, cs, clk, din, 400, 300) {
	
}

// same waveform as by demo code from Good Display, looked from GxEPD2
#define T1  0 // color change charge balance pre-phase
#define T2  0 // color change or sustain charge balance pre-phase
#define T3  0 // color change or sustain phase
#define T4 25 // color change phase

const unsigned char KellyEInk_42_UC8176_BW::LUT_TABLE_20_vcom0_partial[] PROGMEM =
{
  0x00, T1, T2, T3, T4, 1, // 00 00 00 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char KellyEInk_42_UC8176_BW::LUT_TABLE_21_ww_partial[] PROGMEM =
{ // 10 w
  0x18, T1, T2, T3, T4, 1, // 00 01 10 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char KellyEInk_42_UC8176_BW::LUT_TABLE_22_bw_partial[] PROGMEM =
{ // 10 w
  0x5A, T1, T2, T3, T4, 1, // 01 01 10 10
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char KellyEInk_42_UC8176_BW::LUT_TABLE_23_wb_partial[] PROGMEM =
{ // 01 b
  0xA5, T1, T2, T3, T4, 1, // 10 10 01 01
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

const unsigned char KellyEInk_42_UC8176_BW::LUT_TABLE_24_bb_partial[] PROGMEM =
{ // 01 b
  0x24, T1, T2, T3, T4, 1, // 00 10 01 00
  0x00,  1,  0,  0,  0, 1, // gnd phase
};

/*	
	Set partial data for display RAM

	if Image is NULL - clears area by background color (invers = true - turn every pixel ON)
*/
void KellyEInk_42_UC8176_BW::displayPartial(const unsigned char *Image, unsigned int  Xstart, unsigned int  Ystart, unsigned int  Xend, unsigned int  Yend, bool update, bool inverse) {
	
	partialProcess = true;
 	unsigned int i, j, Width, Height;
    unsigned int BytesPerLine = displayWidth / 8;

    // Переводим координаты в байты
    unsigned int ByteXstart = Xstart / 8;
    unsigned int ByteXend = (Xend + 7) / 8; // округляем вверх, чтобы захватить все пиксели

    Width = max(Xstart, Xend) - min(Xstart, Xend);  
    Height = max(Yend, Ystart) - min(Yend, Ystart);
	
	// Controller : UC8176 | WaveShare 4.2 rev2.1 BW || BW + Red or Yellow

	sendCommand(0x91);     // 0x91
	sendCommand(0x90); // 0x90
	
	sendData(Xstart >> 8);
	sendData(Xstart & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
	sendData(((Xstart & 0xf8) + Width  - 1) >> 8);
	sendData(((Xstart & 0xf8) + Width  - 1) | 0x07);
	sendData(Ystart >> 8);        
	sendData(Ystart & 0xff);
	sendData((Ystart + Height - 1) >> 8);        
	sendData((Ystart + Height - 1) & 0xff);
	sendData(0x01);         // Gates scan both inside and outside of the partial window. (default) 
	delay(2);
	
	// 0x13 - второй буффер окна контроллера
	// в BW версиях только этот буфер отвечает за частичное обновление
	// в R + BW точно не понятно

	sendCommand(0x13);

	for (j = Ystart; j < Yend; j++) {
		for (i = ByteXstart; i < ByteXend; i++) {
			unsigned int index = j * BytesPerLine + i;
			if (!Image) sendData(inverse ? FILL_FRONT : FILL_BACKGROUND);
			else sendData(inverse ? ~Image[index] : Image[index]);
		}
	}

	sendCommand(0x10);  
	for (j = Ystart; j < Yend; j++) {
		for (i = ByteXstart; i < ByteXend; i++) {
			unsigned int index = j * BytesPerLine + i;
			if (!Image) sendData(inverse ? FILL_FRONT : FILL_BACKGROUND);
			else sendData(!inverse ? ~Image[index] : Image[index]);
		}
	}

	if (update) {
		displayUpdate();
	}

	delay(2);
	sendCommand(0x92);  
	partialProcess = false;
}

void KellyEInk_42_UC8176_BW::displayInit(unsigned int bitMode, bool partialMode) {	

	Serial.println(F("e-Paper : [displayInit]"));
	if (bitMode > 1 && !colorMode2bitSupport) {
		Serial.println(F("e-Paper : used with [UNSUPPORTED COLOR MODE] 2bit, possible artifacts and wrong output"));
	}

	if (deepSleep) displayReset();		
	sendCommand(0x04);  // Power ON (PON)
	readBusy();		

	/*
		"The voltage sequence and temperature range are stored in a lookup Table(LUT)
		and is precisely paired to the display by production lots to enable precise placement of pigment to achieve accurate gray-level...""
		
		https://www.e-paper-display.com/news_detail/newsId%3D44.html

	*/

	sendCommand(0x00);  // PANEL_SETTING
	sendData(0x0f);	 // LUT from OTP

	if (pushInitTwice) {
		
		delay(300); 
		
		// Copy-past init commands
		// for some reason initializasion always ignored at first call with one of my boards (tested on different pins but nothing changes), idk why, may be just right delay toone needed, but
		// this sequense is stable in my case

		displayReset();
		
		sendCommand(0x04); 
		readBusy();

		sendCommand(0x00);
		sendData(0x0f);
	}
	
	if (partialMode) {

		Serial.println(F("e-Paper : [Partial mode]"));

		sendCommand(0x01); // POWER SETTING
		sendData (0x03);   // VDS_EN, VDG_EN internal
		sendData (0x00);   // VCOM_HV, VGHL_LV=16V
		sendData (0x2b);   // VDH=11V
		sendData (0x2b);   // VDL=11V
		sendCommand(0x06); // boost soft start
		sendData (0x17);   // A
		sendData (0x17);   // B
		sendData (0x17);   // C
		sendCommand(0x00); // panel setting
		sendData(0x3f);    // 300x400 B/W mode, LUT set by register
		sendCommand(0x30); // PLL setting
		sendData (0x3a);   // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ
		sendCommand(0x61); // resolution setting
		sendData (displayWidth / 256);
		sendData (displayWidth % 256);
		sendData (displayHeight / 256);
		sendData (displayHeight % 256);
		sendCommand(0x82); // vcom_DC setting
		sendData (0x1A);   // -0.1 + 26 * -0.05 = -1.4V from OTP
		sendCommand(0x50); // VCOM AND DATA INTERVAL SETTING
		sendData(0xd7);    // border floating to avoid flashing
		sendCommand(0x20);
		sendDataPGM(LUT_TABLE_20_vcom0_partial, sizeof(LUT_TABLE_20_vcom0_partial), 44 - sizeof(LUT_TABLE_20_vcom0_partial));
		sendCommand(0x21);
		sendDataPGM(LUT_TABLE_21_ww_partial, sizeof(LUT_TABLE_21_ww_partial), 42 - sizeof(LUT_TABLE_21_ww_partial));
		sendCommand(0x22);
		sendDataPGM(LUT_TABLE_22_bw_partial, sizeof(LUT_TABLE_22_bw_partial), 42 - sizeof(LUT_TABLE_22_bw_partial));
		sendCommand(0x23);
		sendDataPGM(LUT_TABLE_23_wb_partial, sizeof(LUT_TABLE_23_wb_partial), 42 - sizeof(LUT_TABLE_23_wb_partial));
		sendCommand(0x24);
		sendDataPGM(LUT_TABLE_24_bb_partial, sizeof(LUT_TABLE_24_bb_partial), 42 - sizeof(LUT_TABLE_24_bb_partial));
	}

	delay(dispInitDelay);
}

void KellyEInk_42_UC8176_BW::displayUpdate() {

	sendCommand(0x12); 
	delay(100);
	readBusy();
}

void KellyEInk_42_UC8176_BW::displaySleep()
{

	sendCommand(0X50);
	sendData(0xf7);		//border floating	

	sendCommand(0X02);  	//power off
	readBusy(); //waiting for the electronic paper IC to release the idle signal
	sendCommand(0X07);  	//deep sleep
	sendData(0xA5);
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays, for black & white displays with single buffer
parameter:
******************************************************************************/
void KellyEInk_42_UC8176_BW::display(const unsigned char *blackimage, const unsigned char *ryimage, bool update)
{	
	uint16_t Width, Height;
	Width = (displayWidth % 8 == 0)? (displayWidth / 8 ): (displayWidth / 8 + 1);
	Height = displayHeight;

	// must be filled too
	sendCommand(0x10);
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			sendData(0x00);
		}
	}

	sendCommand(0x13);
	
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (blackimage) sendData(blackimage[i + j * Width]);
			else sendData(FILL_BACKGROUND);
		}
	}

	if (update) {	
		delay(10);	
		displayUpdate();
	}
}


void KellyEInk_42_UC8176_BW::readBusy()
{
	unsigned long start = millis();
	
    Serial.println(F("[UC8176] e-Paper busy"));
	sendCommand(0x71);

	do{
		
		sendCommand(0x71);
		delay(100);

		if (millis() - start >= limit) {
			Serial.println(F("e-Paper busy read error - [TIMEOUT]"));
			break;
		}

	} while(!(digitalRead(ENK_PIN_BUSY)));

	Serial.println(F("e-Paper busy release"));
	delay(100);
}
