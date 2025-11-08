#ifndef WebServerEink_h
#define WebServerEink_h

#include <WebServerBase.h>

#if defined(ESP32)
    #include <HTTPUpdateServer.h>
#else 
    #include <ESP8266HTTPUpdateServer.h>
#endif

extern const unsigned int webdataSize_locale_js PROGMEM;
extern const char webdata_locale_js[] PROGMEM;

// todo - сжать оформление веб версии в блок max-width 1256px

class WebServerEink:public WebServerBase {
    protected : 
             
        String getInfo() override;
        void router() override;

        int fileCursor = -1;
        unsigned int bufferTotalWrite = 0;
        bool cuiUploadMode = false;
        bool cuiUploadDisplayAfter = false;
        // vars restored after CUI uploads - if will be more needed to restore - better move to temporary structure
        bool cuiLandBack = false;
        bool cuiRotateBack = false;

        #if defined(ESP32)
            HTTPUpdateServer httpUpdater;
        #else 
            ESP8266HTTPUpdateServer httpUpdater;
        #endif
        
    private :

        void apiTestData();
        void apiGetBuffer();
        void apiClear(); // clear & stop auto update

        // void apiDrawTest(); 

        void apiCuiDownload();
        void apiCuiDelete(); 
        // void apiCuiGetInffo(); 
        void apiCuiFormat(); 
        void apiCuiList(); 
        void apiCuiSelect(); 

        void apiClockTest(); 
        void apiDirectImage(); 
        void apiDirectWidgets(); 
        void apiClockFontChange();

        void apiDefaultOk();
        void apiUpdate(); // insta update  
        void apiTestLowPower(); // test low power screen
        void apiPartialTest(); // test low power screen
        void apiSCDForceCalibration(); 
        void showUploadImagePage();
        void getUploaderToolsJs();
        void getUploaderJs();
        void getUploaderCss();
        void getUploaderFMJs();
        void getLanguageJs();
        
    public:
       
        WebServerEink(Env * env, int port = 80);
        void runConfigured() override;
};




#endif