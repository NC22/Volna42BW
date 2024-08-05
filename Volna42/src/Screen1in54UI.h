#if !defined(Screen1in54_h)
#define Screen1in54_h

#include <UserDefines.h>

#if defined(HELTEC_BW_15_S810F)

#include "KellyEInk_15_SSD1683_BW.h"


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

        KellyEInk_15_SSD1683_BW * displayDriver  = NULL;
        int drawTemp(int theight, String title, float temperature, float humidity, imageDrawModificators & mods);        
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
};

#endif	

#endif	