#if !defined(Screen4in2_h)
#define Screen4in2_h

#include <UserDefines.h>
#include <Env.h>
#include <ScreenExternData.h>

#if defined(WAVESHARE_BW_42_UC8176) 
#include "KellyEInk_42_UC8176_BW.h"
#elif defined(WAVESHARE_BW_42_SSD1683)
#include "KellyEInk_42_SSD1683_BW_2BIT.h"
#elif defined(WAVESHARE_RY_BW_42_UC8176)
#include "KellyEInk_42_UC8176_RBW.h"
#endif

class Env;

#include <WidgetController.h>

class WidgetController;

class Screen4in2UI {
    private :

        #if defined(WAVESHARE_BW_42_UC8176) 
        KellyEInk_42_UC8176_BW * displayDriver  = NULL;
        #elif defined(WAVESHARE_BW_42_SSD1683)
        KellyEInk_42_SSD1683_BW_2BIT * displayDriver  = NULL;
        #elif defined(WAVESHARE_RY_BW_42_UC8176)
        KellyEInk_42_UC8176_RBW * displayDriver  = NULL;
        #endif
        
        int drawTemp(int theight, bool indoor, float temperature, float humidity, imageDrawModificators & mods, bool land = false);        
        int drawCat(bool land = false);        
        void drawWidget(uiWidgetStyle & widget);
        void drawBat(int baseX, int baseY, bool invert, bool ext, bool shortFormat);

        Env * env;
        bool clearRequired = false; // clear buffer before draw to it if (needed only in constant mode)
        bool initPinsState = false;

        // используются в drawUIToBuffer функциях, выставляются с учетом поворота
        int localWidth = 300;
        int localHeight = 400;

        // partialUpdateData clockPartial;
        // partialUpdateData clockPartialPrev; - moved to RTC memory

        // For partial mode skip draw some of UI parts (temp & hum widget & cat image) during drawUIToBuffer to update only clock widget and possible bottom layer
        bool partial = false; 
        int returnBitPerPixel = -1;
        
        WidgetController * widgetController;

        void initPins();
        int drawWeaterIcon(KellyCanvas * screen, bool cold, bool night, int hpad, bool land = false);
        int calcMarginMiddle(int spaceWidth, int objectWidth);
        int getDevidedBy8Num(int x, bool upper = false);

    public:
        
        bool displayBeasy = false;
        Screen4in2UI(Env * nenv);
        bool tick();
        bool drawUIToBufferCustom();
        void drawUIToBuffer();
        void drawUIToBufferLand();
        void drawUIInit(int w, int h, imageDrawModificators & mods);
        void updateScreen();
        void clearScreen();
        void drawUILowBat(bool land = false);

        void updateTestPartial2();
        void updateTestPartial(bool afterWakeup = false);
        void updatePartialClock();
        bool is4ColorsSupported();
        // void drawUISysNotice(String text, int x, int y, int id);
};

#endif	