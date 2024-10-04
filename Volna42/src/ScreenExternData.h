#if !defined(ScreenExternH)
#define ScreenExternH

#include <KellyCanvasTypes.h>

extern const fontManifest font18x18Config PROGMEM;
extern const fontManifest font44x44Config PROGMEM;

#if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683) 
extern imageData temp_15x48bw_settings PROGMEM;
extern imageData temp_meter_15x48bw_settings PROGMEM;
// extern imageData frog_new_year_hat174x139_settings PROGMEM;
extern imageData frog_magic_hat_186x141bw_settings PROGMEM;
#endif

extern imageData cels_39x43bw_settings PROGMEM;
extern imageData temp_15x68bw_settings PROGMEM;
extern imageData temp_meter_15x68bw_settings PROGMEM;
extern imageData cat_feed_125x127bw_settings PROGMEM;
extern imageData corner_line_7x7bw_settings PROGMEM;
extern imageData corner_top_7x7bw_settings PROGMEM;
extern imageData plug_23x39bw_settings PROGMEM;
extern imageData bat_meter_43x39bw_settings PROGMEM;
extern imageData fahr_39x43bw_settings PROGMEM;
extern imageData bat_43x39bw_settings PROGMEM;
extern imageData cat_watch_85x120bw_settings PROGMEM;
extern imageData cat_ball_40x40bw_settings PROGMEM;
extern imageData cat_night_96x72bw_settings PROGMEM;
extern imageData cat_winter_93x97bw_settings PROGMEM;
extern imageData cat_heat_110x159bw_settings PROGMEM;
extern imageData cat_happy_80x94bw_settings PROGMEM;
extern imageData cat_rain_127x125bw_settings PROGMEM;
extern imageData rain_93x52bw_settings PROGMEM;
extern imageData snowflakes_116x39bw_settings PROGMEM;
extern imageData moon_clear_93x63bw_settings PROGMEM;
extern imageData sun_clear_68x66bw_settings PROGMEM;

extern const char locIndoor[] PROGMEM;
extern const char locOutdoor[] PROGMEM;
extern const char locHumidity[] PROGMEM;
extern const char locTemp[] PROGMEM;
extern const char locUnavailable[] PROGMEM;
extern const char locLowBat[] PROGMEM;

extern const char locShortMonday[] PROGMEM;
extern const char locShortTuesday[] PROGMEM;
extern const char locShortWednesday[] PROGMEM;
extern const char locShortThursday[] PROGMEM;
extern const char locShortFriday[] PROGMEM;
extern const char locShortSaturday[] PROGMEM;
extern const char locShortSunday[] PROGMEM;

extern const char locPressureMM[] PROGMEM;
extern const char locPressureHPA[] PROGMEM;
extern const char locCO2[] PROGMEM;
extern const char locVoltage[] PROGMEM;

extern const char locMonth1January[] PROGMEM;
extern const char locMonth2February[] PROGMEM;
extern const char locMonth3March[] PROGMEM;
extern const char locMonth4April[] PROGMEM;
extern const char locMonth5May[] PROGMEM;
extern const char locMonth6June[] PROGMEM;
extern const char locMonth7July[] PROGMEM;
extern const char locMonth8August[] PROGMEM;
extern const char locMonth9September[] PROGMEM;
extern const char locMonth10October[] PROGMEM;
extern const char locMonth11November[] PROGMEM;
extern const char locMonth12December[] PROGMEM;

#endif	