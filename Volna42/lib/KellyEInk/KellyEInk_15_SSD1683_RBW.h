#ifndef __EINK_15_DRIVER_SSD1683_R_H_
#define __EINK_15_DRIVER_SSD1683_R_H_

#include <Arduino.h>
#include <KellyEInk.h>
#include <KellyEInk_15_SSD1683_BW.h>

// based on Heltec examples

class KellyEInk_15_SSD1683_RBW : public KellyEInk_15_SSD1683_BW {

	private:
	protected:
    public:		

		KellyEInk_15_SSD1683_RBW(int16_t busy, int16_t rst, int16_t dc, int16_t cs = -1, int16_t clk = -1, int16_t din = -1);
		
		void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true);

		bool colorMode2bitSupport = false;
		bool partialModeSupport = true;
};

#endif
