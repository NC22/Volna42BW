#include <Screen4in2UI.h>

#if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683)

/*
  Todo  
  Частичное обновление отключено для 2-bit режима тк нет совместимости (нужно выводить все равно в 1 битном режиме) и работает криво
*/

Screen4in2UI::Screen4in2UI(Env * nenv) {

    #if defined(WAVESHARE_BW_42_UC8176) 
    #include "KellyEInk_42_UC8176_BW.h"
    displayDriver = new KellyEInk_42_UC8176_BW(EPD_BUSY_PIN, EPD_RST_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
    #elif defined(WAVESHARE_BW_42_SSD1683)
    #include "KellyEInk_42_SSD1683_BW_2BIT.h"
    displayDriver = new KellyEInk_42_SSD1683_BW_2BIT(EPD_BUSY_PIN, EPD_RST_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
    #elif defined(WAVESHARE_RY_BW_42_UC8176)
    #include "KellyEInk_42_UC8176_RBW.h"
    displayDriver = new KellyEInk_42_UC8176_RBW(EPD_BUSY_PIN, EPD_RST_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_CLK_PIN, EPD_DIN_PIN);
    #endif

    env = nenv;
    widgetController = new WidgetController();
    widgetController->setEnv(env);


}

bool Screen4in2UI::tick() {
  return false;
}

int Screen4in2UI::drawTemp(int theight, bool indoor, float temperature, float humidity, imageDrawModificators & mods, bool land) {
  
  KellyCanvas * screen = env->getCanvas();
  screen->color = tBLACK;

  if (temperature > 1000 || temperature < -1000) temperature = -1000;
  if (humidity > 1000 || humidity < -1000) humidity = -1000;

  if (!env->celsius) {
    temperature = env->toFahrenheit(temperature);
  }

  int humMarginX = 178;
  int humMarginY = 0;
  int tempMarginX = 10;

  if (land) {
    tempMarginX = localWidth / 2;
    tempMarginX += 10;
    humMarginX = tempMarginX;
    humMarginY = 70;
  }

  String title = FPSTR(indoor ? locIndoor : locOutdoor); // todo - optionaly custom string by config possible

  if (temperature <= -1000) {

    screen->drawString(tempMarginX, theight - 20, FPSTR(locUnavailable), false);
    return theight + 34;
  }

  char buffer[32];

  // titles
  screen->setFont(&font18x18Config);
  int textMarginX = screen->drawString(tempMarginX, theight - 20, title, false);
  if (!indoor) {    
    widgetController->drawBatWidget(textMarginX + 2, theight - 28, true, true, true);
  }

  
  if (humidity > -1000) {
    screen->drawString(humMarginX, theight - 20 + humMarginY, FPSTR(locHumidity), false);
  }

  // big letters metrics

  screen->setFont(&font44x44Config);

  sprintf(buffer, "%.1f", temperature);
  int twidth = screen->drawString(tempMarginX, theight, buffer, false);  

  if (env->celsius) {
    screen->drawImage(10 + twidth, theight + 4, &cels_39x43bw_settings, false); // Celsius glyph symbol    
  } else {
    screen->drawImage(6 + twidth, theight + 4, &fahr_39x43bw_settings, false); // Fahrenheit glyph symbol    
  }

  if (humidity > -1000) {
      sprintf(buffer, "%.1f%%", humidity);  
      screen->drawString(humMarginX, theight + humMarginY, buffer, false);
  }

  // temperature bar icon

  twidth += 39;

  float tempMax = 42.0; float tempMin = -20.0; 
  float tempPercent = temperature;

  if (tempPercent > tempMax) tempPercent = tempMax;
  if (tempPercent < tempMin) tempPercent = tempMin;

  tempPercent += abs(tempMin);
  float tempPerPercent = (abs(tempMax) + abs(tempMin)) / 100.0;

  tempPercent = tempPercent / tempPerPercent;
  if (tempPercent < 10) tempPercent = 10.0;
  if (tempPercent > 100) tempPercent = 100.0;

  float pixelsPerPercent = 68.0 / 100.0; // icon 68 pixels max  

  screen->drawImage(land ? twidth + 29 : humMarginX - 26, theight - 20, &temp_15x68bw_settings, false);
     
  screen->color = tRY;       
  screen->resetImageMods(mods, true, false, false, -1, -1, 68 - (int) round(pixelsPerPercent * tempPercent), 68); 
  screen->drawImageMods(land ? twidth + 29 : humMarginX - 26, theight - 20, &temp_meter_15x68bw_settings, mods, true); 

  screen->color = tBLACK; 

  theight += 34;

  return theight;
}

/*
void Screen4in2UI::drawUISysNotice(String text, int x, int y, int id) {

  KellyCanvas * screen = env->getCanvas();

  if (clearRequired) screen->clear();

  screen->font = font18x18Config;
  screen->drawString(x, y, text, true);
}
*/

void Screen4in2UI::drawUILowBat(bool land) {

  KellyCanvas * screen = env->getCanvas();

  if (clearRequired) screen->clear();

  if (!land) screen->setRotate(90);
  else screen->setRotate(0);

  screen->setFont(&font18x18Config);
  screen->drawString(34, 34, FPSTR(locLowBat), true);
  screen->drawImage(localWidth - 150, localHeight - 132 - 2, &cat_feed_125x127bw_settings, true);
  
}

// reset rotation & color & clear screen & prepare draw frame & round corners

void Screen4in2UI::drawUIInit(int w, int h, imageDrawModificators & mods) {

  KellyCanvas * screen = env->getCanvas();
  screen->clear();

  localWidth = w;
  localHeight = h;

  screen->setFont(&font18x18Config);
  screen->setRotate(0);
  clearRequired = true;

  screen->color = tBLACK;

  if (!partial) {
      // ui borders & frames

      screen->drawRect(2, 2, screen->width - 4, screen->height - 4, true); // black top block in portrait mode (right black half in land)
      screen->drawRect(2, 2, screen->width / 2, screen->height - 4, false); // bottom white in portrait mode (left white in land)

      screen->drawRect(2, 2 + 5, 2, screen->height - 14, true);
      screen->drawRect(2 + 5, 2, screen->width - 16, 2, true);
      screen->drawRect(2 + 5, screen->height - 4, screen->width - 16, 2, true);

      // round corners

      screen->drawImage(0, 0, &corner_line_7x7bw_settings, true, true);

      screen->resetImageMods(mods, true, false, true); 
      screen->drawImageMods(0, screen->height - 7 - 1, &corner_line_7x7bw_settings, mods, true, true);

      screen->resetImageMods(mods, true, true, false); 
      screen->drawImageMods(screen->width - 2 - 7, 1, &corner_top_7x7bw_settings, mods, false);

      screen->resetImageMods(mods, true, true, true); 
      screen->drawImageMods(screen->width - 2 - 7, screen->height - 2 - 7, &corner_top_7x7bw_settings, mods, false);
  }
}

bool Screen4in2UI::drawUIToBufferCustom() {

    /* ESP8266 - cant reallocate more memory & canvas already allocated, restart */

    if (!partial && env->cuiPrepareRebootIfNeeded()) {
        env->restart();
        return false;
    }

    /*
      RTC mem of esp8266 is not very big, so its possible that for esp32 can be implemented store of all last widgets in memo, so we can reuse 
      only clock settings without reload all data from flash 

      if (partial && env->lastState.lastPartialPos.xStart != -1 && env->lastState.lastPartialPos.xEnd != -1) {
          //widgetController->drawWidget(savedClockWidgetSettings);
          widgetController->drawClockWidget(int baseX, int baseY, bool border, bool fill, bool invert, int & resultWidth, int resultHeight)
          return;
      }
    */

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

void Screen4in2UI::drawUIToBufferLand() {
  
  imageDrawModificators mods;
  KellyCanvas * screen = env->getCanvas();
  drawUIInit(400, 300, mods);

  int theight = 26; 
  int lkey = env->lastState.lastTelemetrySize-1;
  
  if (!partial) {
      
    if (env->lastState.lastTelemetrySize > 0) {

        // sensors 

        drawTemp(theight, true, env->lastState.lastTelemetry[lkey].temperature, env->lastState.lastTelemetry[lkey].humidity, mods, true);
                  
        if (env->lastState.extData.isDataValid) {
          
          screen->drawRect(localWidth / 2, localHeight / 2, localWidth / 2, 2, false);  // split line
          theight = (localHeight / 2) + 26;

          drawTemp(theight, false, env->lastState.extData.temperature, env->lastState.extData.humidity, mods, true);
          
          //uText timeText = screen->getUText(dt.timeText); pixelWidth
          
        }

        int batPosX = (localWidth / 2) - 50;
        widgetController->drawBatWidget(batPosX, 0, false, false, false);
        widgetController->drawSystemInfoWidget(10, 5);

    } else {

      drawTemp(theight, true, -1000, 0, mods);
    }

  }
  // time & date & cat icon

  screen->color = tBLACK;
  clockFormatted dt = env->getFormattedTime();

  drawCat(true);

  theight = 56; // localHeight - drawCat(true) - 80;

  uText dateText = screen->getUText(dt.monthText + ", " + dt.dayText);
  int baseX = ceil((float) (((localWidth / 2) - dateText.pixelWidth)  / 2));
  int rwidth = -1; int rheight = -1;
  widgetController->drawClockWidget(baseX, theight, false, false, false, rwidth, rheight);
}

// todo separate classes for screens

void Screen4in2UI::drawUIToBuffer() {

  env->cuiApplyLoop();
  
  // bits per pixel identifyed form custom UI file
  if (env->cuiIsEnabled()) {

    if (drawUIToBufferCustom()) return;
  }

  KellyCanvas * screen = env->getCanvas();
  screen->setBitsPerPixel(1);

  #if defined(DOUBLE_BUFFER_MODE) // optional for displays with double buffer
  screen->initBuffer(2);
  #endif

  if (env->getConfig()->getBool(cScreenLandscape)) {
    drawUIToBufferLand();
    return;
  }

  imageDrawModificators mods;

  drawUIInit(300, 400, mods);
  screen->setRotate(90);  

  int theight = 34; 
  int lkey = env->lastState.lastTelemetrySize-1;

  if (!partial) {

    if (env->lastState.lastTelemetrySize > 0) {

        // sensors 
        theight += drawTemp(theight, true, env->lastState.lastTelemetry[lkey].temperature, env->lastState.lastTelemetry[lkey].humidity, mods);
                  
        if (env->lastState.extData.isDataValid) {
          
          screen->drawRect(0, theight, 2, localWidth, false);  // split line
          drawTemp(theight + 34, false, env->lastState.extData.temperature, env->lastState.extData.humidity, mods);
        }

        theight = localHeight / 2 + 2;
        int batPosX = localWidth - 17 - 40;
        widgetController->drawBatWidget(batPosX, theight - 7, false, false, false);
        widgetController->drawSystemInfoWidget(10, theight);
        
    } else {
        
        drawTemp(theight, true, -1000, 0, mods);
    }

  }

  theight = localHeight - 100;       
  int rwidth = -1; int rheight = -1;
  widgetController->drawClockWidget(10, theight, false, false, false, rwidth, rheight);

  // env->lastState.lastTelemetry[lkey].pressure
  // hPa -> мм. рт. ст. parseFloat(hpa) * 0.750062
  // отслеживать резкое изменение давления - вывод?

  // weather icon -- todo

  // screen->drawImage(localWidth - 125, localHeight - 125 - 2, &catSnowflakes_115x125bw_settings, true);    
   drawCat(false); 
}

void Screen4in2UI::updatePartialClock() {
	
    if (env->canvas->bitPerPixel > 1) return;
    
    /*
      // bit hacky 
      // partial update not support 2-bit images & canvas
      unsigned int origBits = env->canvas->bitPerPixel;
      if (env->canvas->bitPerPixel > 1) {
          origBits = env->canvas->bitPerPixel;
          env->canvas->bitPerPixel = 1;
      }
      // cuiWidgetsOnly = false;
      // env->canvas->bitPerPixel = origBits;
    */

    partial = true;
    drawUIToBuffer();
    partial = false;

    if (  widgetController->clockPartial.xStart == -1 && widgetController->clockPartial.xEnd == -1) {
        Serial.println(F("[drawTestPartial] no clock data"));  
        return;
    }
    
    initPins(); 
    displayDriver->displayInit(1, true);  
    delay(100);

    KellyCanvas * screen = env->getCanvas();
    
    // заполняем буфер контроллера дисплея, без обновления, чтобы он проинициализировал оперативку после сна, иначе может рандомно высыпать белый шум на весь экран
    #if defined(WAVESHARE_BW_42_SSD1683)
    displayDriver->display(screen->bufferBW, NULL, false);  
    #endif

    Serial.println(F("[drawPartial] redraw part CLOCK"));  
    
    // исключаем ситуацию когда прошлая область может быть больше новой - чтобы не осталось старого не перересованного участка
    // если не выщитывать, тогда просто нужно брать с запасом в несколько пикселей

    widgetController->partialDataApplyMaxBounds();

    // for proper refresh background
    #if defined(WAVESHARE_BW_42_SSD1683)
    displayDriver->displayPartial(
      screen->bufferBW, 
      env->lastState.lastPartialPos.xStart, 
      env->lastState.lastPartialPos.yStart, 
      env->lastState.lastPartialPos.xEnd, 
      env->lastState.lastPartialPos.yEnd, 
      true, 
      true
    );
    #endif

    // reduce ghosting for R + BW disp, but this possibly can be fixed by more proper LUT tables
    // #if defined(WAVESHARE_RY_BW_42_UC8176)
    // EPD_4IN2B_V2_Display_Partial(
    //  NULL, 
    //  env->lastState.lastPartialPos.xStart, 
    //  env->lastState.lastPartialPos.yStart, 
    //  env->lastState.lastPartialPos.xEnd, 
    //  env->lastState.lastPartialPos.yEnd, 
    //  true
    // );
    // #endif

    displayDriver->displayPartial(
      screen->bufferBW, 
      env->lastState.lastPartialPos.xStart, 
      env->lastState.lastPartialPos.yStart, 
      env->lastState.lastPartialPos.xEnd, 
      env->lastState.lastPartialPos.yEnd
    );

    env->lastState.lastPartialPos = widgetController->clockPartial;
    displayDriver->displaySleep();
}

int Screen4in2UI::getDevidedBy8Num(int x, bool upper) {

    if (upper) x = x + (8 - (widgetController->clockPartial.yEnd % 8));
    else x = x - (x % 8);
    return x;
}

void Screen4in2UI::updateTestPartial2() {
  KellyCanvas * screen = env->getCanvas();

  localWidth = 400;
  localHeight = 300;

  screen->setBitsPerPixel(1);
  screen->clear();
  screen->setFont(&font44x44Config);
  screen->setRotate(0);
  screen->color = tBLACK;

  initPins(); 
  displayDriver->displayInit(); 
  Serial.println("[drawTestPartial] draw CAT");  
  int catPosX = 280;
  int catPosY = localHeight - 118;
  screen->drawImage(catPosX, catPosY, &cat_watch_85x120bw_settings, true);

  uText testText = screen->getUText("1");

  int width = 64;
  int height = 64;
  int posX = getDevidedBy8Num(64);
  int posY = getDevidedBy8Num(78);

  screen->drawStringUtext(posX + 4, posY + 4, testText, true);

  displayDriver->display(screen->bufferBW, screen->bufferRY);

  displayDriver->displayInit(1, true); 
  for (int i = 2; i < 34; i++)  {
      
      screen->drawRect(posX, posY, width, height, false);
      screen->drawString(posX + 4, posY + 4, String(i), true);    
     // EPD_4IN2B_V2_Display_Partial(screen->bufferBW, posX, posY, posX + width, posY + height, true, true);
      displayDriver->displayPartial(screen->bufferBW, posX, posY, posX + width, posY + height);
    
  }

  delay(500);
  displayDriver->displaySleep();
  Serial.println("[drawTestPartial] EPD_4IN2B_V2_Display_BW end");   
}

// Display partial update benchmark

void Screen4in2UI::updateTestPartial(bool afterWakeup) {
  KellyCanvas * screen = env->getCanvas();

  localWidth = 400;
  localHeight = 300;

  screen->setBitsPerPixel(1);
  screen->clear();
  screen->setFont(&font44x44Config);
  screen->setRotate(0);
  screen->color = tBLACK;
 
  initPins(); 
  displayDriver->displayInit();  
  delay(100);

  Serial.println("[drawTestPartial] draw CAT");  
  int catPosX = 280;
  int catPosY = localHeight - 118;
  screen->drawImage(catPosX, catPosY, &cat_watch_85x120bw_settings, true);

   // uText testText = screen->getUText("Test");

   // screen->drawStringUtext(40, 80, testText, true);

    displayDriver->display(screen->bufferBW, screen->bufferRY);

    displayDriver->displayInit(1, true);  

    Serial.println("[drawTestPartial] EPD_4IN2B_V2_Display_BW end");  

    // int range = 225 - 160 + 1;

    int ballPosX = 125; // rand() % range + 160;
    int ballPosY = 220; // rand() % range + 160;
    int ballDeltaX = -1;
    int ballDeltaY = -1;

    ballPosX = getDevidedBy8Num(ballPosX);
    ballPosY = getDevidedBy8Num(ballPosY);

    int maxFrames = 40;

    for (int i = 0; i < maxFrames; i++)  {

          // redraw intersected
          screen->drawRect(ballPosX - 4, ballPosY - 4, cat_ball_40x40bw_settings.width + 10, cat_ball_40x40bw_settings.height + 10, false);
          int prevX = ballPosX;
          int prevY = ballPosY;
          ballPosX += ballDeltaX * 8;
          ballPosY += ballDeltaY * 8;

          displayDriver->displayPartial(NULL, min(prevX, ballPosX)  - 8, min(prevY, ballPosY)  - 8, prevX+48, prevY+48, false);

          // screen->drawRect(catPosX, catPosY, 85, 120, false);
          // screen->drawImage(catPosX, catPosY, &cat_watch_85x120bw_settings, true);
      
          if (ballPosX <= 0) {
            ballPosX = 8;
            ballDeltaX = 1;
          }

          if (ballPosX+cat_ball_40x40bw_settings.width >= localWidth) ballDeltaX = -1;
          
          if (ballPosY <= 0) {
            ballPosY = 8;
            ballDeltaY = 1;
          }

          if (ballPosY+cat_ball_40x40bw_settings.height >= localHeight) {
            ballDeltaY = -1;
          }

          screen->drawImage(ballPosX, ballPosY, &cat_ball_40x40bw_settings, true, true);

          displayDriver->displayPartial(screen->bufferBW, min(prevX, ballPosX), min(prevY, ballPosY), ballPosX+40, ballPosY+40);
          
    }

    displayDriver->displaySleep();
}

int Screen4in2UI::drawCat(bool land) {

  if (partial) return 0;

  clockFormatted dt = env->getFormattedTime();
  KellyCanvas * screen = env->getCanvas();
  screen->color = tBLACK;
  
  int hpad = 0;
  int marginX = 0;

  bool catShown = false;

  // currently simple logic - may be be more complex later, by pressure table
  
  // cat at home after 10pm - no check cold, show sleepy cat & moon icon
  Serial.println("[TEST]: " + String(dt.h));  
    
  if (dt.h >= 0 && dt.h <=  7) {

    hpad = 72 + 8;

    marginX = land ? calcMarginMiddle(localWidth/2, 110) +10 : localWidth - 110;

    screen->drawImage(marginX, localHeight - hpad, &cat_night_96x72bw_settings, true);
    hpad -= 10;
    catShown = true;
    hpad = drawWeaterIcon(screen, false, true, hpad, land);

  } else {

      // if day and cold outside - show winter cat with snow icon

      if (env->lastState.extData.isDataValid) {

          if (env->lastState.extData.temperature < 5) {

              hpad = 97 + 2;
              marginX = land ? calcMarginMiddle(localWidth/2, 105) : localWidth - 105;

              screen->drawImage(marginX, localHeight - hpad, &cat_winter_93x97bw_settings, true);
              hpad -= 20;
              catShown = true;
              hpad = drawWeaterIcon(screen, true, false, hpad, land);
          }

      } 
  }

  // if warm and day - default - show happy cat with sun, moon depend on hours

  if (!catShown) {
      hpad = 94;
      marginX = land ? calcMarginMiddle(localWidth/2, 105) : localWidth - 105;

      screen->drawImage(marginX, localHeight - hpad, &cat_happy_80x94bw_settings, true);
      hpad -= 20;
      hpad = drawWeaterIcon(screen, false, (dt.h > 20 && dt.h <= 23) || (dt.h >= 0 && dt.h <= 7) ? true : false, hpad, land);
  } 

  return hpad; 
}

int Screen4in2UI::calcMarginMiddle(int spaceWidth, int objectWidth) {
   return ceil((float) ((spaceWidth - objectWidth) / 2));
}

int Screen4in2UI::drawWeaterIcon(KellyCanvas * screen, bool cold, bool night, int hpad, bool land) {

    int marginX = land ? calcMarginMiddle(localWidth/2, 125) : localWidth - 125;
    if (cold) {
      hpad += 56; 
      screen->drawImage(marginX - 2, localHeight - hpad, &snowflakes_116x39bw_settings, true);
    } else if (night) {
      hpad += 80; 
      screen->drawImage(marginX, localHeight - hpad, &moon_clear_93x63bw_settings, true);
    } else {
      hpad += 90;
      marginX = land ? calcMarginMiddle(localWidth/2, 100) + 40: localWidth - 100;
      screen->drawImage(marginX, land ? localHeight - hpad + 30 : localHeight - hpad, &sun_clear_68x66bw_settings, true);
    }

    return hpad;
}

void Screen4in2UI::initPins() {
  if (!initPinsState) {
      displayDriver->initPins();
      delay(300); 
      initPinsState = true;
  }
}

void Screen4in2UI::clearScreen() {

  if (displayBeasy) return;
  displayBeasy = true;

  initPins();
  displayDriver->displayInit();
  
  delay(400);
  
  Serial.println(F("Clear..."));
  displayDriver->displayClear();

  Serial.println(F("Goto Sleep..."));
  displayDriver->displaySleep();

  displayBeasy = false;
}

void Screen4in2UI::updateScreen() {

  if (displayBeasy) return;
  displayBeasy = true;
  initPins(); 
  
  KellyCanvas * screen = env->getCanvas();
  if (screen->bitPerPixel == 2) Serial.print(F("2-bit [4 Color mode]"));
  else Serial.print(F("1-bit [2 Color mode]"));

  displayDriver->displayInit(screen->bitPerPixel);

  delay(400);
  Serial.println(F("Show image for array..."));

  // test inversion
  // screen->color = tBLACK;
  // screen->inverse();

  displayDriver->display(screen->bufferBW, screen->bufferRY);

  //printf("Clear...\r\n");
  //EPD_4IN2B_V2_Clear();
  Serial.println(F("Sleep..."));
  displayDriver->displaySleep();

  displayBeasy = false;
}

#endif