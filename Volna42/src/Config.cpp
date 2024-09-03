#include "Config.h"

// v 1.03 22.06.24

Config::Config() {
    
    cfgSize = cfgOptionsSize;
    cfgValues = new String[cfgOptionsSize];

    limits[ctypeFloat] = 32; 
    limits[ctypeBool] = 1; 
    limits[ctypeString] = 255; 
    limits[ctypeInt] = 32;

    reset();    
}

void Config::test() {
    
    Serial.println(F("-------test output all--------")); 
    for(size_t i=0; i < cfgOptionsSize; i++) {

        Serial.println(getKeyName(cfgOptions[i].key) + " | " + getString(cfgOptions[i].key).c_str());
    } 

    Serial.println(getOptionsJSON());
}

cfgOptionKeys Config::getOptionKey(const char * name) {

    for(size_t i = 0; i < cfgSize; i++) {

        if (strcmp(cfgOptions[i].keyStr, name) == 0) {
            Serial.println(cfgOptions[i].keyStr);
            return cfgOptions[i].key;
        }
    }

    return cNull;
}

String Config::getKeyName(cfgOptionKeys key) {
    
    for(size_t i = 0; i < cfgSize; i++) {

        if (cfgOptions[i].key == key) {
            return cfgOptions[i].keyStr;
        }
    }

    return "";
}

void Config::reset() {
    for(size_t i = 0; i < cfgSize; i++) {
        cfgValues[i] = "";
    }
}

bool Config::isReloadRequired(cfgOptionKeys key) {

    int index = getElementIndexByKey(key);
    if (index == -1) {
        return false;
    }

    return cfgOptions[index].rebootRequired;
}

bool Config::set(cfgOptionKeys key, String value) {

    int index = getElementIndexByKey(key);
    if (index == -1) {
        return false;
    }

    // char buffedr[64];   
    // sprintf(buffedr, "before : %s", value.c_str());
    // std::cout << buffedr << std::endl;
    
    if (cfgOptions[index].secured) {
        if (value.length() > 0 && value[0] == protectChar) {
                return false;
        }
    }

    value.trim();
    
    if (value.length() > limits[cfgOptions[index].type]) {
        value = value.substring(0, limits[cfgOptions[index].type]);
    }

    // sprintf(buffedr, "after  : %s", value.c_str());
    // std::cout << buffedr << std::endl;

    if (cfgOptions[index].type == ctypeBool) {

        // Serial.println("input " + value);
        cfgValues[index] = sanitizeBool(value) ? "1" : "0";

        // Serial.println("result " + cfgValues[index]);

    } else if (cfgOptions[index].type == ctypeFloat) {

        std::replace(value.begin(), value.end(), ',', '.');
        if (value.indexOf(".") == -1) {
            value += ".0";
        }

        float fState = sanitizeFloat(value);
        if (sanitizeError) return false;

        char buffer[32];
        sprintf(buffer, "%.2f", fState);

        cfgValues[index] = buffer;

    } else if (cfgOptions[index].type == ctypeInt) {

        int newValue = sanitizeInt(value);
        if (sanitizeError) return false;
        
        cfgValues[index] = String(newValue);
        
    } else if (cfgOptions[index].type == ctypeString) {

        cfgValues[index] = value;

    } else {

        return false;
    }

    return true;
}   

String Config::getString(cfgOptionKeys key) {

    String state = "";
    int index = getElementIndexByKey(key);
    if (index == -1) return state;

    return cfgValues[index];  
}

void Config::getStringList(String key, std::vector<String> & stringList, char delimiter, unsigned int limit) {

    String current = "";
    int length = key.length();
    
    for ( int i = 0; i < length; i++) {
          
          if (key[i] == delimiter) {
            
            current.trim();
            stringList.push_back(current);
            if (stringList.size() >= limit) return;

            current = "";
            
          } else {
          
            current += key[i];
          }
    }
    
    if (current.length() > 0) stringList.push_back(current);
}
    
float Config::sanitizeFloat(String &value) {

    sanitizeError = false;
    float fState = defaultFailFloat;
    if(sscanf(value.c_str(), "%f", &fState) != 1) {
        sanitizeError = true;
        return fState;    
    }

    return fState;
}

float Config::getFloat(cfgOptionKeys key) {

    int index = getElementIndexByKey(key);
    if (index == -1) return defaultFailFloat;

    return sanitizeFloat(cfgValues[index]);
}

/* Allowed only lat letters and digits */
String Config::sanitizeFileName(String &value) {
    
    sanitizeError = false;
    int len = value.length();
    String result = "";
    bool validSymb = false;

    for (int i = 0; i < len; i++) {
        
        validSymb = false;
        unsigned char chr = (unsigned char) value[i];
        if (chr >= 48 && chr <= 57) { // 0-9
            validSymb = true; 
        } else if (chr >= 97 && chr <= 128) { // a-z
            validSymb = true;
        } else if (chr >= 65 && chr <= 90) { // A-Z
            validSymb = true;
        } 
        
        if (validSymb) result += value[i];
    }

    return result;
 }

int Config::sanitizeInt(String &value) {

    sanitizeError = false;
    int intState = defaultFailInt;
    if(sscanf(value.c_str(), "%d", &intState) != 1) {
        sanitizeError = true;
        return intState;    
    }

    return intState;
}

int Config::getInt(cfgOptionKeys key) {

    int index = getElementIndexByKey(key);
    if (index == -1) return defaultFailInt;

    return sanitizeInt(cfgValues[index]);
}

bool Config::sanitizeBool(String &value) {

    sanitizeError = false;
    int intState;
    if(sscanf(value.c_str(), "%d", &intState) != 1) {
        sanitizeError = true;
        return defaultFailBool;    
    }

    return intState > 0 ? true : false;
}

bool Config::getBool(cfgOptionKeys key) {

    int index = getElementIndexByKey(key);
    if (index == -1) return defaultFailBool;

    return sanitizeBool(cfgValues[index]);
}

int Config::getElementIndexByKey(cfgOptionKeys& key) {
    
    for(size_t i = 0; i < cfgSize; i++) {
        if (cfgOptions[i].key == key) {
            return i;
        }
    }

    return -1;    
}

bool Config::restoreFromBinary(unsigned char * cfg, unsigned int size) {

    size_t index = 0;
    cfgValues[0] = "";

    for(unsigned int i=0; i < size; i++) {

        if (cfg[i] == cfgDelimiterChar) {
            
            index++;
            if (index > cfgSize-1) {
                Serial.println(F("[Warning] stored memory items is bigger then current cfg. [Option value ignored]"));
                break;
            }

            cfgValues[index] = "";            

        } else {
            
            cfgValues[index] += (char) cfg[i];
        }        
    }

    Serial.print(F("max index : ")); Serial.print(index); Serial.print(F(" string lenght : ")); Serial.println(size);

    /*
    Serial.println(F("----------Restore data from config---------------"));

    for(size_t i=0; i < cfgSize; i++) {
        Serial.print(cfgOptions[i].keyStr); Serial.print(F(" --- ")); 
        if (cfgOptions[i].secured) {
            Serial.print(F("[HIDDEN] Length : ")); Serial.println(String(cfgValues[i].length()));  
        } else {
            Serial.println(cfgValues[i]);   
        }  
    }   

    Serial.println(F("----------Restore data from config--END---------------"));
    */
    return true;
}

String Config::getStringQuoted(String val) {
    
    String nstring = "";
    int length = val.length();
    for (int i = 0; i < length; i++) {
        if (val[i] == '"') {
            nstring += "\\\"";
        } else {
            nstring += val[i];
        }
    }

    return "\"" + nstring + "\"";
}

String Config::hideValue(String value){
    
    String hidden = "";
    int length = value.length();

    for(int i = 0; i < length; i++) {
        hidden += protectChar;
    }

    return hidden;
}

// get web-safe json
// usually placed to ENV.cfgSaved during page load initialisation process

String Config::getOptionsJSON() {
    
    String json = "var ENVCFG = {";

    json += "\"__LOCALE\":\"";
    json += FPSTR(defaultLocale);
    json += "\",";

    json += "\"__PRODUCT_VERSION\":\"";
    json += FPSTR(productVersion);
    json += "\",";

    json += "\"__V\":\"";
    json += version; // used for mark web-resources revision, to prevent old cache shown
    json += "\",";

    json += "\"__EXCLUDE\":[";   
    for(size_t b = 0; b < excludeOptions.size(); b++) json += "\"" + getKeyName(excludeOptions[b]) + "\"";
    json += "],";

    for(size_t i = 0; i < cfgSize; i++) {
        json += "\"" + getKeyName(cfgOptions[i].key) + "\"" + ":";
        json += getStringQuoted(cfgOptions[i].secured ? hideValue(cfgValues[i]) : cfgValues[i]) + ",";
    }

    json += "};";

    return json;
}

unsigned int Config::loadEEPROM() {
    
    int cfgSize;
    int address = 0;
    unsigned int eepromVersion = 0;
    int metaSize = sizeof(cfgSize) + sizeof(eepromVersion); 
    int maxEEPROMAllowedSize = 4096;
    
    #if defined(ESP32)
    
        // не экономим, иначе end нормально не срабатывает и после метаданных чтение не идет

        if (!EEPROM.begin(metaSize + maxEEPROMAllowedSize)) {
            Serial.println(F("Failed to initialise EEPROM"));
            return 0;
        }

    #else 
        EEPROM.begin(metaSize);
    #endif

    EEPROM.get(address, cfgSize);
    address += sizeof(cfgSize);

    EEPROM.get(address, eepromVersion);
    address += sizeof(eepromVersion);

    if (eepromVersion >= 1003 && eepromVersion <= 20000 && cfgSize < maxEEPROMAllowedSize - metaSize) {

        #if defined(ESP32)
        #else 
            EEPROM.end();
            EEPROM.begin(metaSize + cfgSize);
        #endif
    
        Serial.print(F("EEPROM-LOADING : size : ")); Serial.print(cfgSize); Serial.print(F(" | version : ")); Serial.println(eepromVersion);

        unsigned char * configStr = new unsigned char[cfgSize];
        
        // Serial.println(F("------EEPROM-DAT-----"));
        for (int i = 0; i < cfgSize; i++) {
            configStr[i] = EEPROM.read(address);        
            // Serial.print(" ["); Serial.print((int) configStr[i]); Serial.print("]");
            address++;
        }

        // Serial.println();
        // Serial.println(F("------EEPROM-DAT-END-----"));

        restoreFromBinary(configStr, cfgSize);
        delete[] configStr;

        EEPROM.end();

    } else {

        EEPROM.end();
        
        if (eepromVersion > 0 && eepromVersion <= 20000) {
            Serial.print(F("VER-OK : ")); Serial.println(String(eepromVersion));
        } else {
            Serial.println(F("VER-NO-DATA"));
        }

        eepromVersion = 0;
        Serial.println(F("------EEPROM-EMPTY-----"));
    }

    return eepromVersion;
}

void Config::fillNopEEPROM(int eepromSize) {

    EEPROM.begin(eepromSize);

    if (eepromSize > 4096) eepromSize = 4096;

    int address = 0;
    for (int i = 0; i < eepromSize; i++) {
        
        EEPROM.write(address, 0xFF);
        address++;
    }

    EEPROM.commit();
    EEPROM.end();
}

bool Config::commitEEPROM(bool clear) {

     String config = commit();

    int cfgSize = (int) config.length();
    int eepromSize = sizeof(cfgSize) + sizeof(version) + cfgSize;
    int address = 0;

    if (clear) {
        Serial.println(F("Clear EEPROM Size : "));
        Serial.println(eepromSize);
        fillNopEEPROM(eepromSize); // todo - keep eeprom size after loadEEPROM for more accurate clear process
        return true;
    }

    if (eepromSize > 4096) {
        Serial.print(F("EEPROM write fail - more than EEPROM storage limit (4096 bytes max) - "));
        Serial.println(eepromSize);
        return false;
    }

    Serial.println(F("----- start -------"));
    Serial.print(F("Size : ")); Serial.println(eepromSize);
    
    EEPROM.begin(eepromSize);

    Serial.println(F("----- write meta -------"));

    EEPROM.put(address, cfgSize);    
    address += sizeof(cfgSize);
    
    EEPROM.put(address, version);    
    address += sizeof(version);

    Serial.println(F("----- write data -------"));

    for (int i = 0; i < cfgSize; i++) {
        unsigned char cChar = config[i];

        Serial.print(" ["); Serial.print((int) cChar); Serial.print("] at "); Serial.print(address);

        EEPROM.write(address, cChar);
        address++;
    }

    Serial.println();
    Serial.println(F("----- end -------"));

    EEPROM.commit();
    EEPROM.end();

    Serial.println(F("----- commit -------"));

    return true;
}

String Config::commit() {

    String writeCfg = "";

    for(size_t i=0; i < cfgSize; i++) {

        writeCfg += cfgValues[i];
        if (i != cfgSize - 1) {
            writeCfg += cfgDelimiterChar;
        }
    }
    
    Serial.println(writeCfg);
    return writeCfg;
}