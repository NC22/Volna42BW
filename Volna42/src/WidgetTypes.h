#ifndef WidgetTypes_h
#define WidgetTypes_h

#include <Arduino.h>

enum uiWidgetType { 
	uiClock, 
	uiInfo, 
	uiTemp, 
	uiHum, 
	uiTempRemote, 
	uiHumRemote, 
	uiBatRemote, 
	uiBat, 
	uiInfoIP, 
	uiInfoVoltage, 
	uiLastSyncRemote, 
	uiInfoSyncNumRemote, 
	uiShortInfoSyncRemote, 
	uiInfoMessage, 
	uiPressure, 
	uiDate,
	uiPressureRemote,
	uiNone 
};

typedef struct {
    
    int16_t x;
    int16_t y;
    bool enabled;
    uiWidgetType type;
    String params;
    unsigned int id;

} uiWidgetStyle;

typedef struct {
    
    int xStart;
    int xEnd;
    int yStart;
    int yEnd;

} partialUpdateData;

typedef struct {
    
    unsigned int id;
    uiWidgetType type;

} uiWidgetDefaults;


#endif