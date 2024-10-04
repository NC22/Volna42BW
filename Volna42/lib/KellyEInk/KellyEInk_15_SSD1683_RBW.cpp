#include <KellyEInk_15_SSD1683_RBW.h>

KellyEInk_15_SSD1683_RBW::KellyEInk_15_SSD1683_RBW(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din): KellyEInk_15_SSD1683_BW(busy, rst, dc, cs, clk, din) {
	displayWidth = 200;
	displayHeight = 200;
}

void KellyEInk_15_SSD1683_RBW::display(const unsigned char *blackimage, const unsigned char *ryimage, bool update)
{	
	uint16_t Width, Height;
	Width = (displayWidth % 8 == 0)? (displayWidth / 8 ): (displayWidth / 8 + 1);
	Height = displayHeight;

	sendCommand(0x24);
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (blackimage) sendData(pgm_read_byte(&blackimage[i + j * Width]));
			else sendData(FILL_BACKGROUND);
		}
	}

	sendCommand(0x26);
	
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (ryimage) sendData(pgm_read_byte(&ryimage[i + j * Width]));
			else sendData(FILL_BACKGROUND);
		}
	}
	
	if (update) {	
		delay(10);	
		displayUpdate();
	}
}