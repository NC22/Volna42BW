#include "KellyOWParserTools.h"

/*
  Simple RAM friendly method for read headers from web-server
  Each header line can be max 255 chars long to limit RAM usage, headers limited with max 2kb
  maxIdleTime - timeout if server will answer longer then this limit reached
*/
void KellyOWParserTools::clientReadHeaders(uint16_t &code, uint16_t &contentLength, WiFiClient * client, unsigned int maxIdleTime) {

  Serial.println(F("[clientReadHeaders] Connected to server [read response] ... "));
  char line[256]; 
  unsigned int index = 0; 

  char c;
  uint8_t nlineCount = 0;

  contentLength = 0;
  code = 0;
  unsigned long secondTimerStart = millis();
  uint16_t maxSize = 2000;
  uint16_t size = 0;

  while (client->connected() || client->available()) {
      
      if (millis() - secondTimerStart >= maxIdleTime) {
        Serial.println(F("[clientReadHeaders] Native Timer fail. Server not responding : Stop by counter")); 
        return;
      }

      while (client->available()) {      
          c = client->read();

          size++;
          if (size > maxSize) {
            Serial.println(F("[clientReadHeaders] Max headers size reached")); 
            return;
          }

          if (c != '\r' && c != '\n') {
              nlineCount = 0;
              if (index < sizeof(line) - 1) {  
                  line[index++] = c;
              }
          }

          if (c == '\n') {
              line[index] = '\0'; // put on last cursor symbol as endpoint [h] [e] [a] ... [\r] \n

              if (contentLength <= 0 && strncmp(line, "Content-Length: ", 16) == 0) {
                  contentLength = atoi(line + 16);
              } else if (code <= 0 && strncmp(line, "HTTP/", 5) == 0) {
                  code = atoi(line + 9);
              }

              // Serial.println(line);

              nlineCount++;
              index = 0;  
              if (nlineCount >= 2) return;
          }
      }
  }

  if (size <= 0) {
      Serial.println(F("[clientReadHeaders] Unexpected end of line -> server close connection")); 
  }

}

/*
  Send simple GET request with basic headers without any addition data, similar to WGET
*/
void KellyOWParserTools::clientSendRequestHeaders(String &host, String &path, WiFiClient * client) {

  String tmp = "GET " + path + " HTTP/1.1\r\n" + 
               "Host: " + host + "\r\n" + 
              // "Accept: text/html\r\n" + 
              // "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:109.0) Gecko/20100101 Firefox/109.0\r\n" + 
                "Connection: close\r\n\r\n";

  client->print(tmp);
}
 
/*
  Read data from server with max limit by size
  size - max amount of bytes that can be readed from server
  maxIdleTime - timeout if server will answer longer then this limit reached
*/
void KellyOWParserTools::clientReadBody(String &out, uint16_t size, WiFiClient * client, unsigned int maxIdleTime) {

  char c;
  out = "";
  unsigned long secondTimerStart = millis();

  while (client->connected() || client->available()) {
      
    if (millis() - secondTimerStart >= maxIdleTime) {
      Serial.println(F("[clientReadHeaders] Server not responding : Return by Timeout"));
      return;
    }

    if (!client->available()) {
      // delay(100);
      continue;
    }
    
    c = client->read();
    out += c;

    if (out.length() > size) {
      return;
    }
  }
}

bool KellyOWParserTools::parseURL(String &url, String &host, int &port, String &path) {

  int protocolEnd = url.indexOf("://");
  if (protocolEnd == -1) {
    return false;
  }

  String proto = url.substring(0, protocolEnd);

  int hostStart = protocolEnd + 3;
  int pathStart = url.indexOf("/", hostStart);
  if (pathStart == -1) {
    pathStart = url.length();
  }
  host = url.substring(hostStart, pathStart);
  path = url.substring(pathStart);

  port = (proto == "https") ? 443 : 80;

  return true;
}

String KellyOWParserTools::sanitizeResponse(String var) {
  
  int responseLength = var.length();
  String sanitized = "";
  bool valid = false;
  unsigned char c;

  for (int i = 0; i < responseLength; i++)  {

    c = var[i];    
    if ((unsigned int) c == 34 || (unsigned int) c == 58 || (unsigned int) c == 47) {
      
      sanitized += "-";

    } else if ((unsigned int) c == 10 || (unsigned int) c == 13) {

      sanitized += "<br>";

    } else {

      valid = false;
      
           if ((unsigned int) c == 32) valid = true; // space
      else if ((unsigned int) c >= 45 && (unsigned int) c <= 57) valid = true; // 0123456789.- 
      else if ((unsigned int) c >= 97 && (unsigned int) c <= 122) valid = true; // a-z
      else if ((unsigned int) c >= 65 && (unsigned int) c <= 90) valid = true;  // A-Z

      if (valid) {
        sanitized += (char) c;
      }
    }

    if (sanitized.length() > 128) {
      sanitized += "...";
      break;
    }
  }

  return sanitized;
}

/*
void KellyOWParserTools::validateHum(float & hum) {
   if (hum <= BAD_SENSOR_DATA) {
      hum = 0;
    } else {
      if (hum > 100) {
          hum = 100;
      } else if (hum < 0) {
          hum = 0;
      }
    }
}
*/

int KellyOWParserTools::validateIntVal(String v) {

    if (v.length() <= 0) return BAD_SENSOR_DATA;

    int nv = BAD_SENSOR_DATA;
    if(sscanf(v.c_str(), "%d", &nv) != 1) {
        return BAD_SENSOR_DATA; 
    }

    return nv;
}

float KellyOWParserTools::validateFloatVal(String v) {

    if (v.length() <= 0) return BAD_SENSOR_DATA;

    float nv = BAD_SENSOR_DATA;
    if(sscanf(v.c_str(), "%f", &nv) != 1) {
        return BAD_SENSOR_DATA; 
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
