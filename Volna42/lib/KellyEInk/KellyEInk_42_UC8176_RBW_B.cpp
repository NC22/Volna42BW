#include <KellyEInk_42_UC8176_RBW_B.h>

KellyEInk_42_UC8176_RBW_B::KellyEInk_42_UC8176_RBW_B(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din): 
	KellyEInk_42_UC8176_BW(busy, rst, dc, cs, clk, din) {

	limit = 50000;
}

void KellyEInk_42_UC8176_RBW_B::display(const unsigned char *blackimage, const unsigned char *ryimage, bool update)
{
    Serial.println(F("Display R + BW buffer [Model B - second buffer inverted]"));

	uint16_t Width, Height;
	Width = (displayWidth % 8 == 0)? (displayWidth / 8 ): (displayWidth / 8 + 1);
	Height = displayHeight;

	sendCommand(0x10);
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (blackimage) sendData(blackimage[i + j * Width]);
			else sendData(FILL_BACKGROUND); 
		}
	}
	
	sendCommand(0x13);
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (ryimage) sendData(~ryimage[i + j * Width]);
			else sendData(~FILL_BACKGROUND);
		}
	}
	
	if (!ryimage) Serial.println(F("Skip Red buffer"));
	if (update) displayUpdate();
}