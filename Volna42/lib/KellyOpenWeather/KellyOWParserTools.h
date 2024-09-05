#ifndef KellyOWParserTools_H
#define KellyOWParserTools_H

#include <Arduino.h>

enum KellyOWStringType { kJSONString, kJSONMix, kJSONArray, kJSONObject };

class KellyOWParserTools {
public:
    static float validateFloatVal(String v);
    static int validateIntVal(String v);
    static bool collectJSONFieldDataRaw(int paramStart, int len, String &data, String &storage, unsigned int maxLength = 255);
    static bool collectJSONFieldData(String fieldName, String & payload, String & storage, unsigned int maxLength = 255);
};

#endif 