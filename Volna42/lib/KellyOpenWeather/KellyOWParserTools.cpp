#include "KellyOWParserTools.h"

String KellyOWParserTools::sanitizeResponse(String var) {
  
  int responseLength = var.length();
  var = var.substring(0, 128);
  String sanitized = "";
  bool valid = false;

  unsigned char c;

  for (int i = 0; i < responseLength; i++)  {

    c = var[i];
    
    if ((unsigned int) c == 34 || (unsigned int) c == 58 || (unsigned int) c == 47) {
      
      sanitized += "-";
      continue;

    } else {

      valid = false;
      
           if ((unsigned int) c == 32) valid = true; // space
      else if ((unsigned int) c >= 45 || (unsigned int) c <= 57) valid = true; // 0123456789.- 
      else if ((unsigned int) c >= 97 || (unsigned int) c <= 122) valid = true; // a-z
      else if ((unsigned int) c >= 65 || (unsigned int) c <= 90) valid = true;  // A-Z

      if (!valid) continue;
    }


    sanitized += (char) c;
  }

  return sanitized;
}

int KellyOWParserTools::validateIntVal(String v) {

    if (v.length() <= 0) return -1000;

    int nv = -1000;
    if(sscanf(v.c_str(), "%d", &nv) != 1) {
        return -1000; 
    }

    return nv;
}

float KellyOWParserTools::validateFloatVal(String v) {

    if (v.length() <= 0) return -1000;

    float nv = -1000;
    if(sscanf(v.c_str(), "%f", &nv) != 1) {
        return -1000; 
    }

    return nv;
}

bool KellyOWParserTools::collectJSONFieldDataRaw(int paramStart, int len, String & data, String & storage, unsigned int maxLength) {

      // paramStart cursor must be at delimiter - ":" at start
      // param:"...."
      paramStart++;
      KellyOWStringType type = kJSONMix;
      int untilCloseTag = 1;
      char closeChar = ' ';
      char openChar = ' ';
      
      // search start
      for (int i = paramStart; i < len; i++) {

        // skip whitespace
        if (data[i] == ' ' || data[i] == '\t' || data[i] == '\n' || data[i] == '\r') { 
            continue;
        // string param - start after quote
        } else if (data[i] == '"') {
            type = kJSONString;
            paramStart = i+1;
            break;  
        } else if (data[i] == '[') {
            type = kJSONArray;
            paramStart = i+1;
            closeChar = ']';
            openChar = '[';
            break;  
        } else if (data[i] == '{') {
            type = kJSONObject;
            paramStart = i+1;
            closeChar = '}';
            openChar = '{';
            break;  
        // start here
        } else {
            paramStart = i;
            break;            
        }
      }

      unsigned char c;
      for (int i = paramStart; i < len; i++) {

        c = data[i];
        // Serial.print(c);

          if (type == kJSONString) {

            if (c == '"') return true;
            if (storage.length() > maxLength) {
              return true;
            }

          } else if (type == kJSONObject || type == kJSONArray) {
            
            if (c == openChar) {
                untilCloseTag++;
            } else if (c == closeChar) {
                untilCloseTag--;
                if (untilCloseTag <= 0) return true;
            }

            if (storage.length() > maxLength) {
              return true;
            }

          } else {

            if (c == ' ') continue;
            if ((unsigned int) c < 45 || (unsigned int) c > 57) return true; // allowed chars 0123456789.-
            
            if (storage.length() > 16) {
              return true;
            }
          }

          storage += (char) c;
    }

    return true; 
}

bool KellyOWParserTools::collectJSONFieldData(String fieldName, String & payload, String & storage, unsigned int maxLength) {
    storage = "";
    int len = payload.length();
    int dataPos = payload.indexOf("\"" + fieldName + "\"");

    if (dataPos != -1) {
        dataPos += fieldName.length();
        for (int i = dataPos; i < dataPos + 255; i++) {
            if (i < len-1 && payload[i] == ':') {  
                return collectJSONFieldDataRaw(i, len, payload, storage, maxLength);
            }
        }
    }

    return false;
}
