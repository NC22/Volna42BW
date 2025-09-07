#ifndef WidgetController_h
#define WidgetController_h

#include <Arduino.h>

#include <KellyCanvas.h>
#include <WidgetTypes.h>
#include <Env.h>

class Env;

class WidgetController {
    private :
	
	Env * env;
	
	public :
	
    partialUpdateData clockPartial;
	
	WidgetController();
	void setEnv(Env * senv);
	
	void partialDataApplyMaxBounds();
	void partialDataSet(int x, int y, unsigned int width, unsigned int height);
	void drawWidget(uiWidgetStyle widget);
	int getDaysInMonth(tm & time);
	int drawCalendarWidget(int baseX, int baseY, bool showTitles, bool showCurrentDate);
	String getPressureFormattedString(float pressure, bool hpa);
	int drawBatWidget(int baseX, int baseY, bool invert, bool ext, bool shortFormat);
	void drawClockWidgetTiny(int baseX, int baseY, bool clockOnly = false); // в формате одной строки  --  12:00 пн, 12.04.25
	void drawClockWidget(int baseX, int baseY, bool border, bool fill, bool invert, int & resultWidth, int & resultHeight, uint8_t fontType = 1); 
	void drawSystemInfoWidget(int baseX, int baseY, bool shortFormat = false);

    bool isHourInRange(int h, int hourFrom, int hourTo);
};

#endif	