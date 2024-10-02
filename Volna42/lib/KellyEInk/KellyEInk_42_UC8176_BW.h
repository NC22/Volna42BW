#ifndef __EINK_42_DRIVER_UC8176_H_
#define __EINK_42_DRIVER_UC8176_H_

#include <Arduino.h>
#include <KellyEInk.h>

class KellyEInk_42_UC8176_BW : public KellyEInk {

	private:
		
	
		static const unsigned char LUT_TABLE_20_vcom0_partial[];
		static const unsigned char LUT_TABLE_21_ww_partial[];
		static const unsigned char LUT_TABLE_22_bw_partial[];
		static const unsigned char LUT_TABLE_23_wb_partial[];
		static const unsigned char LUT_TABLE_24_bb_partial[];	
	protected:
		void readBusy(void);
		bool partialProcess = false;
    public:		

		KellyEInk_42_UC8176_BW(int16_t busy, int16_t rst, int16_t dc, int16_t cs = -1, int16_t clk = -1, int16_t din = -1);
		
		bool pushInitTwice = true;
		
		void displayInit(unsigned int bitMode = 1, bool partialMode = false);
		void displayPartial(const unsigned char *Image, unsigned int  Xstart, unsigned int  Ystart, unsigned int  Xend, unsigned int  Yend, bool update = true, bool inverse = false);
		void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true);
		
		void displaySleep();
		void displayUpdate();

		bool colorMode2bitSupport = false;
		bool partialModeSupport = true;
};

#endif
