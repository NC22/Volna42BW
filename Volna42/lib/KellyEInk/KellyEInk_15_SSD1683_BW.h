#ifndef __EINK_15_DRIVER_SSD1683_H_
#define __EINK_15_DRIVER_SSD1683_H_

#include <Arduino.h>
#include <KellyEInk.h>
#include <KellyEInk_42_SSD1683_BW_2BIT.h>

// based on Heltec examples

class KellyEInk_15_SSD1683_BW : public KellyEInk_42_SSD1683_BW_2BIT {

	private:
		static const unsigned char LUT_TABLE_LUT_WF_PARTIAL[];
	protected:
    public:		

		KellyEInk_15_SSD1683_BW(int16_t busy, int16_t rst, int16_t dc, int16_t cs = -1, int16_t clk = -1, int16_t din = -1);
		void displayInit(unsigned int newBitMode = 1, bool partialMode = false) ;
		//void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true);
		void displayUpdate();
		//void displaySleep();
		//void spiBegin();
};

#endif
