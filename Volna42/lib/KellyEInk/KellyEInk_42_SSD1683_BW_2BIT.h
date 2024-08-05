#ifndef __EINK_42_DRIVER_SSD1683_H_
#define __EINK_42_DRIVER_SSD1683_H_

#include <Arduino.h>
#include <KellyEInk.h>

// Controller : SSD1683 | WaveShare 4.2 rev2.2 
// based on WaveShare examples

class KellyEInk_42_SSD1683_BW_2BIT : public KellyEInk {

	private:
		
		void initDisplay4Gray(void);			
		void display4Gray(const unsigned char *Image, int x, int y, int w, int l);
			
		static const unsigned char LUT_TABLE_LUT_4gray_all[];
		
	protected:
		bool partialProcess = false;
    public:		

		KellyEInk_42_SSD1683_BW_2BIT(int16_t busy, int16_t rst, int16_t dc, int16_t cs = -1, int16_t clk = -1, int16_t din = -1);
				
		void displayInit(unsigned int bitMode = 1, bool partialMode = false);
		void displayPartial(const unsigned char *Image, unsigned int  Xstart, unsigned int  Ystart, unsigned int  Xend, unsigned int  Yend, bool update = true, bool inverse = false);
		void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true);
		
		void displaySleep();
		void displayUpdate();
};

#endif
