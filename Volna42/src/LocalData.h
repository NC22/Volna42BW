

#if defined(LOCALE_JA)
    #include "fonts/KellyFontC44x44JA.h"
    #include "fonts/KellyFontC18x18JA.h"
#else 
    #include "fonts/KellyFontC44x44.h"
    #include "fonts/KellyFontC18x18.h"
#endif

#include "fonts/Clock64x64.h"
#include "fonts/Clock54x54.h"
// #include "tiles/cat115x125snowflakes.h" // old format

#include "tiles/cat_feed.h"
#include "tiles/cat_night.h"
#include "tiles/cat_winter.h"
#include "tiles/cat_happy.h"
#include "tiles/cat_vampi.h"
#include "tiles/cat_rain.h"
#include "tiles/cat_watch.h"
#include "tiles/cat_heat.h"

#if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)
    // #include "tiles/frog_new_year_hat174x139.h"
    #include "tiles/frog_magic_hat186x141.h"
#endif

#include "tiles/weather_effects.h"
#include "tiles/timages.h"