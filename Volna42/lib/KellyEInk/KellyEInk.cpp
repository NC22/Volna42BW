#include <KellyEInk.h>

KellyEInk::KellyEInk(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din, unsigned int swidth, unsigned int sheight, unsigned long limitBusy) {
	
	ENK_PIN_BUSY = busy;
	ENK_PIN_RST = rst;
	ENK_PIN_DC = dc;
	ENK_PIN_CS = cs;

	if (clk >= 0) ENK_PIN_CLK = clk;		
	if (din >= 0) ENK_PIN_DIN = din;
	
	displayWidth = swidth;
	displayHeight = sheight;
	limit = limitBusy;
}

void KellyEInk::displayReset()
{
	deepSleep = false;

	if (ENK_PIN_RST < 0) {
		return;
	}

	digitalWrite(ENK_PIN_RST, HIGH);
	pinMode(ENK_PIN_RST, OUTPUT);
	delay(20);
	digitalWrite(ENK_PIN_RST, LOW);
	delay(resetDelay);
	digitalWrite(ENK_PIN_RST, HIGH);
	delay(200);

}

void KellyEInk::sendCommand(unsigned char reg) {

	digitalWrite(ENK_PIN_DC, LOW);

	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, LOW);
	}

	writeByte(reg);

	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, HIGH);
	}
}


void KellyEInk::sendDataPGM(const uint8_t* data, uint16_t n, int16_t fillZeros) {

    digitalWrite(ENK_PIN_DC, HIGH);
	
	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, LOW);
	}

    for (uint8_t i = 0; i < n; i++)
    {
        writeByte(pgm_read_byte(&*data++));
    }

    while (fillZeros > 0)
    {
        writeByte(0x00);
        fillZeros--;
    }	
	
	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, HIGH);
	}
}


void KellyEInk::sendData(unsigned char data) {

	digitalWrite(ENK_PIN_DC, HIGH);

	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, LOW);
	}

	writeByte(data);

	if (ENK_PIN_CS >= 0) {
		digitalWrite(ENK_PIN_CS, HIGH);
	}
}


void KellyEInk::writeByte(unsigned char data) {
    SPI.transfer(data);
}

void KellyEInk::endPins() {

  pinMode(ENK_PIN_CLK, INPUT);
  pinMode(ENK_PIN_DIN, INPUT);

  if (ENK_PIN_CS >= 0) pinMode(ENK_PIN_CS, INPUT);
  if (ENK_PIN_DC >= 0) pinMode(ENK_PIN_DC, INPUT);
  if (ENK_PIN_RST >= 0) pinMode(ENK_PIN_RST, INPUT);
  if (ENK_PIN_BUSY >= 0) pinMode(ENK_PIN_BUSY, INPUT);
}

void KellyEInk::spiBegin() {
	
	// SPI, ESP8266 defaults used, ESP32 can be more flexible
	
	#if defined(ESP32) 
		// int8_t sck, int8_t miso, int8_t mosi, int8_t ss
		SPI.begin(ENK_PIN_CLK, -1, ENK_PIN_DIN, ENK_PIN_CS);
	#else
		SPI.begin();
	#endif
    // SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
}

bool KellyEInk::initPins() {
	
	if (pinsInit) return true;

    if (ENK_PIN_BUSY >= 0) pinMode(ENK_PIN_BUSY, INPUT);
    if (ENK_PIN_RST >= 0) pinMode(ENK_PIN_RST, INPUT);
    if (ENK_PIN_DC >= 0) pinMode(ENK_PIN_DC, OUTPUT);
    
	if (ENK_PIN_CS >= 0) {
    	pinMode(ENK_PIN_CS, OUTPUT);
    	digitalWrite(ENK_PIN_CS, HIGH);
	}

	spiBegin();

	// ESP8266
	// reenable pin if assigned to unused SPI pin - MISO

	if (ENK_PIN_BUSY == 12) pinMode(ENK_PIN_BUSY, INPUT);
	if (ENK_PIN_RST == 12) pinMode(ENK_PIN_RST, INPUT);
	if (ENK_PIN_DC == 12) pinMode(ENK_PIN_DC, OUTPUT);


    Serial.println(F("Pins : "));
    Serial.print(F("BUSY : ")); Serial.println(ENK_PIN_BUSY);
    Serial.print(F("RST : ")); Serial.println(ENK_PIN_RST);
    Serial.print(F("DC : ")); Serial.println(ENK_PIN_DC);
    Serial.print(F("CS : ")); Serial.println(ENK_PIN_CS);
    Serial.println(F("-----------"));

	pinsInit = true;

	delay(initPinsDelay); 
	return true;
}

void KellyEInk::readBusy()
{
	unsigned long start = millis();
    
    Serial.println(F("[Default] e-Paper busy"));

	while(digitalRead(ENK_PIN_BUSY) == HIGH) {      //1: busy, 0: idle
		delay(100);
		if (millis() - start >= limit) {
			Serial.println(F("e-Paper busy read error - [TIMEOUT]"));
			break;
		}
	}  

	Serial.println(F("e-Paper busy release"));
}

