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
    
    //if (y > localHeight) y = localHeight;
    //if (x > localWidth) x = localWidth;

    clockPartial.yStart = y - (y % 8);
    clockPartial.yEnd = y + height;
    clockPartial.yEnd = clockPartial.yEnd + (8 - (clockPartial.yEnd % 8));

    clockPartial.xStart = x - (x % 8);
    clockPartial.xEnd = x + width;
    clockPartial.xEnd = clockPartial.xEnd + (8 - (clockPartial.xEnd % 8));

    // clockPartial struct contains absolute coordinats, order must be the same as in SPI driver
    if (clockPartial.xEnd > screen->width) {
      clockPartial.xEnd = localWidth;
    }

    if (clockPartial.yEnd > screen->height) {
      clockPartial.yEnd = localHeight;
    }
    
    if (clockPartial.yStart == clockPartial.yEnd || clockPartial.xStart == clockPartial.xEnd) {
      clockPartial.xEnd = -1;
      clockPartial.xStart = -1;
      Serial.println(F("Bad partial data position - out of bounds, move widget"));  
    }
    /*
        Serial.println("-------0000--------"); 
        Serial.println(screen->rotation);  
        Serial.println(x);  
        Serial.println(y);   
        Serial.println(width);  
        Serial.println(height);  
        Serial.println(localWidth);  
        Serial.println(localHeight);  
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

int WidgetController::getDaysInMonth(tm & time) {

  //  https://habr.com/ru/articles/261773/#comment_8486521

  int days;
  int month = time.tm_mon + 1;
  int year = time.tm_year + 1900;

  switch (month) {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
       days = 31;
       break;
    case 4:
    case 6:
    case 9:
    case 11:
       days = 30;
       break;
    case 2:
 
       days = 28;
       if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
          days += 1;
       }

       break;
    default:
        days = -1;
    }

    return days;
}

void WidgetController::drawCalendarWidget(int baseX, int baseY, bool showTitles) {

  time_t now = time(nullptr);
  struct tm stnow;

  localtime_r(&now, &stnow);

  int maxDayOfMonth = getDaysInMonth(stnow);
  int currentDayOfMonth = stnow.tm_mday;

  stnow.tm_mday = 1;
  mktime(&stnow);

  // с понедельника. от 1 до 7
  int firstDayOfWeek = (stnow.tm_wday == 0) ? 7 : stnow.tm_wday;

  stnow.tm_mday = currentDayOfMonth;
  mktime(&stnow);

  KellyCanvas * screen = env->getCanvas();
	screen->setFont(&font18x18Config);

  // шаг по ширине при выводе дней недели 
  int dayBlockWidth; 
  int posX; int posY = baseY;

  bool baseColor = true;

  /*
    для других языков нужно будет подгонять отступы заголовков дней недели т.к. они длинней чем на кирилице
  */

  if (showTitles && strcmp_P("ru", defaultLocale) != 0) {
    showTitles = false;
  }

  if (showTitles) {    

    dayBlockWidth = font18x18Config.width + 10;

    // строчки дня недели - пн вт ср чт пт сб вс
    for (int i = 1; i <= 7; i++) {

      posX = baseX + ((i - 1) * dayBlockWidth) - 4;

      switch(i) {
        case 1:
          screen->drawString(posX, posY, FPSTR(locShortMonday), baseColor);
          break;
        case 2:
          screen->drawString(posX, posY, FPSTR(locShortTuesday), baseColor);
          break;
        case 3:
          screen->drawString(posX, posY, FPSTR(locShortWednesday), baseColor);
          break;
        case 4:
          screen->drawString(posX, posY, FPSTR(locShortThursday), baseColor);
          break;
        case 5:
          screen->drawString(posX, posY, FPSTR(locShortFriday), baseColor);
          break;
        case 6:
          screen->drawString(posX, posY, FPSTR(locShortSaturday), baseColor);
          break;      
        case 7:
          screen->drawString(posX, posY, FPSTR(locShortSunday), baseColor);
        break;
      }

    }
    
  } else {
    
    dayBlockWidth = font18x18Config.width + 6;
  }

  posY += font18x18Config.height + 4;
  int dayOfWeekCounter = firstDayOfWeek;
  String tmp;

  Serial.print(F("maxDayOfMonth : ")); Serial.println(String(maxDayOfMonth));

  for (int i = 1; i <= maxDayOfMonth; i++) {

    posX = baseX + ((dayOfWeekCounter - 1) * dayBlockWidth);
    tmp = i;

    if (i == currentDayOfMonth) {
      
	    uText dateText;
      dateText = screen->getUText(tmp);
      screen->drawRect(posX - 4, posY - 2, dateText.pixelWidth + 8, font18x18Config.height + 4, baseColor);
      screen->drawStringUtext(posX, posY, dateText, !baseColor);

    } else {

      screen->drawString(posX, posY, tmp, baseColor);
    } 

    dayOfWeekCounter++;
    if (dayOfWeekCounter > 7) {
      dayOfWeekCounter = 1;
      posY += font18x18Config.height + 4;
    }   
  }
}

void WidgetController::drawClockWidgetTiny(int baseX, int baseY) {

  KellyCanvas * screen = env->getCanvas();
  clockFormatted dt = env->getFormattedTime();
  uText dateShortText = screen->getUText(dt.timeText + " " + dt.dayText + ", " + dt.dateShort);
  partialDataSet(baseX, baseY, 80, 24);

  screen->setFont(&font18x18Config);
  screen->drawStringUtext(baseX + 10, baseY, dateShortText, true);
}

void WidgetController::drawClockWidget(int baseX, int baseY, bool border, bool fill, bool invert, int & resultWidth, int & resultHeight, uint8_t fontType) {
	
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

	uText dateText;
  if (pgm_read_byte(&textDateFormat) == 2) {
    dateText = screen->getUText(dt.monthText + "(" + dt.dayText + ")");
  } else {
    dateText = screen->getUText(dt.monthText + ", " + dt.dayText);
  }
 
	uText dateShortText = screen->getUText(dt.date);

	resultWidth = dateText.pixelWidth + 20;
  if (resultWidth < 114) {
    resultWidth = 114;
  }

  resultHeight = 92;

  if (fontType == 1) {
	  screen->setFont(&font44x44Config);    
  } else if (fontType == 2) {
    screen->setFont(&clock54x54Config);
    resultHeight+=10;
  } else if (fontType == 3) {
    screen->setFont(&clock64x64Config);
    // baseX += -6;
    resultHeight+=15;
  }

	if (fill) {
    screen->drawRoundedSquare(baseX, baseY, resultWidth, resultHeight, 4, colorBg, borderWidth);
    baseX = baseX + 8; // + ceil((float) (((resultWidth / 2) - dateText.pixelWidth)  / 2));   
    baseY = baseY + 2;
  }

	screen->color = tBLACK;
	uText timeText = screen->getUText(dt.timeText);

	if (env->hour12) {
      timeText.pixelWidth += 10;
  }

	unsigned int partialX = baseX + ceil((float) ((dateText.pixelWidth - timeText.pixelWidth) / 2));
	unsigned int partialY = baseY;
	unsigned int partialHeight = screen->font->height;

	int timePoseX = screen->drawStringUtext(partialX, partialY, timeText, colorText);	
	screen->setFont(&font18x18Config);

	if (env->hour12) {
    timePoseX = screen->drawString(timePoseX + 4, baseY + 44 - 18, dt.pm ? "pm" : "am", true);
  }
  
  unsigned int partialWidth = timePoseX - partialX + 4;
	
	if (env->lastState.updateMinutes) {
    partialDataSet(partialX, partialY, partialWidth, partialHeight);
  }

  if (fontType == 1) {
	  baseY += 44;
  } else if (fontType == 2) {
    baseY += 54;
  } else if (fontType == 3) {
    baseY += 64;
  }
	
  if (fontType != 3) {
    screen->drawStringUtext(baseX + ceil((float) ((dateText.pixelWidth - dateShortText.pixelWidth) / 2)), baseY, dateShortText, colorText);
    baseY += 17;
  }
	
	screen->drawStringUtext(baseX, baseY, dateText, colorText);  
	screen->setFont(&font18x18Config);
}

void WidgetController::drawSystemInfoWidget(int baseX, int baseY, bool shortFormat) {
  
    KellyCanvas * screen = env->getCanvas();
    screen->setFont(&font18x18Config);
    if (env->isOnBattery()) {

        // possibly some important system info about errors
        if (env->wifiInfo.length() > 0) {

          screen->drawString(baseX, baseY, env->wifiInfo, true);

        } else {
          
          int lkey = env->lastState.lastTelemetrySize-1;
          String techInfo = String(env->lastState.lastTelemetry[lkey].bat) + FPSTR(locVoltage);
          screen->drawString(baseX, baseY, techInfo, true);

          baseY += 20;
          techInfo = String(env->lastState.connectTimes) + " " + env->getFormattedExtSensorLastSyncTime();
          screen->drawString(baseX, baseY, techInfo, true);
        }

    } else {

        if (env->wifiInfo.length() > 0) {
          screen->drawString(baseX, baseY, shortFormat && env->wifiInfo.length() >= 14 ? env->wifiInfo : "IP : " + env->wifiInfo, true);    
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

String WidgetController::getPressureFormattedString(float pressure, bool hpa) {

    String result = "";
    if (pressure != -1000) {

      if (!hpa) {
        result = String((int) ((pressure / 100.0f) * 0.750062f)) + " ";
      } else {
        result = String((int) (pressure / 100.0f)) + " ";
      }

    } else {
      result = "-.- ";          
    }

    result += hpa ? FPSTR(locPressureHPA) : FPSTR(locPressureMM);

    return result;
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

    bool min = false;
    bool fill = false;
    bool invert = false;
    bool border = false;

    if (widget.params.length() > 0) {
        min = widget.params.indexOf("-m") != -1;
        fill = widget.params.indexOf("-f") != -1;
        invert = widget.params.indexOf("-i") != -1;
    }  

    if (widget.type == uiClock) {
      
      // tiny inline clock format

      if (widget.params.indexOf("-f0") != -1) {

          drawClockWidgetTiny(baseX, baseY);

      } else {

        uint8_t fontType = 1;

              if (widget.params.indexOf("-f2")  != -1) fontType = 2;
        else if (widget.params.indexOf("-f3")  != -1) fontType = 3;

        drawClockWidget(
          baseX, 
          baseY, 
          border, 
          fill, 
          invert, 
          widgetWidth, 
          widgetHeight,
          fontType
        );
      
      }

    } else if (
          widget.type == uiInfoIP || 
          widget.type == uiInfoVoltage || 
          widget.type == uiLastSyncRemote || 
          widget.type == uiShortInfoSyncRemote || 
          widget.type == uiInfoSyncNumRemote || 
          widget.type == uiPressure ||
          widget.type == uiPressureRemote ||
          widget.type == uiDate
      ) {
      
      widgetHeight = 20;
      String result = "";

      if (widget.type == uiInfoIP) {

        if (env->wifiInfo.length() > 0) {
	        result = "IP : " + env->wifiInfo;
        }

      } else if (widget.type == uiPressure || widget.type == uiPressureRemote) {
        
        float pressure = -1000;
        if (widget.type == uiPressure) {
          if (lkey > -1) {
            pressure = env->lastState.lastTelemetry[lkey].pressure;
          }
        } else {
          if (env->lastState.extData.isDataValid) {
            pressure = env->lastState.extData.pressure;
          }
        }

        bool hpa = PRESSURE_HPA;
        if (widget.params.indexOf("-hpa") != -1) hpa = true;
        result = getPressureFormattedString(pressure, hpa);

      } else if (widget.type == uiDate) {   

        clockFormatted dt = env->getFormattedTime();
        result = dt.date;

      } else if (widget.type == uiInfoVoltage) {
        
        if (lkey > -1) {
          result = String(env->lastState.lastTelemetry[lkey].bat);
          result += FPSTR(locVoltage);      
        } else {
          result = "-.-";
          result += FPSTR(locVoltage);
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

          if (fill) {
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
        if (fill) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
        }

        drawBatWidget(baseX + 20, baseY, false, widget.type == uiBatRemote ? true : false, false); // baseX started from bat icon, not count prefix text
    
    } else if (widget.type == uiCalendar) {

        widgetWidth = 216;
        widgetHeight = 216;
        drawCalendarWidget(baseX, baseY, widget.params.indexOf("-t") != -1);
        
    } else if  (widget.type == uiSCD4XHum || widget.type == uiSCD4XTemp || widget.type == uiSCD4XCO2) {

      #if defined(CO2_SCD41)  
      widgetWidth = 152;
      widgetHeight = 54;
      if (min) {
        widgetHeight = 20;
      }

      if (fill) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
      }

      if (env->scd4XCO2 == 0) {
        
        screen->drawString(baseX, baseY, "no data (SCD4X)", true);
        
      } else {

            char buffer[128];
            if (widget.type == uiSCD4XTemp) {
                    
                if (!min) {
               
                  screen->setFont(&font44x44Config);
                  sprintf(buffer, "%.1f", !env->celsius ? env->toFahrenheit(env->scd4XTemp) : env->scd4XTemp);
                  baseX = screen->drawString(baseX, baseY + 20, buffer, true);  

                  if (env->celsius) {
                    screen->drawImage(10 + baseX, baseY + 20 + 4, &cels_39x43bw_settings, true); // Celsius glyph symbol    
                  } else {
                    screen->drawImage(6 + baseX, baseY + 20 + 4, &fahr_39x43bw_settings, true); // Fahrenheit glyph symbol    
                  }

                } else {

                  if (env->celsius) sprintf(buffer, "%.1fC",  env->toFahrenheit(env->scd4XTemp));
                  else sprintf(buffer, "%.1fF", env->scd4XTemp);
                  
                  screen->drawString(baseX, baseY, buffer, true);
                }


            } else if (widget.type == uiSCD4XHum)  {
              
                  if (!min) {
                     screen->setFont(&font44x44Config);
                  }
                  
                  sprintf(buffer, "%.1f%%", env->scd4XHumidity);  
                  screen->drawString(baseX, baseY, buffer, true);

            } else if (widget.type == uiSCD4XCO2)  {
              
                  if (!min) {
                     screen->setFont(&font44x44Config);
                  }
                  
                  String result = String(env->scd4XCO2);
                  result += " ";
                  result += FPSTR(locCO2);

                  screen->drawString(baseX, baseY, result, true);
            }
      }
      #endif
      
    } else if  (widget.type == uiTemp || widget.type == uiTempRemote || widget.type == uiHumRemote || widget.type == uiHum) {
        
        widgetWidth = 152;
        widgetHeight = 70;

        if (min) {
          widgetHeight = 20;
        }
          
        if (fill) {
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
                    
                    if (!env->celsius) {
                      t = env->toFahrenheit(t);
                    }

                    if (!min) {

                      screen->drawString(baseX, baseY, env->getFormattedSensorTitle(!extSens), true);                
                      screen->setFont(&font44x44Config);
                      sprintf(buffer, "%.1f", t);
                      baseX = screen->drawString(baseX, baseY + 20, buffer, true);  

                      // todo - include to font
                      if (env->celsius) {
                        screen->drawImage(10 + baseX, baseY + 20 + 4, &cels_39x43bw_settings, true); // Celsius glyph symbol    
                      } else {
                        screen->drawImage(6 + baseX, baseY + 20 + 4, &fahr_39x43bw_settings, true); // Fahrenheit glyph symbol    
                      }

                    } else {

                      if (env->celsius) sprintf(buffer, "%.1fC", t);
                      else sprintf(buffer, "%.1fF", t);
                      
                      screen->drawString(baseX, baseY, buffer, true);
                    }

                }

            } else {
              
              if (h > -1000) {

                  if (!min) {
                     screen->drawString(baseX, baseY, FPSTR(locHumidity), true); 
                     screen->setFont(&font44x44Config);
                     baseY += 20;
                  }
                  
                  sprintf(buffer, "%.1f%%", h);  
                  screen->drawString(baseX, baseY, buffer, true);
              }
            }
        }
    } else if (widget.type == uiInfo) {

        widgetWidth = 102;
        widgetHeight = 36;
        
        if (fill) {
          screen->drawRoundedSquare(baseX-2, baseY-2, widgetWidth+2, widgetHeight+2, 4, false, 0);
        }

        if (widget.params.indexOf("border") != -1) {
          int padding = 1;
          screen->drawRoundedSquare(baseX-padding, baseY-padding, widgetWidth+(padding * 2), widgetHeight+(padding * 2), 4, false);
        }

        drawSystemInfoWidget(baseX, baseY);
    }
}