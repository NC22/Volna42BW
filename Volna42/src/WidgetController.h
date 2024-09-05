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
	String getPressureFormattedString(float pressure, bool hpa);
	int drawBatWidget(int baseX, int baseY, bool invert, bool ext, bool shortFormat);
	void drawClockWidget(int baseX, int baseY, bool border, bool fill, bool invert, int & resultWidth, int resultHeight); // todo - font size config
	void drawSystemInfoWidget(int baseX, int baseY);
};

#endif	