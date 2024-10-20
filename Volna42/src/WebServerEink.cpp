#include <WebServerEink.h>
// #include <testImage.h>

#include <FS.h>
#include <LittleFS.h>

WebServerEink::WebServerEink(Env * env, int port) : WebServerBase(env, port) {
   
        #if defined(ESP32)
            ramFriendlyMode = false;
        #else 
            ramFriendlyMode = true;
        #endif
}


void WebServerEink::runConfigured() {

    WebServerBase::runConfigured();
 
    /*
        Заливка кастомного оформления через /api/direct/image и /api/direct/widgets

        1. Буфер картинки будет принят только если изначально активен режим кастомных оформлений через /api/direct/widgets
        где можно указать расположение виджетов поверх картинки и название оформления

        2. Принимаем части буфера картинки. Разбивка может быть на несколько промежуточных файлов 
        размер промежуточного файла = общий размер / bitsPerPixel
        они будут дописаны в файл LittleFS в соответствии с выбранным на данный момент именем env-cuiName в файл формата bit.

        3. Когда общий размер привысит или будет равен требуемому (bufferTotalWrite >= [размер промежуточного файла] * bitsPerPixel)
        сервер вернет дальнейшие инструкции (оформление будет применено или потребуется перезагрузка если мало памяти)
    */

    if (server) {

        server->on(
            "/api/direct/image", 
            HTTP_POST, 
            [this]() { this->apiDefaultOk(); },
            [this]() { this->apiDirectImage(); }
           // std::bind(&WebServerEink::apiDefaultOk, this), 
           // std::bind(&WebServerEink::apiDirectImage, this)
        );
    } else {
        
       Serial.println(F("[FATAL] WebServer not initialized"));
    }

    httpUpdater.setup(server, "/update", FPSTR(cfgOTALogin), FPSTR(cfgOTAPassword));
}

// без обработчиков вида server->on("/api/testimage", std::bind(&WebServerEink::apiDrawTest, this)); 
// выигрываем 2кб+ памяти

void WebServerEink::router() {

    if (fileCursor != -1) {        
         server->send(200, "application/json", "{\"status\":\"beasy\"}");      
         return;
    }

    if (defaultRoute(false)) {
    
        #if defined(RESET_MINUTE_TIMER_ON_WEB_REQUEST) 
            env->resetTimers(true);
        #endif
        
        return;
    } 

    bool routeFound = true;

    if (server->uri().indexOf("/out/uploader.js") != -1) {
        getUploaderJs();
    } else if (server->uri().indexOf("/out/uploader.tools.js") != -1) {
        getUploaderToolsJs();
    }  else if (server->uri().indexOf("/out/uploaderfm.js") != -1) {
        getUploaderFMJs();
    } else if (server->uri().indexOf("/out/language.js") != -1) {
        getLanguageJs();
    } else if (server->uri().indexOf("/out/style.uploader.css") != -1) {
        getUploaderCss();
    } else if (server->uri().indexOf("/upload/image") != -1) {
        showUploadImagePage();
    } else if (server->uri().indexOf("/api/testlowpower") != -1) {
        apiTestLowPower();
    } else if (server->uri().indexOf("/api/cui/format") != -1) {
        apiCuiFormat();
    } else if (server->uri().indexOf("/api/cui/list") != -1) {
        apiCuiList();
    } else if (server->uri().indexOf("/api/cui/delete") != -1) {
        apiCuiDelete();
    } else if (server->uri().indexOf("/api/cui/download") != -1) {
        apiCuiDownload();
    } else if (server->uri().indexOf("/api/direct/widgets") != -1) {
        apiDirectWidgets();
    } else if (server->uri().indexOf("/api/update") != -1) {
        apiUpdate();
    } else if (server->uri().indexOf("/api/clear") != -1) {
        apiClear();
    } else if (server->uri().indexOf("/api/clocktest") != -1) {
        apiClockTest();
    } else if (server->uri().indexOf("/api/partialtest") != -1) {
        apiPartialTest();
    } else if (server->uri().indexOf("/api/cui/select") != -1) {
        apiCuiSelect();
    } else if (server->uri().indexOf("/api/ntp/reset") != -1) {
        env->restartNTP();
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 
    } else if (server->uri().indexOf("/api/delete/notification") != -1) {
        env->lastError = "";
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 
    } else if (server->uri().indexOf("/api/hide/ip") != -1) {
        env->wifiInfo = "127.0.0.1"; // default
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 
    } else if (server->uri().indexOf("/api/buffer") != -1) {
        apiGetBuffer();
    } else if (server->uri().indexOf("/api/testloop") != -1) {
        env->cuiLoopNext();
        env->updateScreen();
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 
        return; 
    } else if (server->uri().indexOf("/api/mqttsync") != -1) {
        server->send(200, "application/json", env->mqttSendCurrentData() ? "{\"status\":\"ok\"}" : "{\"status\":\"fail\"}"); 
        return; 
    } else if (server->uri().indexOf("/api/testdata") != -1) {
        apiTestData();
    } else {
        
        String message = "404 Not Found\n\n";
        message += "URL: ";
        message += server->uri();
        message += "\nMethod: ";
        message += (server->method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server->args();
        message += "\n";

        routeFound = false;
        server->send(404, "text/plain", message);
    }

    if (routeFound) env->resetTimers();
}

void WebServerEink::apiClockTest() {
    #if defined(HELTEC_BW_15_S810F) || defined(WAVESHARE_R_BW_15_SSD1683)
        env->screen->enableClockMode(true);
        server->send(200, "application/json", "{\"status\":\"ok\"}"); 
    #endif
}

void WebServerEink::apiPartialTest() {
    env->defaultTime += 60;
    env->initDefaultTime();
    env->updateTime();
    env->screen->updatePartialClock();
    server->send(200, "application/json", "{\"status\":\"ok\"}"); 
}

void WebServerEink::apiCuiDownload() {

    String filename = "";
    bool settings = false;
    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "filename") {
            filename = server->arg(i);
        } else if (server->argName(i) == "type" && server->arg(i) == "settings") {
            settings = true;
        }
    }

    if (filename.length() <= 0) {
        server->send(200, "application/json", "{\"status\":\"fail\", \"error\":\"filename not specified\"}");  
        return;
    }

    filename.trim();
    filename += !settings ? ".bit" : ".ini";

    env->cuiInitFS();
    #if defined(ESP32)
    File file = SPIFFS.open("/cui/" + filename, FILE_READ); 
    #else
    File file = LittleFS.open("/cui/" + filename, "r"); 
    #endif
    
    if (!file) {
        server->send(200, "application/json", "{\"status\":\"fail\", \"error\":\"cant read file " + filename + "\"}");  
        return;
    }

    server->sendHeader("Content-Disposition", "attachment; filename=\"" + filename + "\"");
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");
    server->setContentLength(file.size());
    server->send(200, "application/octet-stream", "");

    const size_t bufferSize = 512;
    uint8_t buffer[bufferSize];
    size_t bytesSent = 0;
    while (file.available()) {
        size_t bytesToRead = std::min(bufferSize, static_cast<size_t>(file.available()));
        size_t bytesRead = file.read(buffer, bytesToRead);
        server->client().write(buffer, bytesRead);
        bytesSent += bytesRead;
    }
    
    // Закрываем файл
    file.close();
}

void WebServerEink::apiCuiDelete() {

    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "filename") {
        
            int newIndex = env->cuiGetIndexByName(server->arg(i));
            if (newIndex == -1) {

                Serial.print(F("[apiCuiSelect] CUI File not found : ")); 
                Serial.print(server->arg(i));
                
                server->send(200, "application/json", "{\"status\":\"fail\",\"error\":\"CUI File not found\"}"); 
                return; 
            }

            env->cuiDeleteStorageFile(server->arg(i));
            server->send(200, "application/json", "{\"status\":\"ok\",\"list\":" + env->cuiGetListFilesJSON() + "}"); 
            return; 
        }
    }

    server->send(200, "application/json", "{\"status\":\"no\",\"list\":" + env->cuiGetListFilesJSON() + "}"); 
}

  /*

void WebServerEink::apiCuiGetInfo() {
  
    переделать на скачивание файлов и разбор на клиентской стороне

    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "filename") {

            if (!env->cuiReadStorageFile(server->arg(i))) {

                server->send(200, "application/json", "{\"status\":\"fail\"}"); 

            } else {

                env->cuiInitFS();
                File file;
                KellyCanvas * screen = env->getCanvas();
                
                file = LittleFS.open("/cui/" + server->arg(i) + ".bit", "r"); 
                if (!file) {
                    Serial.println("Failed to open [display buffer] for reading");
                    server->send(200, "application/json", "{\"status\":\"fail read\"}"); 
                    return;
                }

                String json = "";

                json += "{";
                json += "\"screen\":{";
                json += "\"bitPerPixel\":" + String(screen->bitPerPixel) + ",";

                if (env->getConfig()->getBool(cScreenRotate)) {
                    json += "\"flip\":true,";
                } else {
                    json += "\"flip\":false,";
                }

                if (env->getConfig()->getBool(cScreenLandscape)) {
                    json += "\"landscape\":true";
                } else {
                    json += "\"landscape\":false";
                }

                json += "},";
                json += "\"widgets\":[";

                for (unsigned int b = 0; b < env->cuiWidgets.size(); b++) {
                    json += "{";
                    json += "\"id\":\"" + env->cuiGetIdByType(env->cuiWidgets[b].type) + "\",";
                    json += "\"x\":\"" + String(env->cuiWidgets[b].x) + "\",";
                    json += "\"y\":\"" + String(env->cuiWidgets[b].y) + "\",";
                    json += "\"enabled\":true,";
                    json += "},";
                }

                json += "],";


                // todo - переделать - загрузка в два этапа - буфер качаем через /api/cui/download а тут только инфа о виджетах
                // или можно оба файла просто качать и по месту разбирать на js там проще

                json += "\"simpleBuffer\":[";

                server->setContentLength(json.length() + env->cuiWebSize + 2);
                server->sendContent(json);

                json = "";
                int offset = 0;
                
                while (file.available() && offset < screen->bufferWidth) {

                    server->sendContent(String((unsigned int) file.read()));

                    offset++;
                    if (offset == screen->bufferWidth) {
                        
                    } else {
                        server->sendContent(",");
                    }
                    
                }

                server->sendContent("]}");
                server->sendContent("");
            }

            return; 
        }
    }

    server->send(200, "application/json", "{\"status\":\"fail\", \"error\":\"unknown name\"}");
} 
*/

void WebServerEink::apiCuiFormat() {

    if (server->method() != HTTP_POST) {
        server->send(200, "application/json", "{\"status\":\"fail\"}");  
        return;
    }

    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "confirm-format") {
            env->cuiSetState(false);
            env->resetPartialData();
            env->cuiInitFS(true);
            server->send(200, "application/json", "{\"status\":\"ok\"}");  
            return; 
        }
    }

    server->send(200, "application/json", "{\"status\":\"fail\"}");  
} 

void WebServerEink::apiCuiList() {
    
    server->send(200, "application/json", "{\"list\":" + env->cuiGetListFilesJSON() + ", \"info\":" + env->cuiGetFSInfoJSON() + "}"); 
} 

void WebServerEink::apiDefaultOk() {
    
    server->send(200, "application/json", "{\"status\":\"ok\"}"); 
}

void WebServerEink::apiDirectWidgets() {

    if (server->method() != HTTP_POST) {
        server->send(200, "application/json", "{\"status\":\"fail\"}");  
        return;
    }

    std::vector<uiWidgetStyle> widgets;
    bool commit = false;
    String screenFlip = "";
    String screenLandscape = "";
    String targetFilename = "";
    bufferTotalWrite = 0;
    cuiUploadDisplayAfter = false;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i) == "commit" && server->arg(i) == "1") {

            commit = true;
            continue;

        } else if (server->argName(i) == "screen-flip") {

            screenFlip = server->arg(i);
            continue;

        }  else if (server->argName(i) == "screen-landscape") {

            screenLandscape = server->arg(i);
            continue;

        } else if (server->argName(i) == "filename") {

            String inputFileName = server->arg(i);
            targetFilename = env->getConfig()->sanitizeFileName(inputFileName);            
            continue;

        } else if (server->argName(i) == "display-after-upload" && server->arg(i) == "1") {

            cuiUploadDisplayAfter = true;
            continue;

        } else if (server->argName(i) == "screen-bits") {
            
            int intState = 1;
            if(sscanf(server->arg(i).c_str(), "%d", &intState) != 1) {
                intState = 1;
            }

            if (intState <= 0 || intState > 2) intState = 1;
            // env->getCanvas()->setBitsPerPixel(intState); -- not enough memory, cant use that, just use buffer as half part
            env->cuiBits = intState;

            continue;

        } else if (server->argName(i).indexOf("widget") == -1) continue;

        // argument format : 
        // widget-[param name]-[ID] 

        String keyId = server->argName(i).substring(server->argName(i).lastIndexOf('-') + 1);

        int intState = 0;
        if(sscanf(keyId.c_str(), "%d", &intState) != 1) {
            intState = 0;
        }

        uiWidgetType type = env->cuiGetTypeById(intState);
        if (type == uiNone) {            
            Serial.print(F("Unknow widget key ID : ")); Serial.println(intState);
            continue;            
        } else {
            Serial.print(F("Widget input key ID : ")); Serial.println(intState);
        }

        int widgetKey = -1;
        for (unsigned int b = 0; b < widgets.size(); b++) {
            if (widgets[b].type == type) {                
                widgetKey = b;
                break;
            }
        }

        if (widgetKey != -1) {
            Serial.print(F("Duplicate widget setup in request")); 
            continue;
        }

        uiWidgetStyle newWidget;
                      newWidget.x = 0;
                      newWidget.y = 0;
                      newWidget.enabled = false;
                      newWidget.type = type;
                      newWidget.params = "";

        String rawParam = "";
        unsigned int inputCursor = 0;

        // widget settings string format
        // [enabled];[x];[y];[params]

        for (unsigned int b = 0; b < server->arg(i).length(); b++) {

            if (server->arg(i)[b] == ';') {

                if (inputCursor == 0 || inputCursor == 1 || inputCursor == 2) {
                                        
                    intState = 0;
                    if(sscanf(rawParam.c_str(), "%d", &intState) != 1) {
                        intState = 0;
                    }

                    // Serial.print(F("Read PARAM_N : ")); Serial.print(inputCursor); Serial.print(F(" RESULT : ")); Serial.print(intState);  Serial.print(F(" RAW : ")); Serial.println(rawParam);
                    
                    if (inputCursor == 0) {

                        newWidget.enabled = intState > 0 ? true : false;
                        if (!newWidget.enabled) break;

                    } else if (inputCursor == 1) {
                        
                        newWidget.x = intState;

                    } else if (inputCursor == 2) { 
                        
                        newWidget.y = intState;
                    }

                } else if (inputCursor == 3) {

                    newWidget.params = rawParam;

                } else {
                    break;
                }
                
                rawParam = "";
                inputCursor++;

            } else {
                rawParam += server->arg(i)[b];
            }
        }

        if (!newWidget.enabled) continue;
        widgets.push_back(newWidget);
    } 

    if (commit) {

        env->cuiResetWidgets();

        if (screenFlip.length() > 0) {            
            env->getConfig()->set(cScreenRotate, screenFlip);
        }

        if (screenLandscape.length() > 0) {
            env->getConfig()->set(cScreenLandscape, screenLandscape);
        }

        if (widgets.size() > 0) {

            String state = "";
            for (unsigned int b = 0; b < widgets.size(); b++) {

                state = "[Widget config] ";

                if (widgets[b].enabled) state += "Enabled";
                else  state += "Disabled";

                state += " Offset : " + String(widgets[b].x) + " - " + String(widgets[b].y);

                if (widgets[b].enabled && env->cuiSetWidget(widgets[b])) {
                    state += " [Success]";
                } else {
                    state += " [Fail] Max widgets limit reached";
                }

                Serial.println(state);
            }
        } else {
            Serial.println(F("Widgets options, not recieved, skip config"));
        }

        env->cuiSetState(true, targetFilename);
        env->resetPartialData();
        
        cuiUploadMode = true;
        server->send(200, "application/json", "{\"status\":\"enabled\"}");  

    } else {

        env->cuiSetState(false);
        env->resetPartialData();
        env->updateScreen();
        cuiUploadMode = false;
        server->send(200, "application/json", "{\"status\":\"disabled\"}");  
    }
}

void WebServerEink::apiCuiSelect() {

    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i) == "filename") {

            int newIndex = env->cuiGetIndexByName(server->arg(i));
            if (newIndex == -1) {

                Serial.print(F("[apiCuiSelect] CUI File not found : ")); 
                Serial.print(server->arg(i));
                
                server->send(200, "application/json", "{\"status\":\"fail\",\"error\":\"CUI File not found\"}"); 
                return; 
            }

            String comm = "";

            if (env->lastState.cuiLoop) {
                env->lastState.cuiFileIndex = newIndex;
            } else {
                env->cuiSetState(true, server->arg(i));
            }

            env->resetPartialData();

            if (env->cuiPrepareRebootIfNeeded()) {
                
                env->lastState.cuiResetOnReboot = true;
                comm = ",\"reboot_required\":true";
                
            } else {
                env->updateScreen();
            }

            server->send(200, "application/json", "{\"status\":\"ok\"" + comm + "}"); 
            return; 
        }
    }

    server->send(200, "application/json", "{\"status\":\"fail\",\"error\":\"name not specifed\"}"); 
}

void WebServerEink::apiDirectImage() {    

    if (!env->cuiIsEnabled() || !cuiUploadMode) {
        server->send(200, "application/json", "{\"status\":\"fail\"}");  
        return;
    }
    KellyCanvas * screen = env->getCanvas();
    unsigned int maxOffset = screen->bufferWidth-1;

    HTTPUpload& upload = server->upload();

    if (upload.status == UPLOAD_FILE_START) {

        fileCursor = 0;
        // String filename = upload.filename;
        // if (!filename.startsWith("/")) { filename = "/" + filename; }
        // Serial.println("File " + filename);

    } else if (upload.status == UPLOAD_FILE_WRITE) {

        Serial.print(F("[Image buffer upload] write ")); Serial.print(upload.currentSize); Serial.println(F(" bytes"));

        for (unsigned int i = 0; i < upload.currentSize; i++) {
            
            if (fileCursor + i > maxOffset) {
                break;
            }

            screen->bufferBW[fileCursor + i] = (unsigned char) upload.buf[i];
        }

        fileCursor += upload.currentSize;
        env->resetTimers(); 

    } else if (upload.status == UPLOAD_FILE_END) {

        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(CONTENT_LENGTH_UNKNOWN);

        fileCursor = -1;

        // if (upload.totalSize == maxOffset+1) {
        // Adds recieved file data to CUI file from screen buffer array that used as temp storage

        if (upload.totalSize > 512) {

            env->resetTimers();

            bool append = false;
            if (bufferTotalWrite > 0) {
                append = true;
            }
            
            bufferTotalWrite += upload.totalSize;
            
            Serial.println(F("[Direct input] File upload ended. Write to FS"));
            if (append) {
                Serial.println(F("[Direct input] ADD"));
            } else {
                Serial.println(F("[Direct input] CLEAR AND WRITE NEW FILE"));
            }

            if (!env->cuiWriteStorageFile(append, upload.totalSize)) {

                Serial.println(F("[Direct input] FAIL DURING cuiWrite write process"));

                env->cuiDeleteStorageFile(env->cuiName);
                env->cuiResetStateByConfig();
                cuiUploadMode = false;

                server->send(200, "application/json", "{\"status\":\"fail\"}");  
                return;
            }

            // [screen buffer full size] = ( Width x Height / 8 ) * [bit per pixel]
            // this final binary data sended from client splited by parts 
            // [screen buffer full size] / [bit per pixel]
            
            // 1bit graphic - 400 x 300 / 8 = 15kb
            // 2bit graphic - 400 x 300 / 8 * 2 = 30kb

            // In case 2bit image buffer we wait second file upload and keep counting bufferTotalWrite
            // since we can accept only ~15kb by one POST iteration without any crashing \ low memory issues on ESP8266

            unsigned int requiredSize = ((env->canvas->height * env->canvas->width) / 8) * env->cuiBits;

            if (bufferTotalWrite >= requiredSize) { 
                
                if (cuiUploadDisplayAfter) {

                    // if (env->cuiBits > 1 && screen->bitPerPixel != env->cuiBits) {
                    if (env->cuiPrepareRebootIfNeeded()) {
                        
                        env->lastState.cuiResetOnReboot = true;
                        server->send(200, "application/json", "{\"status\":\"ok\",\"full\":true,\"reboot_required\":true}");  

                    } else {
                        env->updateScreen();
                        server->send(200, "application/json", "{\"status\":\"ok\",\"full\":true}");  
                    } 

                } else {

                    env->cuiResetStateByConfig();
                    server->send(200, "application/json", "{\"status\":\"ok\",\"full\":true}");
                }

                bufferTotalWrite = 0;
                cuiUploadMode = false;
                Serial.println(F("[Direct input] DONE"));

            } else {
                server->send(200, "application/json", "{\"status\":\"ok\"}");  
            }
           
    } else {
        server->send(200, "application/json", "{\"status\":\"fail\",\"err\":\"Too small buffer data " + String(upload.totalSize) + "/" + String(screen->bufferWidth) + "\"}");  
    }
    
    server->sendContent("");
  }

}

void WebServerEink::getUploaderFMJs() {    
 
     if (ramFriendlyMode) {
    
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUIFileManager_js);

        server->send(200, "text/javascript; charset=utf-8", "");
        
        outputROMData(webdata_KellyUIFileManager_js, webdataSize_KellyUIFileManager_js);

       server->sendContent("");
    } else {


        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUIFileManager_js);
        server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_KellyUIFileManager_js)); 
    }
}

void WebServerEink::getLanguageJs() {    
    
    if (ramFriendlyMode) {
    
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_locale_js);

        server->send(200, "text/javascript; charset=utf-8", "");
        
        outputROMData(webdata_locale_js, webdataSize_locale_js);

        server->sendContent("");
    } else {


        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_locale_js);
        server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_locale_js)); 
    }
}

void WebServerEink::getUploaderCss() {    
    
    server->sendHeader("Cache-Control", "max-age=31536000");
    server->setContentLength(webdataSize_style_uploader_css);
    server->send(200, "text/css; charset=utf-8", FPSTR(webdata_style_uploader_css)); 
}


void WebServerEink::getUploaderToolsJs() {    
    
    if (ramFriendlyMode) {
    
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUploader_tools_js);

        server->send(200, "text/javascript; charset=utf-8", "");
        
        outputROMData(webdata_KellyUploader_tools_js, webdataSize_KellyUploader_tools_js);

        server->sendContent("");
    } else {


        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUploader_tools_js);
        server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_KellyUploader_tools_js)); 
    }
}

void WebServerEink::getUploaderJs() {    
    
    if (ramFriendlyMode) {
    
        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUploader_js);

        server->send(200, "text/javascript; charset=utf-8", "");
        
        outputROMData(webdata_KellyUploader_js, webdataSize_KellyUploader_js);

        server->sendContent("");
    } else {


        server->sendHeader("Cache-Control", "max-age=31536000");
        server->setContentLength(webdataSize_KellyUploader_js);
        server->send(200, "text/javascript; charset=utf-8", FPSTR(webdata_KellyUploader_js)); 
    }
}

void WebServerEink::showUploadImagePage() {
    
    String currentCfg = env->getConfig()->getOptionsJSON();

    currentCfg += "var ENVSCREEN = {";
    currentCfg += "\"width\":" + String(env->canvas->getWidth()) + ",";
    currentCfg += "\"height\":" + String(env->canvas->getHeight()) + ",";
    currentCfg += "\"bitPerPixel\":" + String(env->canvas->bitPerPixel) + ",";
        
    if (env->screen->is4ColorsSupported()) {
        currentCfg += "\"color4\":true,";
    } else {
        currentCfg += "\"color4\":false,";
    }        

    if (env->getConfig()->getBool(cScreenLandscape)) {
        currentCfg += "\"landscape\":true,";
    } else {
        currentCfg += "\"landscape\":false,";
    }

    if (env->getConfig()->getBool(cScreenRotate)) {
        currentCfg += "\"flip\":true,";
    } else {
        currentCfg += "\"flip\":false,";
    }

    currentCfg += "};";
           
    server->setContentLength(currentCfg.length() + 
                            webdataSize_client_html + 
                            webdataSize_client_uploader_html + 
                            webdataSize_client_end_html + 
                            webdataSize_initUpload_js);

    if (ramFriendlyMode) {

        server->send(200, "text/html", "");
        
        outputROMData(webdata_client_html, webdataSize_client_html);
        
        server->sendContent(currentCfg);
        
        outputROMData(webdata_client_uploader_html, webdataSize_client_uploader_html);
        outputROMData(webdata_initUpload_js, webdataSize_initUpload_js);

        outputROMData(webdata_client_end_html, webdataSize_client_end_html);
        
        server->sendContent("");

    } else {
        String response = FPSTR(webdata_client_html);
               response += currentCfg;
               response += FPSTR(webdata_client_uploader_html);
               response += FPSTR(webdata_initUpload_js);
               response += FPSTR(webdata_client_end_html);

        server->send(200, "text/html", response); 
    }
}

String WebServerEink::getInfo() {

    String json = WebServerBase::getInfo(); // "{";

    env->updateTelemetry();
    rtcData & lastState = env->getCurrentState();
    
    int lkey = lastState.lastTelemetrySize - 1;
    Serial.print(F("Currently collected telemetry size : ")); Serial.println(lastState.lastTelemetrySize); 
   
    json += ",";
    json += "\"notifications\":[";

        if (env->lastError.length() > 0) {
            json += "{\"id\":\"1\",\"text\":\"" + env->lastError + "\",\"d\":1,\"t\":0,\"tc\":0,\"s\":1}";        
        }

    json += "]";

    json += ",";

    json += "\"sensors\":{";

    // recheck battery state 
    float batTest = env->readBatteryV();
    env->batteryInit = false;

        if (lastState.lastTelemetrySize > 0) {
            json += "\"temperature\":" + String(lastState.lastTelemetry[lkey].temperature) + ",";
            json += "\"humidity\":" + String(lastState.lastTelemetry[lkey].humidity) + ",";
            json += "\"pressure\":" + String(lastState.lastTelemetry[lkey].pressure) + ",";

            if (lastState.extData.isDataValid) {

                json += "\"temperature__external\":" + String(lastState.extData.temperature) + ",";
                json += "\"humidity__external\":" + String(lastState.extData.humidity) + ",";
                if (lastState.extData.pressure > BAD_SENSOR_DATA) {
                    json += "\"pressure__external\":" + String(lastState.extData.pressure) + ",";
                }
            }

            #if defined(CO2_SCD41)
            if (env->scd4XCO2 != 0) {

                json += "\"temperature__scd4x\":" + String(env->scd4XTemp) + ",";
                json += "\"humidity__scd4x\":" + String(env->scd4XHumidity) + ",";
                json += "\"co2__scd4x\":" + String(env->scd4XCO2) + ",";
            }
            #endif

            json += "\"battery_sensor_v\":" + String(batTest) + ",";
            json += "\"battery_sensor_percent\":" + String((int) round(env->getBatteryLvlfromV(batTest))) + ",";
            json += "\"battery\":"; 

            if (env->isOnBattery()) {
                json += "true";
            } else {
                json += "false";
            }

            json += ",";

            json += "\"mqtt\":";
            if (env->mqttSuccess) {
                json += "true";
            } else {
                json += "false";
            }

            json += ",";
            json += "\"ext_last_sync\":\""  + env->getFormattedExtSensorLastSyncTime(true) + "\",";
            json += "\"ext_connect_times\":" + String(env->lastState.connectTimes);
            // ext battery
            // ext data : raw temp & hum
            // todo - add RAM meter
        }

    json += "}";

    return json;
}

/*
    Test display states & scenarios

    http://192.168.0.1/api/testdata?preset=ext_winter

    // changes Date & Time + Temperature & Humidity

    ext_winter -- cold day, no clouds 
    ext_summer || ext_day -- warm summer day, no clouds
    ext_overheat -- summer day +35C
    ext_rain   -- summer rainy
    ext_night  -- after 20:00
    ext_night_cloudy -- after 20:00 with clouds
    ext_night_sleep -- after 00:00
    ext_night_sleep_cloudy -- after 00:00 with clouds
    ext_day_cloudy  -- summer day, clouds
    ext_day_cloudy_cold -- winter clouds
    ext_day_snow -- winter snow

    // changes Battery level

    ext_bat_full --  100%
    ext_bat_mid -- ~50%
    ext_bat_low | ext_bat_low2 -- 5-11%

    // bad input from external sensor

    ext_bad_data

    // partial update test mode 

    partial_update | partial_update2

    cold = 1 - force temperature < 0

*/
void WebServerEink::apiTestData() {

    String result = "fail";
    bool cold = false;

    for (int i = 0; i < server->args(); i++)  {

        if (server->argName(i).indexOf("cold") != -1) {

            cold = true;
            Serial.println(F("[apiTestData] Low temp - enabled"));

        } else if (server->argName(i).indexOf("preset") != -1) {
        
            if (env->lastState.lastTelemetrySize <= 0 || env->lastState.lastTelemetry[env->lastState.lastTelemetrySize-1].temperature <= BAD_SENSOR_DATA) {
                
                Serial.println(F("[apiTestData] No sensor detected - used default telemetry"));

                env->lastState.lastTelemetrySize = 1;
                env->lastState.lastTelemetry[0].bat = 50;
                env->lastState.lastTelemetry[0].humidity = 40.0f;
                env->lastState.lastTelemetry[0].temperature = 23.0f;
                env->lastState.lastTelemetry[0].pressure = 1200.0f  * 100.0f;
            }
        
            if (server->arg(i) == "ext_bad_data") {   
                
                result = server->arg(i);
                env->lastState.extData.temperature = BAD_SENSOR_DATA;
                env->lastState.extData.humidity = BAD_SENSOR_DATA;
                env->lastState.extData.bat = BAD_SENSOR_DATA;
                env->lastState.extData.pressure = BAD_SENSOR_DATA;
                env->lastState.extData.isDataValid = false;
                env->lastState.extData.t = time(nullptr);

            } else if (server->arg(i) == "partial_update" ) {               
                
                #if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176_B)
                
                    // Serial.println("[Deep sleep INIT 4 sec]");  
                    // env->lastState.t = time(nullptr);
                    // env->lastState.partialUpdateTest = true;
                    // env->saveCurrentState();

                    if (server->method() == HTTP_POST) env->screen->updateTestPartial();
                    
                    // ESP.deepSleep(4 * 1000000);
                    
                    server->send(200, "application/json", "{\"status\":\"updateTestPartial\"}");
                    return;
                #endif

            } else if (server->arg(i) == "partial_update2") {   
                
                result = server->arg(i);
                #if defined(WAVESHARE_RY_BW_42_UC8176) || defined(WAVESHARE_BW_42_UC8176) || defined(WAVESHARE_BW_42_SSD1683) || defined(WAVESHARE_RY_BW_42_UC8176_B)
                
                    if (server->method() == HTTP_POST) env->screen->updateTestPartial2();
                    
                    server->send(200, "application/json", "{\"status\":\"updateTestPartial2\"}");
                    return;
                #endif

            } else if (server->arg(i) == "ext_bat_full" || server->arg(i) == "ext_bat_mid" || server->arg(i) == "ext_bat_low" || server->arg(i) == "ext_bat_low2"){

                result = server->arg(i);
                env->lastState.extData.temperature = 12.12f;
                env->lastState.extData.humidity = 48.7f;

                if (server->arg(i) == "ext_bat_mid") {
                    env->lastState.extData.bat = 68;  
                } else if (server->arg(i) == "ext_bat_low") {
                    env->lastState.extData.bat = 5;  
                } else if (server->arg(i) == "ext_bat_low2") {
                    env->lastState.extData.bat = 11;  
                } else {
                    env->lastState.extData.bat = 100; 
                }

                env->lastState.extData.isDataValid = true;
                env->lastState.extData.t = time(nullptr);

            } else if (server->arg(i) == "ext_summer" || 
                    server->arg(i) == "ext_winter" || 
                    server->arg(i) == "ext_overheat" || 
                    server->arg(i) == "ext_rain" || 
                    server->arg(i) == "ext_night_cloudy" || 
                    server->arg(i) == "ext_night" || 
                    server->arg(i) == "ext_night_sleep" || 
                    server->arg(i) == "ext_night_sleep_cloudy" || 
                    server->arg(i) == "ext_day_cloudy_cold" ||
                    server->arg(i) == "ext_day_snow" ||  
                    server->arg(i) == "ext_day" || 
                    server->arg(i) == "ext_day_cloudy"
            ) {

                result = server->arg(i);
                if (server->arg(i) == "ext_winter") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-02-03 12:32:04";
                    env->lastState.extData.temperature = -21.12f;
                    env->lastState.extData.humidity = 67.8f;

                } else if (server->arg(i) == "ext_overheat") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-07-03 15:32:04";
                    env->lastState.extData.temperature = 40.12f;
                    env->lastState.extData.humidity = 10.8f;

                } else if (server->arg(i) == "ext_rain") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 15:32:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = ICON_RAIN_DETECT_RAINY_HUM+1;             
                    env->lastState.extData.pressure = (ICON_RAIN_DETECT_RAINY_HPA-10) * 100.0f; 

                } else if (server->arg(i) == "ext_day_cloudy_cold") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-01-03 15:32:04";
                    env->lastState.extData.temperature = -25.12f;
                    env->lastState.extData.humidity = ICON_SNOW_DETECT_CLOUDY_HUM+1;             
                    env->lastState.extData.pressure = (ICON_SNOW_DETECT_CLOUDY_HPA-10) * 100.0f; 

                } else if (server->arg(i) == "ext_day_snow") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-01-03 15:32:04";
                    env->lastState.extData.temperature = -25.12f;
                    env->lastState.extData.humidity = ICON_SNOW_DETECT_SNOW_HUM+1;             
                    env->lastState.extData.pressure = (ICON_SNOW_DETECT_SNOW_HPA-10) * 100.0f; 

                } else if (server->arg(i) == "ext_night") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 22:32:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = 20.0f;
                    env->lastState.extData.pressure = 1200.0f * 100.0f;

                } else if (server->arg(i) == "ext_night_cloudy") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 22:30:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = ICON_RAIN_DETECT_CLOUDY_HUM+1;                
                    env->lastState.extData.pressure = (ICON_RAIN_DETECT_CLOUDY_HPA-10) * 100.0f;

                } else if (server->arg(i) == "ext_night_sleep") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 00:30:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = 20.0f;
                    env->lastState.extData.pressure = 1200.0f * 100.0f;

                }  else if (server->arg(i) == "ext_night_sleep_cloudy") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 00:30:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = ICON_RAIN_DETECT_CLOUDY_HUM+1;                
                    env->lastState.extData.pressure = (ICON_RAIN_DETECT_CLOUDY_HPA-10) * 100.0f;

                }  else if (server->arg(i) == "ext_day_cloudy") {

                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-03 15:30:04";
                    env->lastState.extData.temperature = 25.12f;
                    env->lastState.extData.humidity = ICON_RAIN_DETECT_CLOUDY_HUM+1;                
                    env->lastState.extData.pressure = (ICON_RAIN_DETECT_CLOUDY_HPA-10) * 100.0f;

                } else { // summer

                    result = "default preset - summer day";
                    env->lastState.extData.temperature = 27.12f;
                    env->lastState.extData.humidity = 50.7f;
                    env->getConfig()->cfgValues[cTimestamp] = "2024-06-12 15:44:04";
                }

                env->lastState.extData.bat = 68;  
                env->lastState.extData.isDataValid = true;
                env->lastState.extData.t = time(nullptr);

                std::vector<cfgOptionKeys> updatedKeys;
                updatedKeys.push_back(cTimestamp);
                env->validateConfig(-1, &updatedKeys);
            } else {
                
                result = "unknown preset";
            }
        }
    }
    
    if (cold) {
        env->lastState.extData.temperature = -20.0f;
    }
    env->lastState.extData.icon = kowUnknown;
    env->updateExtIconState();
    env->updateScreen();
    server->send(200, "application/json", "{\"status\":\"" + result + "\"}");

}

void WebServerEink::apiGetBuffer() {

    String bufferId = "";
    
    for (int i = 0; i < server->args(); i++)  {
        if (server->argName(i).indexOf("id") == -1) continue;
        bufferId = server->arg(i);
        break;
    }

    server->sendHeader("Content-Disposition", "attachment; filename=\"" + bufferId + ".txt\"");
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");
  
    env->updateTime();
    env->screen->drawUIToBuffer();

    KellyCanvas * screen = env->getCanvas();
    if (bufferId == "buffer2") {
        screen->initBuffer(2);
    } 

    if (!ramFriendlyMode) {

        String tt = "";
        for (int i = 0; i < env->canvas->bufferWidth; i++) {

            if (bufferId == "buffer2") {
                tt += env->canvas->bufferRY[i];
            } else {
                tt += env->canvas->bufferBW[i];
            }

            tt += ',';               
        }

         server->setContentLength(tt.length());
         server->send(200, "application/json", tt);

    } else {

        server->setContentLength(CONTENT_LENGTH_UNKNOWN);
        server->send(200, "text/plain", "");
        //env->screen->drawClock();

        String tt = "";
        for (int i = 0; i < env->canvas->bufferWidth; i++) {

            if (bufferId == "buffer2") {
                tt = env->canvas->bufferRY[i];
            } else {
                tt = env->canvas->bufferBW[i];
            }

            tt += ',';               
            server->sendContent(tt);
        }

        // screen->setBitsPerPixel(1);
        
        server->sendContent("");
        server->client().stop(); 
    }
}

/*
void WebServerEink::apiDrawTest() {

    server->send(200, "application/json", "{\"status\":\"ok\"}");

    env->screen->clearScreen();
    env->workEnabled = false;

    KellyCanvas * screen = env->getCanvas();
    screen->clear();
    screen->setRotate(0);

    screen->color = tBLACK;
    screen->drawImage(0, 0, &testImage_400x300b1_settings, true, false);
    screen->color = tRY;
    screen->drawImage(0, 0, &testImage_400x300b2_settings, true, false);

    screen->color = tBLACK;
    env->screen->updateScreen();
}
*/

void WebServerEink::apiClear() {

    server->send(200, "application/json", "{\"status\":\"ok\"}");

    env->screen->clearScreen();
    env->workEnabled = false;
}

void WebServerEink::apiTestLowPower() {

    server->send(200, "application/json", "{\"status\":\"ok\"}");

    env->screen->drawUILowBat();
    env->resetTimers();
    env->screen->updateScreen();
    env->workEnabled = false;    
}

void WebServerEink::apiUpdate() {

    server->send(200, "application/json", "{\"status\":\"ok\"}");

    
    Serial.println(F("[apiUpdate] : refresh telemetry "));

    env->resetPartialData();
    env->updateTelemetry();
    
    Serial.println(F("[apiUpdate] : update external sensor "));

    env->updateExtSensorData();
    env->updateExtIconState();

    // mqttSendCurrentData, keep less operations to prevent soft-wdt-reset
    Serial.println(F("[apiUpdate] : update screen"));

    env->resetTimers();
    env->updateScreen();

    env->workEnabled = true;    
}