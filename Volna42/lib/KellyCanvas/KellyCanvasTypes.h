#ifndef KellyCanvasTypes_h
#define KellyCanvasTypes_h

#include <Arduino.h>

typedef struct {
  unsigned int size;
  std::vector<int> charCodes;
  String text;
  int pixelWidth;
} uText;

typedef struct {
  int size;
  int code;
} uCharCode;

/*
typedef struct {
  int fromX;
  int width;
  int height;
  int fromY;
  int enabled;
} drawFrame;
*/

typedef struct {
  int offsetXb;
  int offsetXa;
  int offsetY;
  int aWidth;
  int aHeight;
  int pByte;
  int pBit;
} glyphSettings;

typedef struct {
  uint8_t * glyphs;
  int * map;
  int width;
  int height;
  int size;
  glyphSettings * settings;
} fontManifest;

typedef struct {
      uint8_t * data;
      int size;
      int width;
      int height;
} imageData;

typedef struct {
      bool enabled;
      bool flipByX;
      bool flipByY;
      int fromX;
      int toX;
      int fromY; 
      int toY;
} imageDrawModificators;

enum tColor { tBLACK, tWHITE, tRY, tGRAY1, tGRAY2 };

#endif	



