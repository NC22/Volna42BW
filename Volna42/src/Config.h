#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <vector>
#include <EEPROM.h>
// using namespace std;

// const char cdNull[] PROGMEM = "";

#include <EnvConfigTypes.h>

// extern const char* const pgmCfgOptionsKeys[] PROGMEM;
extern const cfgOption cfgOptions[];
extern const size_t cfgOptionsSize PROGMEM;
extern const char defaultLocale[] PROGMEM;
extern const char productVersion[] PROGMEM;

class Config {
    
    private :
        
        // password type fields cant contain first space for security reasons (it used as placeholder, hides pass on client side)
        // toogle "secured" boolean value to false in EnvConfigOptions.h, to exclude this hide values logic
        char protectChar = (char) 0x20; // replaces password characters and used as placeholder
        char cfgDelimiterChar = (char) 0x01; // used to split variables of saved cfg raw string

        size_t cfgSize;
        
        int getElementIndexByKey(cfgOptionKeys& key); 
        String hideValue(String value);

        // todo - move to tools file
        
        String getStringQuoted(String val);

        float defaultFailFloat = -1.0f;
        bool defaultFailBool = false;
        int defaultFailInt = -1;
       
        bool restoreFromBinary(unsigned char * cfg, unsigned int size);

    public:

        Config();

        unsigned int limits[4];  
        String *cfgValues;
        bool sanitizeError = false;

        bool sanitizeBool(String &value);
        float sanitizeFloat(String &value);
        int sanitizeInt(String &value);

        String sanitizeFileName(String &value);

        String getKeyName(cfgOptionKeys key);
        cfgOptionKeys getOptionKey(const char * name); 

        bool isReloadRequired(cfgOptionKeys key);

        bool getBool(cfgOptionKeys key); 
        int getInt(cfgOptionKeys key);
        float getFloat(cfgOptionKeys key);        
        String getString(cfgOptionKeys key);

        void getStringList(String value, std::vector<String> & list, char delimiter = ',', unsigned int limit = 10);

        bool set(cfgOptionKeys key, String value); // creates new or update exist config item by its key
        void reset();
        String commit(); // creates new or update exist config item by its key

        unsigned int loadEEPROM();
        void fillNopEEPROM(int eepromSize);
        bool commitEEPROM(bool clear = false);
        
        String getOptionsJSON();
        std::vector<cfgOptionKeys> excludeOptions;

        void test();

        unsigned int version = 0; // default config version to compare with, initialized by Env from user config, valid version ids - loadEEPROM() 

        // void load(String input);
        // String getList(); // return helpfull options list in format - key:data
        // void stringify();
        // void commit(); // write data to EEPROM
};

#endif

