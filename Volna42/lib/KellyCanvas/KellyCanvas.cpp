#include "KellyCanvas.h"

// Since on ESP8266 you cant reallocate big amount of memory dinamicly (30kb)
// Before draw or access buffer, you must initialize buffer by - canvas.setBitsPerPixel(bitMode); to init memory buffer once
// in Volna E-Ink, buffer initialized in Screen[].cpp entities by drawUIToBuffer method on first updateScreen call 

KellyCanvas::KellyCanvas(int iwidth, int iheight) {

    defaultMods.enabled = false;
    width = iwidth;
    height = iheight;
    // secondBuffer = isecondBuffer;
    // todo remove isecondbuffer var
    
    bitPerPixel = 1;

    // drawFrameEt.enabled = false;
    colorMerged = bitPerPixel > 1 ? true : false;
    // setBitsPerPixel(bitPerPixel);

    //  initBuffer(1);
    // if (secondBuffer) initBuffer(2);

    //
}

void KellyCanvas::setFont(const fontManifest * fontSettings) {
    font = fontSettings;
}

void KellyCanvas::setBitsPerPixel(unsigned int bits, bool skipRealloc) {

    bitPerPixel = bits;
    int newWidth = (int) ceil(((float) (width * height) / 8) * bitPerPixel);

    if (!skipRealloc) {
        
        if (newWidth != bufferWidth) {
            bufferWidth = newWidth;
            //removeBuffer();
            initBuffer(1);
            // clear();

            Serial.println(F("[Screen buffer reallocated] : BitsPerPixel : ")); Serial.print(bitPerPixel);
            Serial.print(F(" | Size : ")); Serial.println(bufferWidth);

        } else {

            Serial.println(F("[Screen buffer reinit] : Buffer already allocated : "));
        }

    } else {

        bufferWidth = newWidth;        
        Serial.print(F("[Screen buffer] [Unsafe Skip memory realloc] : BitsPerPixel : ")); Serial.println(bitPerPixel);
    }
}

/*
void KellyCanvas::removeBuffer() {

    Serial.println("Free memory");
    if (bufferBW){
       delete[] bufferBW; 
       bufferBW = nullptr;
    }  

    if (bufferRY){
       delete[] bufferRY;
       bufferRY = nullptr;
    }          
}
*/

void KellyCanvas::initBuffer(uint8_t n) {

    // нельзя переинициализировать буфер
    // переопределение памяти плохая идея т.к. при выделении больших блоков есть ошибки из за фрагментации рам в процессе работы

    if (n == 1) {
        if (!bufferBW){
            Serial.println(F("Init [MAIN] buffer"));
            // bufferBW = new unsigned char[bufferWidth];
            bufferBW = (unsigned char*)malloc(bufferWidth * sizeof(unsigned char)); 
        } else {
            Serial.println(F("Realloc [MAIN] buffer"));
            free(bufferBW);
            bufferBW = (unsigned char*)malloc(bufferWidth * sizeof(unsigned char)); 
            // bufferBW = (unsigned char*)realloc(bufferBW, bufferWidth * sizeof(unsigned char));
        }
    } else {  

        secondBuffer = true;

        if (!bufferRY) {
            Serial.println(F("Init [SECOND] buffer"));
            // bufferRY = new unsigned char[bufferWidth];
            bufferRY = (unsigned char*)malloc(bufferWidth * sizeof(unsigned char)); 
        } else {
            Serial.println(F("Realloc [MAIN] buffer"));
            bufferRY = (unsigned char*)realloc(bufferRY, bufferWidth * sizeof(unsigned char));
        }
    }
}

int KellyCanvas::getGlyphIndex(int s) {

  // char buffer[128];
  // sprintf(buffer, "map length : %d  | char index : %d", font.size, s);
  // Serial.println(buffer);

    int i = 0;
    while (i < font->size)
    {
        if (font->map[i] == s) {
            break;
        }
        i++;
    }
 
    if (i < font->size)
    {
       return i;
    }

    return -1;
}

uText KellyCanvas::getUText(String text) {

    int length = text.length();
    char const *c = text.c_str();
    
    uText textU;
    // memcpy(buffer, tt.data(), tt.length());
    
    std::vector<int> codeList;

    for ( int i = 0; i < length; i++) {
          
          //unsigned char ct = c[i];
          uCharCode ct = uniord(c, i);
          if (ct.size > 1) {
                i += ct.size-1;
          }
         
          codeList.push_back(ct.code);
          //sprintf(buffer, "Draw char : %s Index : %d", symbol, (int) symbol);
    }

    textU.size = codeList.size();
    textU.charCodes = codeList;
    textU.text = text;
    textU.pixelWidth = getStringWidth(textU);

    return textU;
}


// https://stackoverflow.com/questions/10333098/utf-8-safe-equivalent-of-ord-or-charcodeat-in-php
// todo add protection from hack symbol code (check max lenght)  - (char * c, unsigned int offset, unsigned int maxLength)

uCharCode KellyCanvas::uniord(String c, int offset) {

    unsigned char h = (unsigned char) c[offset];

    uCharCode cn;
              cn.size = 1;

    if (h <= 0x7F) {
        
        cn.code = h;

    } else if (h < 0xC2) {

        cn.code = 0;

    } else if (h <= 0xDF) {
        
        cn.code = (h & 0x1F) << 6 | ((unsigned char)c[offset + 1] & 0x3F);
        cn.size = 2;

    } else if (h <= 0xEF) {

        cn.code = (h & 0x0F) << 12 | ((unsigned char)c[offset + 1] & 0x3F) << 6
                                 | ((unsigned char)c[offset + 2] & 0x3F);
        cn.size = 3;

    } else if (h <= 0xF4) {

        cn.code = (h & 0x0F) << 18 | ((unsigned char)c[offset + 1] & 0x3F) << 12
                                 | ((unsigned char)c[offset + 2] & 0x3F) << 6
                                 | ((unsigned char)c[offset + 3] & 0x3F);

        cn.size = 4;     

    } else {

        cn.size = 0;
    }

    return cn;
}

unsigned char KellyCanvas::getBit(unsigned char byte, int position) // position in range 0-7
{
    return (byte >> (7 - position)) & 0x1;
}

unsigned char KellyCanvas::setBit(unsigned char number, int position, bool state) {
    
    position = (7 - position);
    
    if (!state) {
        unsigned char mask = ~(1 << position);
        return number & mask;
    } else {
        return number | (1 << position) ;
    }
    
}

void KellyCanvas::drawCircle(int sX, int sY, int radius, bool colorFront) {

    for(int y=-radius; y<=radius; y++) {
        for(int x=-radius; x<=radius; x++) {
              if(x*x+y*y < radius*radius + radius) {
                    drawPixel(sX+x, sY+y, colorFront);
              }
            
        }
     
    }

}
    
void KellyCanvas::drawRect(int sX, int sY, int w, int h, bool colorFront) {

    for(int y = 0; y < h; y++) {                    
        for(int x = 0; x < w; x++) {   
            if (x + sX < width && y + sY < height) drawPixel(x + sX, y + sY, colorFront);
        }	
    }
}

void KellyCanvas::drawImage(int screenX, int screenY, imageData * image, bool frontColor, bool invert) {
    drawImageMods(screenX, screenY, image, defaultMods, frontColor, invert);
}

void KellyCanvas::drawImageMods(int screenX, int screenY, imageData * image, imageDrawModificators & td, bool frontColor, bool invert) {
      
    if (!bufferBW && !bufferRY) {
	    Serial.println(F("[KellyCanvas] Cant draw image. Canvas is not initialized"));
        return;
    }

    int byteCursor = 0; int bitCursor = 0;        
    int resultY = 0; int resultX = 0;
    bool tCheck = false;

    // todo - add drawing scaled by circle with radius \ rectangle if needed

    for (int imageY = 0, scaleY = 0; imageY < image->height; imageY++, scaleY += scale) {

        resultY = screenY + scaleY;

        for (int imageX = 0, scaleX = 0; imageX < image->width; imageX++, scaleX += scale) {

            tCheck = true;
            if (td.enabled) {
                if (
                    (td.fromY >= 0 && imageY < td.fromY) || 
                    (td.toY >= 0 && imageY > td.toY) ||
                    (td.fromX >= 0 && imageX < td.fromX) ||
                    (td.toX >= 0 && imageX > td.toX)
                ) {
                    tCheck = false;   
                }
            }

            if (tCheck) {                

                resultX = screenX + scaleX;

                if (td.enabled && td.flipByX) {
                    resultX = screenX + (image->width - scaleX);
                } 

                if (td.enabled && td.flipByY) {
                    resultY = screenY + (image->height - scaleY);
                }

                bool draw = getBit(pgm_read_byte(&image->data[byteCursor]), bitCursor) > 0 ? false : true;
                if (invert) draw = !draw;

                if (draw) {
                    drawPixel(resultX, resultY, frontColor ? true : false);  
                    //Serial.println(String(resultX) + " || " + String(resultY));           
                }

            }

            bitCursor++;
            
            if (bitCursor > 7) {
                byteCursor++;
                bitCursor = 0;                            
            }                   
        }

         if (byteCursor >= image->size) {
            Serial.println(F("Overbounds"));
            Serial.println(resultX);
            Serial.println(resultY);
            break;

        }          
    }
}

// utf-8 unsafe, for digits \\ english

void KellyCanvas::drawGlyphByChar(int screenX, int screenY, char c, bool fontColor) {
    
    int dbIndex = getGlyphIndex((int) c);
    if (dbIndex >= 0) {
        drawGlyph(screenX, screenY, dbIndex, fontColor);
    }
}

void KellyCanvas::resetImageMods(imageDrawModificators & td, bool enabled, bool flipByX, bool flipByY, int fromX, int toX, int fromY, int toY) {
  td.enabled = enabled;
  td.flipByX = flipByX;
  td.flipByY = flipByY;
  td.fromX = fromX;
  td.fromY = fromY;
  td.toX = toX;
  td.toY = toY;
}

// [0,0] . . . y+
// .
// .
// . x+

void KellyCanvas::drawGlyph(int screenX, int screenY, int dbIndex, bool fontColor) {
      
    int byteCursor = font->settings[dbIndex].pByte; 
    int bitCursor = font->settings[dbIndex].pBit;
        
    //char buffer[128];
    //sprintf(buffer, "[INDEX: %d] %d | %d %d", dbIndex, screenX, screenY, byteCursor);
    //Serial.println(buffer);

    screenY += (font->height - font->settings[dbIndex].aHeight) * scale;
    screenY += font->settings[dbIndex].offsetY * scale;
    int resultY;
    int resultX;

    for (int glyphY = 1, scaleY = 1; glyphY <= font->settings[dbIndex].aHeight; glyphY++, scaleY += scale) {

        resultY = screenY + scaleY - 1;

        for (int glyphX = 1, scaleX = 1; glyphX <= font->settings[dbIndex].aWidth; glyphX++, scaleX += scale) {
            
            resultX = screenX + scaleX - 1;
            bool draw = getBit(pgm_read_byte(&font->glyphs[byteCursor]), bitCursor) > 0 ? true : false;
            //sprintf(buffer, "lasssssssst %02X || %d", charByte, charByte);
            //Serial.println(buffer);

            if (draw) {
                drawPixel(resultX, resultY, fontColor ? true : false);            
            }

            bitCursor++;
            
            if (bitCursor > 7) {
                byteCursor++;
                bitCursor = 0;                            
            }                   
        }          
    }
}

int KellyCanvas::getStringWidth(uText & utext) {
    
    int pixelWidth = 0;

    for (unsigned int i = 0; i < utext.size; i++) {
 
          int dbIndex = getGlyphIndex(utext.charCodes[i]);
          if (dbIndex >= 0) {
                if (i > 0)  pixelWidth += font->settings[dbIndex].offsetXb;
                pixelWidth += font->width + font->settings[dbIndex].offsetXa;
          } else {

            Serial.println(F("Unknown symbol by charcode : "));
            Serial.println(utext.charCodes[i]);

          }


    }
    
    if (pixelWidth <= 0) pixelWidth = 0;

    return pixelWidth;
}

/* 
    drawString(int screenX, int screenY, String text, bool fontColor)

    Return end X point of drawn text after render 
*/  

int KellyCanvas::drawString(int screenX, int screenY, String text, bool fontColor) {
    
    if (!font) {      
	    Serial.println(F("[KellyCanvas] Cant print text. Setup font first"));
        return 0;
    }

    uText utext = getUText(text);

   // Serial.println("before drawStringUtext");
   // Serial.println(text);
   // Serial.println(utext.pixelWidth);
   // Serial.println(utext.size);
   // Serial.println("--------");

    return drawStringUtext(screenX, screenY, utext, fontColor);
}

/*
    Draw text to screen canvas and return end position from screenX
*/
int KellyCanvas::drawStringUtext(int screenX, int screenY, uText & utext, bool fontColor, std::vector<int> * xPositions) {
 
    if (utext.pixelWidth <= 0) return 0;

    int posX = screenX;
    // delete [] drawPosMap;
    // drawPosMap = new int[text.length()];
    
    // char buffer[255];
    // if (loopPaddingX > 0) {
    //    
    //    sprintf(buffer, "Draw string : %s Length : %d Pixels width : %d padX : %d", utext.text.c_str(), utext.size, utext.pixelWidth, loopPaddingX);
    //    Serial.println(buffer);
    // }


    for (unsigned int i = 0; i < utext.size; i++) {
          
          int dbIndex = getGlyphIndex(utext.charCodes[i]);
          // Serial.println(dbIndex); 
          // sprintf(buffer, "Draw char code : %d | DB index %d", textu.charCodes[i], dbIndex);
          // Serial.println(buffer);
          if (dbIndex < 0) {
            
            Serial.print(F("Unknown code : ")); Serial.println(utext.charCodes[i]); 
            
            if (xPositions) {
                xPositions->push_back(-1);
            }

            continue;
          }

          if (i > 0) {
            posX += font->settings[dbIndex].offsetXb;
          }

          // drawPosMap[i] = posX;
          // Serial.println("drawGlyph utext"); 
          // Serial.println(utext.charCodes[i]); 

          drawGlyph(posX, screenY, dbIndex, fontColor);
          if (xPositions) {
            xPositions->push_back(posX);
          }

          posX += font->width + font->settings[dbIndex].offsetXa;
          // if (posX > getWidth()) break; - todo - detect rotated pixel
    }

    return posX;        
}

int KellyCanvas::getWidth() {
    
    return width;
}

int KellyCanvas::getHeight() {
  
    return height;
}

/*
void KellyCanvas::setDrawFrame(int fromX, int width, int fromY, int height) {

    if (fromX < 0) {

        drawFrameEt.enabled = false;

    } else {

        drawFrameEt.enabled = true;
        drawFrameEt.fromX = fromX;
        drawFrameEt.width = width;
        drawFrameEt.fromY = fromY;
        drawFrameEt.height = height;

    }
}
*/

void KellyCanvas::setRotate(int nrotation) {

    if (nrotation == 270) {
        rotation = 270;
    } else if (!nrotation) {
        rotation = 0;
    } else if (nrotation == 180) {
        rotation = 180;
    } else if (nrotation == 90) {
        rotation = 90;        
    }

    if (rotation) {
        Serial.println(F("Screen : [NOT rotated]"));
    } else {
        Serial.print(F("Screen : [rotated] : Deg : ")); Serial.println(rotation); 
    }
}

// inverse current buffer (depends on current color)

void KellyCanvas::inverse() {
    
    if (color == tBLACK) {
        for (int i = 0; i < bufferWidth; i++) {
            bufferBW[i] = ~bufferBW[i];
        }
    } else if (!colorMerged && secondBuffer) {
        for (int i = 0; i < bufferWidth; i++) {
            bufferRY[i] = ~bufferRY[i];
        }
    }
}

void KellyCanvas::applyXYMods(int & x, int & y) {
    
    if (rotation > 0) {
        int tmp = 0;
        if (rotation == 90) {
            tmp = x;
            x = getWidth() - y;
            y = tmp;
        } else if (rotation == 180) {
            x = getWidth() - x;
            y = getHeight() - y;
        } else if (rotation == 270) {
            tmp = y;
            y = getHeight() - x;
            x = tmp;
        }
    }  

    /*
    if (x < 0 || y < 0) return;
    if (x >= getWidth() || y >= getHeight()) return;

    if (drawFrameEt.enabled) {
        if ( x < drawFrameEt.fromX || x > drawFrameEt.fromX + drawFrameEt.width - 1) {
            x = -1;
            return;
        } else if (y < drawFrameEt.fromY || y > drawFrameEt.fromY + drawFrameEt.height - 1) {
            y = -1;
            return;
        }
    }
    */
}

// from 1 to screenWidth \ screenHeight
void KellyCanvas::drawPixel(int x, int y, bool state) {
    
    //char buffer[128];
    //sprintf(buffer, "Draw pixel at : %d %d | displays : %d ", x, y, displaysBottom);
    //Serial.println(buffer);

    applyXYMods(x, y);
    if (x < 0 || y < 0) return;
    if (x >= getWidth() || y >= getHeight()) return;

    moveBufferCursor(x, y);    

    // separate buffer mode
    if (bufferCursorByte > bufferWidth-1 || bufferCursorByte < 0) {
        
        // Serial.print(F("Out of bounds : ")); Serial.print(x); Serial.print(F(" | ")); Serial.print(y); Serial.print(F(" | ")); Serial.print(width); Serial.print(F("x")); Serial.println(height); 
        return;
    }

    if (!state) { // clear || white

        if (bitPerPixel == 1) {

            if (secondBuffer) {
                bufferRY[bufferCursorByte] = setBit(bufferRY[bufferCursorByte], bufferCursorBit, 1); 
            }

            bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit, 1); 

        } else {

            bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit, 1);
            bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit+1, 1);
        }

    } else {

        if (bitPerPixel == 1) {

            if (secondBuffer && color == tRY) { // second color buffer array
                bufferRY[bufferCursorByte] = setBit(bufferRY[bufferCursorByte], bufferCursorBit, 0);
            } else { // black color
                bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit, 0);
            }

        } else {

            uint8_t bits[2] = {0, 0}; // black

            if (color == tGRAY1 || color == tRY) {
                bits[0] = 1; bits[1] = 0;
            } else if (color == tGRAY2) {
                bits[0] = 0; bits[1] = 1;
            }

            bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit, bits[0]);
            bufferBW[bufferCursorByte] = setBit(bufferBW[bufferCursorByte], bufferCursorBit+1, bits[1]);
        }
    }
}

void KellyCanvas::moveBufferCursor(int x, int y) {

    int bitPos = (y * width) + x;  
        bitPos = bitPerPixel * bitPos;
            
    if (bitPos % 8) {
        
        bufferCursorByte = (int) floor((float) bitPos / 8);
        bufferCursorBit = bitPos - (bufferCursorByte * 8);

    } else {

        bufferCursorBit = 0;
        bufferCursorByte = bitPos / 8;
    }   
}

bool KellyCanvas::getPixel(int x, int y, int bufferN) {    

    moveBufferCursor(x, y); 
    if (!secondBuffer) bufferN = 0;

    if (bitPerPixel == 1) {

        return !getBit(bufferN == 0 ? bufferBW[bufferCursorByte] : bufferRY[bufferCursorByte], bufferCursorBit);
        
    } else {
        
        if (getBit(bufferBW[bufferCursorByte], bufferCursorBit) || getBit(bufferBW[bufferCursorByte], bufferCursorBit+1)) {
            return true;
        } else return false;
    }

}

void KellyCanvas::drawRoundedSquare(int x, int y, int width, int height, int cornerRadius, bool frontColor, int borderWidth) {
                            
    drawRect(x + cornerRadius, y, width - cornerRadius * 2, height, frontColor);
    drawRect(x, y + cornerRadius, width, height - cornerRadius * 2, frontColor);
    drawCircle(x + cornerRadius, y + cornerRadius, cornerRadius, frontColor);
    drawCircle(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, frontColor);
    drawCircle(x + cornerRadius, y + height - cornerRadius - 1, cornerRadius, frontColor);
    drawCircle(x + width - cornerRadius - 1, y + height - cornerRadius - 1, cornerRadius, frontColor);
    
    if (borderWidth > 0) {
        if (borderWidth > (width / 2)) borderWidth = width / 2;
        
        drawRoundedSquare(x + borderWidth, y + borderWidth, width - (borderWidth * 2), height - (borderWidth * 2), !frontColor, 0);
    }
}

void KellyCanvas::clear() {

      Serial.println("clear BW");
    for (int i = 0; i < bufferWidth; i++) {
        bufferBW[i] = 0XFF;
    }

    if (!colorMerged && secondBuffer) {

      Serial.println("clear RY");
        for (int i = 0; i < bufferWidth; i++) {
            bufferRY[i] = 0XFF;
        }

    }
}