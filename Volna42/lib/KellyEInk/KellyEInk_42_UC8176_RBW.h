#ifndef __EINK_42_DRIVER_UC8176_RBW_H_
#define __EINK_42_DRIVER_UC8176_RBW_H_

#include <Arduino.h>
#include <KellyEInk_42_UC8176_BW.h>

class KellyEInk_42_UC8176_RBW : public KellyEInk_42_UC8176_BW {

	private:
	protected:
    public:		
		KellyEInk_42_UC8176_RBW(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk = -1, int16_t din = -1);
		void display(const unsigned char *blackimage, const unsigned char *ryimage = NULL, bool update = true);
};

#endif