#include "deepsleepFix.h"

uint32_t*RT= (uint32_t *)0x60000700;

// method = 2 - slightly different codes, tested without issues on LOLIN D1 Mini V4, original D1 Mini Clone boards
// method = 1 - works ok, but some issues reported with some specifed delays, may be depends on board itself

void nkDeepsleep(uint32_t t_us, unsigned int method)
{ 

    // possibly this props will be ignored
    system_phy_set_powerup_option(2); // calibrate only 2ms;
    system_deep_sleep_set_option(2); // low deepsleep

    system_phy_set_max_tpw(82); //0-82 , 1=0.25dB

  if (method == 2) {
    t_us = 1.31 * t_us;

    RT[4] = 0;
    *RT = 0;
    RT[1]=100;
    RT[3] = 0x10010;
    RT[6] = 8;
    RT[17] = 4;
    RT[2] = 1<<20;
    ets_delay_us(10);
    RT[1]=t_us>>3;
    RT[3] = 0x640C8;
    RT[4]= 0;
    RT[6] = 0x18;
    RT[16] = 0x7F;
    RT[17] = 0x20;
    RT[39] = 0x11;
    RT[40] = 0x03;
    RT[2] |= 1<<20;
    __asm volatile ("waiti 0");

  } else {

    RT[0]= 0x30;
    RT[1]= RT[7] + 5;
    RT[3]= 0x10010;
    RT[4]=0;
    RT[6]= 8;
    RT[17]= 4;
    RT[2]=1<<20;
    ets_delay_us(10);
    RT[0]&=0xFCF;
    RT[0]=0;
    RT[1]=RT[7] + (45*(t_us >> 8));
    RT[3]=0x640C8;
    RT[6]=0x18;
    RT[16]=0x7F;
    RT[17]=0x20;
    RT[39]=0x11;
    RT[40]=0x03;
    RT[2]=1<<20;
    __asm volatile ("waiti 0");
  }

}
