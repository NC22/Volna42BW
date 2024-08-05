/*
   @name           KellyCanvas
   @description    Library for draw frame buffer
   @author         Rubchuk Vladimir <torrenttvi@gmail.com> aka nradiowave
   @license        GPLv3
   @version        v0.7 
	
	Part of Volna 42 project  |  https://42volna.ru/

	Prepare compatible with KellyEInk 1-bit, 2-bit pixel buffer. 
    Include basic draw functions for output text strings, draw shapes and output images

    Instruments for prepare structures : 

    https://42volna.ru/tools/fontconverter - generate compatible fonts
    https://42volna.ru/tools/glypheditor   - convert images to compatible bin format

    Tools Github - https://github.com/NC22/Volna42BW-Tools
*/

#ifndef KellyCanvas_h
#define KellyCanvas_h

#include <Arduino.h>
#include <vector>
#include <KellyCanvasTypes.h>

class KellyCanvas {
    private :

        unsigned char getBit(unsigned char byte, int position);
        unsigned char setBit(unsigned char byte, int position, bool state);

        uCharCode uniord(String c, int offset);

        void moveBufferCursor(int x, int y);

        int bufferCursorByte;
        int bufferCursorBit;
        imageDrawModificators defaultMods;
        
    public:
        
        KellyCanvas(int iwidth = 400, int iheight = 300);
        
        const fontManifest * font = NULL;

        int width;
        int height;
        int getWidth();
        int getHeight();
        
        void inverse();

        // todo - попробовать выделять память через malloc - стандартный способ создает фрагментацию и креши при 20кб+
        unsigned char *bufferBW = NULL;
        unsigned char *bufferRY = NULL;
        int bufferWidth = 0;

        drawFrame drawFrameEt;
        
        tColor color = tBLACK; // in separate mode - tBLACK | tRY

        bool secondBuffer = false;

        // unfinished for multicolor displays (4-7 colors, 2-3 bit per per pixel), better to make another class for this type
        bool colorMerged = false; 

        int rotation = false;
        unsigned int bitPerPixel = 1;
        int scale = 1;

        void setRotate(int rotation);
        void setFont(const fontManifest * fontSettings);

        void clear();
        void applyXYMods(int & x, int & y);
  
        void drawPixel(int x, int y, bool state);
        void drawGlyph(int screenX, int screenY, int charIndex, bool frontColor = false);
        void drawGlyphByChar(int screenX, int screenY, char c, bool frontColor = false);
        void drawImage(int screenX, int screenY, imageData * image, bool frontColor = true, bool invert = false);
        void drawImageMods(int screenX, int screenY, imageData * image, imageDrawModificators & td, bool frontColor = true, bool invert = false);
        void resetImageMods(imageDrawModificators & td, bool enabled = false, bool flipByX = false, bool flipByY = false, int fromX = -1, int toX = -1, int fromY = -1, int toY = -1);
        void drawRoundedSquare(int ox, int oy, int width, int height, int cornerRadius, bool frontColor = false, int borderWidth = 0);

        bool getPixel(int x, int y, int bufferN = 0);        

       // void removeBuffer();
        void setBitsPerPixel(unsigned int bits);
        void initBuffer(uint8_t n);
        void initCanvas(int iwidth = 400, int iheight = 300, bool isecondBuffer = true, int ibitPerPixel = 1);

        int drawString(int screenX, int screenY, String text, bool frontColor = false);
        int drawStringUtext(int screenX, int screenY, uText & utext, bool frontColor = false, std::vector<int> * xPositions = NULL);
        void drawCircle(int sX, int sY, int radius, bool colorFront);
        void drawRect(int sX, int sY, int w, int h, bool colorFront);

        void transitionScreen(int x = 1, int y = 1, int width = -1, int height = -1);        

        void setDrawFrame(int fromX = -1, int width = -1, int fromY = -1, int height = -1);

        void clearDisplay(int addr);
        
        int getGlyphIndex(int s);
        int getStringWidth(uText & text);
        // std::vector<int> getStringCharsPoses(int baseX, uText & text);
            
        uText getUText(String text);
        
        bool overwriteMode = false;
};

#endif	



