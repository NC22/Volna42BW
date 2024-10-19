#if !defined(Screen1in54_h)
#define Screen1in54_h

#include <UserDefines.h>
#include <SystemDefines.h>

#if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)

#if defined(HELTEC_BW_15_S810F)
    #include "KellyEInk_15_SSD1683_BW.h"
#elif defined(WAVESHARE_R_BW_15_SSD1683)
    #include "KellyEInk_15_SSD1683_RBW.h"
#endif



#include <Env.h>
#include <ScreenExternData.h>

enum scMode { scTClock, scTSensors };
enum clockModes { clockModeEnabled, clockModePrepare, clockModeDisabled };

class Env;

#include <WidgetController.h>

class WidgetController;

class Screen1in54UI {
    private :
        unsigned long constPowerTimerStart;
        unsigned long partialRefreshStart;
        // updateTime();

        #if defined(HELTEC_BW_15_S810F)
            KellyEInk_15_SSD1683_BW * displayDriver  = NULL;
        #elif defined(WAVESHARE_R_BW_15_SSD1683)
            KellyEInk_15_SSD1683_RBW * displayDriver  = NULL;
        #endif
        
        int drawTemp(int theight, bool indoor, float temperature, float humidity, float pressure, imageDrawModificators & mods);        
        Env * env;
        bool clearRequired = false;
        bool initPinsState = false;
        void initPins();
        scMode currentMode = scTSensors;

        clockModes clockModeState = clockModeDisabled;

        int ttextWidth = -1;
        int tstrX = -1;
        int tstrY = -1;
        int tstrXend = -1;
        int tstrYend= -1;
        int tstrW = -1;
        int tstrH = -1;
        time_t clockTime;
        WidgetController * widgetController;
        bool partial = false;

        bool drawUIToBufferCustom();

    public:
        
        bool displayBeasy = false;
        Screen1in54UI(Env * nenv);
        void drawUIToBuffer();
        void drawClock();
        void updateClock(); // big clock demo mode
        void updateScreen();
        void clearScreen();
        bool tick();
        void drawUILowBat();
        void updatePartialClock();
        void enableClockMode(bool state);
        bool is4ColorsSupported();
};

#endif	

#endif	