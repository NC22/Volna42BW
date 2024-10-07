#include <Screen1in54UI.h>

#if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)

Screen1in54UI::Screen1in54UI(Env * nenv) {

    #if defined(HELTEC_BW_15_S810F)
      displayDriver = new KellyEInk_15_SSD1683_BW(EPD_BUSY_PIN, EPD_RST_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
    #elif defined(WAVESHARE_R_BW_15_SSD1683)
      displayDriver = new KellyEInk_15_SSD1683_RBW(EPD_BUSY_PIN, EPD_RST_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
    #endif

    env = nenv;
    constPowerTimerStart = millis();    
    widgetController = new WidgetController();
    widgetController->setEnv(env);
}

bool Screen1in54UI::is4ColorsSupported() {
  
  #if defined(COLORMODE_2BIT_SUPPORT)
      return displayDriver->colorMode2bitSupport;
  #endif

  return false;  
}

int Screen1in54UI::drawTemp(int theight, bool indoor, float temperature, float humidity, float pressure, imageDrawModificators & mods) {
  
  KellyCanvas * screen = env->getCanvas();
  screen->color = tBLACK;

  if (env->noScreenTest) {
    humidity = 33.6;
    temperature = 22.4;
  } 

  String title;

  if (temperature <= -1000) {
    title = FPSTR(locUnavailable);
    humidity = 0.0;
    temperature = -10;
  } else {

    title = env->getFormattedSensorTitle(indoor);
    if (!env->celsius) {
      temperature = env->toFahrenheit(temperature);
    }
    
  }

  int twidthTitle = 178;
  char buffer[128];

  // titles

  screen->setFont(&font18x18Config);
  screen->drawString(10, theight - 20, title, false);
  
  bool showPressure = false;
  if (pressure > -1000) {
    if (indoor) {
      showPressure = DUI_PRESSURE_HOME;
    } else {
      showPressure = DUI_PRESSURE_EXTERNAL;
    }
  } 

  bool showCO4 = false;
  #if defined(CO2_SCD41) && defined(DUI_CO4)
    if (indoor) {
      showCO4 = true;
      showPressure = false;
    }
  #endif

  if (showPressure) {
    screen->drawString(10, theight + 70 - 20,  widgetController->getPressureFormattedString(pressure, PRESSURE_HPA), false);

  } else if (showCO4) {

    #if defined(CO2_SCD41)  

    if (!env->updateSCD4X()) {

      screen->drawString(10, theight + 70 - 20, "no data (SCD4X)", false);

    } else {

      String result = String(env->scd4XCO2);
      result += " ";
      result += FPSTR(locCO2);

      screen->drawString(10, theight + 70 - 20, result, false);
    }

    #endif

  } else {
    screen->drawString(10, theight + 70 - 20,  FPSTR(locHumidity), false);
  }

  // big letters metrics

  screen->setFont(&font44x44Config);

  sprintf(buffer, "%.1f", temperature);
  int twidth = screen->drawString(10, theight, buffer, false);  

  if (env->celsius) {
    screen->drawImage(10 + twidth, theight + 4, &cels_39x43bw_settings, false); // Celsius glyph symbol
  } else {
    screen->drawImage(6 + twidth, theight + 4, &fahr_39x43bw_settings, false); // Fahrenheit glyph symbol    
  }

  sprintf(buffer, "%.1f%%", humidity > -1000 ? humidity : 0.0f);  
  screen->drawString(10, theight + 70, buffer, false);

  // temperature bar icon

  twidth += 39;

  float tempMax = 42.0; float tempMin = -20.0; 

  if (!env->celsius) {
    tempMax = env->toFahrenheit(tempMax);
    tempMin = env->toFahrenheit(tempMin);
  }

  float tempPercent = temperature;

  if (tempPercent > tempMax) tempPercent = tempMax;
  if (tempPercent < tempMin) tempPercent = tempMin;

  tempPercent += abs(tempMin);
  float tempPerPercent = (abs(tempMax) + abs(tempMin)) / 100.0;

  tempPercent = tempPercent / tempPerPercent;
  if (tempPercent < 10) tempPercent = 10.0;
  if (tempPercent > 100) tempPercent = 100.0;

  float pixelsPerPercent = 48.0 / 100.0; // icon 48 pixels max  

  // 16 + twidth
  screen->drawImage(twidthTitle - 26, theight - 4, &temp_15x48bw_settings, false);
     
  screen->color = tRY;
  screen->resetImageMods(mods, true, false, false, -1, -1, 48 - (int) round(pixelsPerPercent * tempPercent), 48);   
  screen->drawImageMods(twidthTitle - 26, theight - 4, &temp_meter_15x48bw_settings, mods, true); 
  screen->color = tBLACK; 

  theight += 34;

  return theight;
}

bool Screen1in54UI::drawUIToBufferCustom() {

    if (!env->cuiReadStorageFile()) {
        // env->getCanvas()->drawString(20, 20, "Not found :" + env->cuiName, true);
        return false;
    }

    for(unsigned int i=0; i < env->cuiWidgets.size(); i++) {
        
        if (partial && env->cuiWidgets[i].type != uiClock) continue;

        if (env->cuiWidgets[i].enabled) {
          Serial.println(F("DRAW WIDGET"));  
    
          widgetController->drawWidget(env->cuiWidgets[i]);
        } else 
          Serial.println(F("skip WIDGET"));  
    } 

    return true;
}

void Screen1in54UI::drawUILowBat() {

  KellyCanvas * screen = env->getCanvas();

  if (clearRequired) screen->clear();

  screen->drawString(10, 10, FPSTR(locLowBat), true);
}

// todo separate classes for screens

void Screen1in54UI::drawUIToBuffer() {

  env->cuiApplyLoop();

  if (env->cuiIsEnabled()) {
    if (drawUIToBufferCustom()) return;
  }

  KellyCanvas * screen = env->getCanvas();
  screen->setBitsPerPixel(1);
  screen->clear();
  
  screen->setFont(&font18x18Config);
  screen->setRotate(0);
  clearRequired = true;

  screen->color = tBLACK;

  imageDrawModificators mods;

  // ui borders & frames

  screen->drawRect(2, 2, screen->width - 4, screen->height - 4, true); // black top block
  screen->drawRect(2, 2, 50, screen->height - 4, false); // bottom white

  screen->drawRect(2, 2 + 5, 2, screen->height - 14, true);
  screen->drawRect(2 + 5, 2, screen->width - 16, 2, true);
  screen->drawRect(2 + 5, screen->height - 4, screen->width - 16, 2, true);

  screen->drawImage(0, 0, &corner_line_7x7bw_settings, true, true);

  screen->resetImageMods(mods, true, false, true); 
  screen->drawImageMods(0, screen->height - 7 - 1, &corner_line_7x7bw_settings, mods, true, true);

  screen->resetImageMods(mods, true, true, false); 
  screen->drawImageMods(screen->width - 2 - 7, 1, &corner_top_7x7bw_settings, mods, false);

  screen->resetImageMods(mods, true, true, true); 
  screen->drawImageMods(screen->width - 2 - 7, screen->height - 2 - 7, &corner_top_7x7bw_settings, mods, false);

  screen->setRotate(90);  

  int localWidth = 200;
  int localHeight = 200;
  int theight = 20; 

  int lkey = env->lastState.lastTelemetrySize-1;
  if (env->lastState.lastTelemetrySize > 0) {

      // sensors 

      theight += drawTemp(theight, true, env->lastState.lastTelemetry[lkey].temperature, env->lastState.lastTelemetry[lkey].humidity, env->lastState.lastTelemetry[lkey].pressure, mods);

      // battery

      theight = localHeight - 50;
      int batPosX = 46;
      
      int chargeLvl = 100;
      int minChargePixels = 10;
      int maxChargePixels = 36;
      int chargePixelsTotal = maxChargePixels - minChargePixels;

      if (env->isOnBattery()) {
          chargeLvl = (int) round(env->getBatteryLvlfromV(env->lastState.lastTelemetry[lkey].bat));
      }

      int chargePixels = chargePixelsTotal - (int) round(((float) chargePixelsTotal / 100.0) * chargeLvl);
      
      Serial.println("Charge pixels : " + String(chargePixels) + " | " + String(chargeLvl) + "%");   

      screen->setFont(&font18x18Config);
      String techInfo = "";

      if (env->isOnBattery()) {

          batPosX = 48;
          screen->drawString(batPosX - 39, theight, String(chargeLvl) + "%", true);
          // techInfo += String(env->lastState.lastTelemetry[lkey].bat) + "V";
          // techInfo += " #" + String(env->lastState.connectTimes) + " " + env->getFormattedExtSensorLastSyncTime();

          screen->drawString(10, theight, techInfo, true);

      } else {

          // todo - replace to wifiInfo - always display server ip if WiFi enabled
          if (env->wifiInfo.length() > 0) {
             techInfo += "IP : " + env->wifiInfo;
          }

          screen->drawImage(batPosX - 34, theight - 10, &plug_23x39bw_settings, true);
      }     

      if (techInfo.length() > 0) {
        uText techInfoUtxt = screen->getUText(techInfo);
        screen->drawStringUtext(localWidth - techInfoUtxt.pixelWidth - 8, theight, techInfoUtxt, true);
      }

      if (env->isOnBattery()) {
        screen->drawImage(batPosX, theight - 10, &bat_43x39bw_settings, true);  
        // screen->color = tRY;

        screen->resetImageMods(mods, true, false, false, minChargePixels + chargePixels, maxChargePixels); 
        screen->drawImageMods(batPosX, theight - 10, &bat_meter_43x39bw_settings, mods, true);
      }

  } else {

      theight += drawTemp(theight, true, -1000, 0, 0, mods);
  }

  // time & date

  screen->color = tBLACK;
            
  theight = localHeight - 28;          

  clockFormatted dt = env->getFormattedTime();
  uText dateShortText = screen->getUText(dt.timeText + " " + dt.dayText + ", " + dt.dateShort);
  widgetController->partialDataSet(0, theight, 80, 24);

  screen->setFont(&font18x18Config);
  screen->drawStringUtext(10, theight, dateShortText, true);
}

void Screen1in54UI::drawClock() {

  KellyCanvas * screen = env->getCanvas();

  if (clearRequired) screen->clear();
  clearRequired = true;

  screen->setFont(&font18x18Config);
  screen->setRotate(90);

  screen->drawImage(0, 0, &frog_magic_hat_186x141bw_settings, true);  // frog_new_year_hat174x139_settings

  clockFormatted dt = env->getFormattedTime();
  uText dateShortText = screen->getUText(dt.monthText);

  screen->drawStringUtext(ceil((float) ((200 - dateShortText.pixelWidth) / 2)), 200-64, dateShortText, true);

  screen->setFont(&font44x44Config);
  dateShortText = screen->getUText(dt.timeTextS);

  //tstrX = ceil((float) ((200 - dateShortText.pixelWidth) / 2));
  //tstrY = 200-48;

  tstrX = 8;
  tstrY = 160;

  tstrW = 184;
  tstrH = 40;

  widgetController->partialDataSet(tstrX, tstrY, tstrW, tstrH);

  tstrXend = tstrX + tstrW; // dateShortText.pixelWidth + offset;
  tstrYend = tstrY + tstrH;

  ttextWidth = ceil((float) ((200 - dateShortText.pixelWidth) / 2));
  screen->drawStringUtext(ttextWidth, tstrY-4, dateShortText, true);

 // tstrY += 11; // font 44 - 15 pixels of white space (max digit height = 29)
 // tstrY -= offset/2;  
 // tstrX -= offset/2;



  Serial.println("DRAW CLOCK");

	Serial.println(String(tstrX) + " - " + String(tstrY) + " - " + String(tstrXend) + " - " + String(tstrYend));
  //screen->drawRect(tstrX, tstrY, tstrW, tstrH, true);
}

void Screen1in54UI::updateClock() {
  if (tstrY > -1) {
    
    if (widgetController->clockPartial.xStart == -1 && widgetController->clockPartial.xEnd == -1) {
        Serial.println(F("[drawTestPartial] no clock data"));  
        return;
    }

    KellyCanvas * screen = env->getCanvas();
    
    screen->drawRect(10, tstrY, 190, tstrH, false);
    uText dateShortText = screen->getUText(env->getFormattedTime().timeTextS);

    screen->drawStringUtext(ttextWidth, tstrY-4, dateShortText, true);

    // screen->drawString(tstrX+20, tstrY-4, dateShortText, true);

    // int scrX = tstrX;
    //int scrY = tstrY;
    //int scrXend = tstrXend;
    //int scrYend = tstrYend;
    //screen->applyXYMods(scrX, scrY);
    //screen->applyXYMods(scrXend, scrYend);

    displayDriver->displayPartial(
      screen->bufferBW, 
      env->lastState.lastPartialPos.xStart, 
      env->lastState.lastPartialPos.yStart, 
      env->lastState.lastPartialPos.xEnd, 
      env->lastState.lastPartialPos.yEnd
    );

    // displayDriver->displayPartial(screen->bufferBW, tstrX, tstrY, tstrXend, tstrYend, true, true);
    // displayDriver->displayPartial(screen->bufferBW, tstrX, tstrY, tstrXend, tstrYend);
    // spiController.SetFrameMemory(screen->bufferBW, scrX, scrY, screen->getWidth(), screen->getHeight(), scrX, scrY, scrXend, scrYend);
    // spiController.HalLcd_Partial_Update();
  }
}

void Screen1in54UI::enableClockMode(bool state) {
  
  env->resetPartialData();

  if (state) {
 
    clockModeState = clockModePrepare;
    constPowerTimerStart = millis();
    printf("Screen1in54UI prepaer to enter clock mode (wait web request end) \r\n");

  } else {
        
        clockModeState = clockModeDisabled;
        env->updateTelemetry();
        env->sync();

        drawUIToBuffer();

        printf("Screen1in54UI draw Sensors info \r\n");
        if (!env->noScreenTest) {
          updateScreen();
        }

  }

}

bool Screen1in54UI::tick() {

    if (clockModeState == clockModeDisabled) return false;

    unsigned long now = millis();
    if (clockModeState == clockModePrepare && now - constPowerTimerStart >= 1500) {

        constPowerTimerStart = millis();
        clockModeState = clockModeEnabled;
        clockTime = time(nullptr);
        Serial.println(String(clockTime));

        env->updateTime(clockTime);
        drawClock();

        if (!env->noScreenTest) {
          updateScreen();
        }
        printf("Screen1in54UI draw clock \r\n");

    } else if (clockModeState == clockModeEnabled && now - constPowerTimerStart >= 15000) {

      constPowerTimerStart = millis();
      enableClockMode(false);

    } else if (clockModeState == clockModeEnabled) {

        // update clock seconds by partial update function without sleep
        if (now - partialRefreshStart >= 1000) {
            partialRefreshStart = now;
            clockTime += 1;

            Serial.println(String(clockTime));
            env->updateTime(clockTime);

            if (!env->noScreenTest) {
              updateClock();
            }
            
        }
    }

    /* old switch mode format

      unsigned long now = millis();
      if (now - constPowerTimerStart >= 15000) {

        constPowerTimerStart = millis();
        enableClockMode(false);
        if (currentMode == scTClock) currentMode = scTSensors;
        else {
          currentMode = scTClock;
          partialRefreshStart = constPowerTimerStart;
        }

        if (currentMode == scTSensors) {

          env->updateTelemetry();
          env->sync();

          drawUIToBuffer();

          printf("Screen1in54UI draw Sensors info \r\n");

        } else {

          clockTime = time(nullptr);
          Serial.println(String(clockTime));

          env->updateTime(clockTime);
          drawClock();

          printf("Screen1in54UI draw clock \r\n");
        }

        if (!env->noScreenTest) {
          updateScreen();
        }

      } else {
        
        // update clock seconds by partial update function without sleep

        if (currentMode == scTClock && now - partialRefreshStart >= 1000) {
            partialRefreshStart = now;
            clockTime += 1;

            Serial.println(String(clockTime));

            env->updateTime(clockTime);
            if (!env->noScreenTest) {
              updateClock();
            }
            
        }

      }
    */
    return true;
}

void Screen1in54UI::initPins() {
  if (!initPinsState) {
    
    displayDriver->initPins();
    //  spiController.EPD_Init();
      initPinsState = true;
  }
}

void Screen1in54UI::clearScreen() {

  if (displayBeasy) return;
  displayBeasy = true;

  // spiController.EPD_Init();
  // spiController.Clear();
  // spiController.EPD_DeepSleep(); 

  initPins(); 
  displayDriver->displayInit();
  displayDriver->displayClear();
  displayDriver->displaySleep();

  printf("Goto Sleep...\r\n");

  displayBeasy = false;
}

void Screen1in54UI::updatePartialClock() {

    partial = true;
    drawUIToBuffer();
    partial = false;

    if (widgetController->clockPartial.xStart == -1 && widgetController->clockPartial.xEnd == -1) {
        Serial.println(F("[drawTestPartial] no clock data"));  
        return;
    }
    
    initPins(); 

    // spiController.EPD_Init();    
    displayDriver->displayInit(1, true);

    delay(100);


    widgetController->partialDataApplyMaxBounds();
    KellyCanvas * screen = env->getCanvas();

    displayDriver->displayPartial(
      screen->bufferBW, 
      env->lastState.lastPartialPos.xStart, 
      env->lastState.lastPartialPos.yStart, 
      env->lastState.lastPartialPos.xEnd, 
      env->lastState.lastPartialPos.yEnd, 
      true, 
      true
    );

    displayDriver->displayPartial(
      screen->bufferBW, 
      env->lastState.lastPartialPos.xStart, 
      env->lastState.lastPartialPos.yStart, 
      env->lastState.lastPartialPos.xEnd, 
      env->lastState.lastPartialPos.yEnd
    );

    env->lastState.lastPartialPos = widgetController->clockPartial;
  // spiController.SetFrameMemory(screen->bufferBW, widgetController->clockPartial.xStart, widgetController->clockPartial.yStart, screen->getWidth(), screen->getHeight(), widgetController->clockPartial.xStart, widgetController->clockPartial.yStart, widgetController->clockPartial.xEnd, widgetController->clockPartial.yEnd);
    // spiController.HalLcd_Partial_Update();


    env->lastState.lastPartialPos = widgetController->clockPartial;
    // spiController.EPD_DeepSleep(); 

    displayDriver->displaySleep();
}

void Screen1in54UI::updateScreen() {

  if (displayBeasy) return;
  displayBeasy = true;

  printf("e-Paper Init and Clear...\r\n");
  // spiController.EPD_Init();
  initPins(); 
  displayDriver->displayInit();


 /*+++*/ // EPD_4IN2B_V2_Init();
 /*+++*/ // DEV_Delay_ms(400);
  
  // show image for array    
  printf("show image for array\r\n");
  
  KellyCanvas * screen = env->getCanvas();

  // screen->color = tBLACK;
  // screen->inverse();

  if (clockModeState == clockModeEnabled) {

      printf("show image for array in partial mode\r\n");
      
      displayDriver->display(screen->bufferBW);
      displayDriver->displayInit(1, true);
      
      // spiController.DisplayPartBaseImage(screen->bufferBW);
      // delay(100);
      // spiController.HalLcd_Partial();

      constPowerTimerStart = millis();

  } else {
      printf("show image static\r\n");
      // spiController.EPD_ALL_image(screen->bufferBW); 
      
      displayDriver->display(screen->bufferBW);      
      constPowerTimerStart = millis(); 
  }

  displayBeasy = false;
  displayDriver->displaySleep();

  // spiController.EPD_DeepSleep(); 
}

#endif