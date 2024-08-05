#include "WidgetController.h"

WidgetController::WidgetController() {
    clockPartial.xEnd = -1;
    clockPartial.xStart = -1;
}

void WidgetController::setEnv(Env * senv) {
	env = senv;
}

void WidgetController::partialDataSet(int x, int y, unsigned int width, unsigned int height) {
        
    KellyCanvas * screen = env->getCanvas();

    int localWidth = screen->width;    
    int localHeight = screen->height;

    if (screen->rotation == 90) {
      screen->applyXYMods(x, y);
      int tmp = height;
      height = width;
      width = tmp;
      x -= width;
      localWidth = screen->height;
      localHeight = screen->width;
    }

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    
    if (y > localHeight) y = localHeight;
    if (x > localWidth) x = localWidth;

    clockPartial.yStart = y - (y % 8);
    clockPartial.yEnd = y + height;
    clockPartial.yEnd = clockPartial.yEnd + (8 - (clockPartial.yEnd % 8));

    clockPartial.xStart = x - (x % 8);
    clockPartial.xEnd = x + width;
    clockPartial.xEnd = clockPartial.xEnd + (8 - (clockPartial.xEnd % 8));

    if (clockPartial.xEnd > localWidth) {
      clockPartial.xEnd = localWidth;
    }

    if (clockPartial.yEnd > localHeight) {
      clockPartial.yEnd = localHeight;
    }
    
    if (clockPartial.yStart == clockPartial.yEnd || clockPartial.xStart == clockPartial.xEnd) {
      clockPartial.xEnd = -1;
      clockPartial.xStart = -1;
      Serial.println(F("Bad partial data position - out of bounds, move widget"));  
    }

    /*
        Serial.println("-------0000--------");  
        Serial.println(x);  
        Serial.println(y);   
        Serial.println(width);  
        Serial.println(height);  
        Serial.println("---------------");  

        Serial.println("-------11111--------");  
        Serial.println(clockPartial.xStart);  
        Serial.println(clockPartial.xEnd);   
        Serial.println(clockPartial.yStart);  
        Serial.println(clockPartial.yEnd);  
        Serial.println("---------------");  
    */
   
    if (env->lastState.lastPartialPos.xStart == -1 && env->lastState.lastPartialPos.xEnd == -1) {
        env->lastState.lastPartialPos = clockPartial;
    } 
}

void WidgetController::partialDataApplyMaxBounds() {
	
    env->lastState.lastPartialPos.xStart = min(clockPartial.xStart, env->lastState.lastPartialPos.xStart);
    env->lastState.lastPartialPos.yStart = min(clockPartial.yStart, env->lastState.lastPartialPos.yStart);
    env->lastState.lastPartialPos.xEnd = max(clockPartial.xEnd, env->lastState.lastPartialPos.xEnd);
    env->lastState.lastPartialPos.yEnd = max(clockPartial.yEnd, env->lastState.lastPartialPos.yEnd);
}

void WidgetController::drawClockWidget(int baseX, int baseY, bool border, bool fill, bool invert, int & resultWidth, int resultHeight) {
	
  KellyCanvas * screen = env->getCanvas();
	bool colorBg = false;
	bool colorText = true;
	int16_t borderWidth = 0;

  if (border) borderWidth = 2;

	if (invert) {
		colorText = !colorText;
		colorBg = !colorBg;
	}
	
  clockFormatted dt = env->getFormattedTime();

	screen->setFont(&font18x18Config);
	uText dateText = screen->getUText(dt.monthText + ", " + dt.dayText);
	uText dateShortText = screen->getUText(dt.date);

	resultWidth = dateText.pixelWidth + 20;
	resultHeight = 92;

	if (fill) screen->drawRoundedSquare(baseX, baseY, resultWidth, resultHeight, 4, colorBg, borderWidth);
	
	screen->color = tBLACK;      

	screen->setFont(&font44x44Config);
	uText timeText = screen->getUText(dt.timeText);
	if (env->hour12) timeText.pixelWidth += 10;

	baseX = baseX + 8;// + ceil((float) (((resultWidth / 2) - dateText.pixelWidth)  / 2));   
	baseY = baseY + 2;

	unsigned int partialX = baseX + ceil((float) ((dateText.pixelWidth - timeText.pixelWidth) / 2));
	unsigned int partialY = baseY;
	unsigned int partialHeight = screen->font->height;

	int timePoseX = screen->drawStringUtext(partialX, partialY, timeText, colorText);	
	screen->setFont(&font18x18Config);

	if (env->hour12) timePoseX = screen->drawString(timePoseX + 4, baseY + 44 - 18, dt.pm ? "pm" : "am", true);
	unsigned int partialWidth = timePoseX - partialX + 4;
	
	if (env->lastState.updateMinutes) partialDataSet(partialX, partialY, partialWidth, partialHeight);
	baseY += 44;
	
	screen->drawStringUtext(baseX + ceil((float) ((dateText.pixelWidth - dateShortText.pixelWidth) / 2)), baseY, dateShortText, colorText);
	baseY += 17;
	
	screen->drawStringUtext(baseX, baseY, dateText, colorText);  
	screen->setFont(&font18x18Config);
}

void WidgetController::drawSystemInfoWidget(int baseX, int baseY) {
  
    KellyCanvas * screen = env->getCanvas();
    screen->setFont(&font18x18Config);
    if (env->isOnBattery()) {

        // possibly some important system info about errors
        if (env->wifiInfo.length() > 0) {

          screen->drawString(baseX, baseY, env->wifiInfo, true);

        } else {
          
          int lkey = env->lastState.lastTelemetrySize-1;
          String techInfo = String(env->lastState.lastTelemetry[lkey].bat) + "V";
          screen->drawString(baseX, baseY, techInfo, true);

          baseY += 20;
          techInfo = String(env->lastState.connectTimes) + " " + env->getFormattedExtSensorLastSyncTime();
          screen->drawString(baseX, baseY, techInfo, true);
        }

    } else {

        if (env->wifiInfo.length() > 0) {
          screen->drawString(baseX, baseY, "IP : " + env->wifiInfo, true);    
          baseY += 20;      
        }

        screen->drawString(baseX, baseY, env->getFormattedExtSensorLastSyncTime(), true); // String(env->lastState.connectTimes) + " " + 
    }     

}

/*
  baseX - x position
  baseY - y position
  invert - use white color for line
  ext    - get battery info for external sensor
  shortFormat - draw battery charge percents inside of battery icon

  returns battery widget padding by x
*/

int WidgetController::drawBatWidget(int baseX, int baseY, bool invert, bool ext, bool shortFormat) {
 
    KellyCanvas * screen = env->getCanvas();
    baseY -= 2;
    baseX += 10;
    int chargeLvl = 100;
    int minChargePixels = 10;
    int maxChargePixels = 36;
    int chargePixelsTotal = maxChargePixels - minChargePixels;
    
    bool plugged = false;
    bool color = true;
    if (invert) color = false;
    
    if (ext) {

        if (env->lastState.extData.bat > 0) {
          chargeLvl = env->lastState.extData.bat;
          if (chargeLvl >= 100) chargeLvl = 99;
        } else return 0;

    } else {

      if (env->isOnBattery()) {
              
        int lkey = env->lastState.lastTelemetrySize-1;
        if (lkey > -1) {
            chargeLvl = (int) round(env->getBatteryLvlfromV(env->lastState.lastTelemetry[lkey].bat));
        }

      } else {
          screen->drawImage(baseX - 23 - 2, baseY, &plug_23x39bw_settings, color);
          plugged = true;
      }
    }    
    
    int chargePixels = chargePixelsTotal - (int) round(((float) chargePixelsTotal / 100.0) * chargeLvl);
    Serial.print(F("Charge pixels : ")); Serial.print(String(chargePixels)); Serial.print(F(" | % ")); Serial.println(String(chargeLvl));

    screen->drawImage(baseX, baseY, &bat_43x39bw_settings, color);      

    if (shortFormat) {

        screen->setFont(&font18x18Config);
        uText chargeLvlText = screen->getUText(String(chargeLvl));

        // old - baseX + 12
        // [base padding from left side] + (battery icon inner space width - text width) / 2
        screen->drawStringUtext(baseX + (6 + ((33 - chargeLvlText.pixelWidth) / 2)), baseY + 9, chargeLvlText, color);

        return bat_43x39bw_settings.width;

    } else {
      
        screen->color = tRY;
        imageDrawModificators mods;
        screen->resetImageMods(mods, true, false, false, minChargePixels + chargePixels, maxChargePixels); 
        screen->drawImageMods(baseX, baseY, &bat_meter_43x39bw_settings, mods, color);
        screen->color = tBLACK;
        screen->setFont(&font18x18Config);
        
        if (!plugged) return 39 + screen->drawString(baseX - 39, baseY + 9, String(chargeLvl) + "%", color);
        else return plug_23x39bw_settings.width + 39;
    }
}

void WidgetController::drawWidget(uiWidgetStyle widget) {
	
    KellyCanvas * screen = env->getCanvas();
    screen->setRotate(!env->land ? 90 : 0);

    // todo - invert colors \ disable background square by .params

    screen->setFont(&font18x18Config);
    int widgetWidth = -1;
    int widgetHeight = -1;

    int baseX = widget.x;   
    int baseY = widget.y;

    int lkey = env->lastState.lastTelemetrySize-1;

    if (widget.type == uiClock) {
		
      drawClockWidget(
        baseX, 
        baseY, 
        widget.params.indexOf("-b") != -1, 
        widget.params.indexOf("-f") != -1, 
        widget.params.indexOf("-i") != -1, 
        widgetWidth, 
        widgetHeight
      );
		
    } else if (
          widget.type == uiInfoIP || 
          widget.type == uiInfoVoltage || 
          widget.type == uiLastSyncRemote || 
          widget.type == uiShortInfoSyncRemote || 
          widget.type == uiInfoSyncNumRemote
      ) {
      
      widgetHeight = 20;
      String result = "";

      if (widget.type == uiInfoIP) {

        if (env->wifiInfo.length() > 0) {
	        result = "IP : " + env->wifiInfo;
        }

      } else if (widget.type == uiInfoVoltage) {
        
        if (lkey > -1) {
          result = String(env->lastState.lastTelemetry[lkey].bat) + "V";      
        } else {
          result = "-.-V";
        }

      } else if (widget.type == uiLastSyncRemote) {

         result = env->getFormattedExtSensorLastSyncTime();

      } else if (widget.type == uiShortInfoSyncRemote) {

         result = String(env->lastState.connectTimes) + " " + env->getFormattedExtSensorLastSyncTime();

      } else if (widget.type == uiInfoSyncNumRemote) {

         result = String(env->lastState.connectTimes);

      }
      
      if (result.length() > 0) {

          uText text = screen->getUText(result);

          widgetWidth = text.pixelWidth + 10;

          if (widget.params.indexOf("-f") != -1) {
            screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
            baseX += 5;
          }

          screen->drawStringUtext(baseX, baseY, text, true); 

      } else {

         widgetWidth = 0;
         widgetHeight = 0;
      }

    } else if (widget.type == uiBat || widget.type == uiBatRemote) {

        widgetWidth = 80;
        widgetHeight = 20;
        if (widget.params.indexOf("-f") != -1) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
        }

        drawBatWidget(baseX + 20, baseY, false, widget.type == uiBatRemote ? true : false, false); // baseX started from bat icon, not count prefix text

    } else if  (widget.type == uiTemp || widget.type == uiTempRemote || widget.type == uiHumRemote || widget.type == uiHum) {
        
        widgetWidth = 152;
        widgetHeight = 70;
          
        if (widget.params.indexOf("-f") != -1) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
        }

        // screen->drawRoundedSquare(baseX, baseY, widgetWidth, widgetHeight, 4, false);

        bool extSens = widget.type == uiHumRemote || widget.type == uiTempRemote;
        bool temp = widget.type == uiTemp || widget.type == uiTempRemote;

        if (extSens && !env->lastState.extData.isDataValid) {  // external sensor no data
          
          screen->drawString(baseX, baseY, "no data (ext)", true);

        } else if (!extSens && env->lastState.lastTelemetrySize <= 0) {// internal sensor no data

          screen->drawString(baseX, baseY, "no data", true);

        } else if (lkey <= -1) {

          screen->drawString(baseX, baseY, "no data (empty)", true);

        } else {
          
            // sensors 
            float t; float h; 

            if (extSens) {
                t = env->lastState.extData.temperature;
                h = env->lastState.extData.humidity;
            } else {
                t = env->lastState.lastTelemetry[lkey].temperature;
                h = env->lastState.lastTelemetry[lkey].humidity;
            }

            baseX += 10;
            char buffer[128];

            if (temp) {
                if (t <= -1000) {

                  screen->drawString(baseX, baseY, "sensor bad data", true);

                } else {
                    screen->drawString(baseX, baseY, extSens ? FPSTR(locOutdoor) : FPSTR(locIndoor), true);                
                    screen->setFont(&font44x44Config);

                    sprintf(buffer, "%.1f", t);
                    int twidth = screen->drawString(baseX, baseY + 20, buffer, true);  

                    // Celsius glyph symbol
                    screen->drawImage(twidth + 10, baseY + 20 + 4, &cels_39x43bw_settings, true);     
                }

            } else {
              
              if (h > -1000) {
                screen->drawString(baseX, baseY, FPSTR(locHumidity), true);                
                screen->setFont(&font44x44Config);
                sprintf(buffer, "%.1f%%", h);  
                screen->drawString(baseX, baseY + 20, buffer, true);
              }
            }
        }
    } else if (widget.type == uiInfo) {

        widgetWidth = 102;
        widgetHeight = 36;
        
        if (widget.params.indexOf("-f") != -1) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
        }

        if (widget.params.indexOf("border") != -1) {
          int padding = 1;
          screen->drawRoundedSquare(baseX-padding, baseY-padding, widgetWidth+(padding * 2), widgetHeight+(padding * 2), 4, false);
        }

        drawSystemInfoWidget(baseX, baseY);
    }
}