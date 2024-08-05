function KellyUIFM() {
    
    var handler = this;    
    
    function getList() {
        if (KellyTools.locked()) return false;
        // KellyTools.showNotice("Загружаю список оформлений...");
        handler.filelist.list = false;
        KellyTools.cLock = true;
        handler.filelist.show(function() {KellyTools.cLock = false; });
    }
    
    // при клике на "вывести на экран" - сохранять в файл default - его и выводить поумолчанию при переключении режима в cui. Остальные уже на усмотрение пользователя
    // #3# - date - выходит за границы
    
    handler.filelist = KellyTools.getSelectWidget({
        tpl : '#0# #1#кб #2#бит <button href="#" data-action="/api/cui/delete">X</button><button href="#" data-id="toscreen">На экран</button><button href="#" data-id="preview">Превью</button><button href="#" data-id="download">Скачать</button><div class="clear"></div>',
        valueIndex : 0,
        onLoadList : function(self, rawData) { return self.getListData(rawData);},        
        onUpdate : function(self) {
            
            KellyTools.showNotice(false);
            if (self.list.length <= 0 && self.lastError === false) {
                self.containerEl.innerHTML = 'Файлов стилей нет';
                KellyTools.updateSpoilerBounds();  
                return;
            }
            
            var actBtns = self.containerEl.getElementsByTagName('button');
            for (var i = 0; i < actBtns.length; i++) {
                actBtns[i].onclick = function() {
                    
                    var item = KellyTools.getParentByClass(this, 'dselect-item');
                    if (KellyTools.locked()) return false;
                    
                    // if (this.getAttribute('data-link')) {
                        
                       // var link = document.createElement('A');
                       //     link.target = "_blank";
                       //     link.href = this.getAttribute('data-link') + "?filename=" + item.getAttribute("data-value");
                       //     link.click();
                            
                    // } else { 
                     if (this.getAttribute('data-id') == 'preview') {
                         
                            handler.preloadJSON(item.getAttribute("data-value"), function(json) {
                                window.ENV.widgetsUpdatedByCfg = false;
                                window.ENV.preloadedJSON = json;
                                window.ENV.initPreviewFromInput();                                
                            });
                            
                     } else if (this.getAttribute('data-id') == 'download') {
                         
                            handler.preloadJSON(item.getAttribute("data-value"), function(json) {
                                 KellyTools.downloadFile(JSON.stringify(json), item.getAttribute("data-value") + '_' + screen.width +'x' + screen.height + '_' + 'bw' + '.json', "application/json");
                            });
                            
                     } else if (this.getAttribute('data-id') == 'toscreen') {
                         
                            KellyTools.cLock = true;
                            KellyTools.showNotice("Загружаю данные... ");  
                            
                            var sform = new FormData();
                                sform.append("filename", item.getAttribute("data-value"));
                                
                            KellyTools.cfetch('/api/cui/select', {method : 'POST', body : sform, responseType : 'json'}, function(response, error) {
                                
                                if (!response || response.status == "fail") {
                                    
                                    KellyTools.showNotice("Нет обратной связи с устройством. " + error);  
                                    
                                } else if (response.reboot_required) {
                            
                                    var formData = new FormData();
                                        formData.append('reboot', '1');  
                                        
                                    KellyTools.cfetch("/api/reboot", {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {}); 
                                    KellyTools.showNotice("Перезагружаюсь для переинициализации буфера экрана!");
                                    
                                } else {
                                    KellyTools.showNotice("Готово!");   
                                }
                                
                                KellyTools.cLock = false;
                            }, {timeout : 20000}); 
                            
                     } else { // delete
                        
                        var html = 'Подтвердите удаление файла <a href="#" class="k-important" id="ui-files-confirm-delete" data-action="' + this.getAttribute('data-action') + '" data-value="' + item.getAttribute("data-value") + '">Удалить</a>';
                        var onNoticeShow = function() {                             
                            gid('ui-files-confirm-delete').onclick = function() {    
                            
                                var sform = new FormData();
                                    sform.append("filename", this.getAttribute("data-value"));
                                
                                KellyTools.cLock = true;  
                                KellyTools.cfetch(this.getAttribute('data-action'), {method : 'POST', body : sform, responseType : 'json'}, function(response, error) {
                                    
                                    KellyTools.cLock = false;
                                    
                                    if (error) {
                                        console.log(error);
                                    } else {                                    
                                        console.log(response);
                                        handler.filelist.list = self.getListData(response);
                                        handler.filelist.updateList();                                   
                                    }
                                }); 
                                 
                                 return false;
                            };
                        };
                        
                        KellyTools.showNotice(html, false, onNoticeShow);
                        
                    }
                    return false;
                }
            }
            
            KellyTools.updateSpoilerBounds();  
        }, 
        
        getListUrl : '/api/cui/list',
        onSelectItem : function(self, item, selected) {},
        list : [],
        cId : 'ui-files-list',
    });
    
    handler.preloadJSON = function(filename, onSuccess) {
     
        if (handler.downloader) {
            handler.downloader.abort();
            handler.downloader = false;
            KellyTools.showNotice("Процесс отменен", true);     
            return false;
        }
        
         var sform = new FormData();
             sform.append("filename", filename);
        
         var pJSON = {simpleBuffer : [], widgets : [], screen : {}};
         handler.downloader = KellyTools.cfetch("/api/cui/download", {method : 'POST', body : sform, responseType : 'arrayBuffer'}, function(response, error) {
            
            handler.downloader = false;
            if (!response) {      
                console.log('Bad response for screen buffer file data'); console.log(error); 
            } else {
                
                var view = new Uint8Array(response);
                for (var i = 0; i < view.length; i++) {
                    pJSON.simpleBuffer[i] = view[i];
                }
                
                sform.append("type", "settings");
                handler.downloader = KellyTools.cfetch("/api/cui/download", {method : 'POST', body : sform, responseType : 'text'}, function(response, error) {
                    
                    handler.downloader = false;
                    if (!response) {   
                        console.log('Bad response for widgets data'); console.log(error);  
                    } else if (response.indexOf('"status":"fail"') != -1) {
                        console.log('Bad response for widgets data'); console.log(response);
                        KellyTools.showNotice("Ошибка файловой системы устройства. Невозможно открыть файл", true);     
                    } else {
                        
                        var strings = response.split("\n");                        
                        for (var i = 0; i < strings.length; i++) {     
                            
                            console.log(strings[i]);
                            var values = strings[i].split(';');                            
                            if (i == 0) {
                                
                                pJSON.screen.bitPerPixel = parseInt(values[0]);
                                pJSON.screen.width = ENVSCREEN.width;
                                pJSON.screen.height = ENVSCREEN.height;
                                pJSON.screen.flip = parseInt(values[1]) ? true : false;
                                pJSON.screen.landscape = parseInt(values[2]) ? true : false;
                                
                            } else if (values[0].length > 0) {                                
                               pJSON.widgets.push({
                                    id : window.ENV.getWidgetByKey(values[0], 'uuid').id,
                                    x : parseInt(values[1]),
                                    y : parseInt(values[2]),
                                    enabled : true,
                                    params : values[3],
                              });
                            }
                        }
                        
                        console.log( pJSON);
                        onSuccess(pJSON);                        
                    }
                    
                }); 
            }
        });
        
        return true;        
   };
    
   handler.filelist.getListData = function(rawData) {
        
        if (typeof rawData == 'undefined') rawData = {list : []};
        if (!rawData.list) rawData.list = [];
        
        var listInfo = document.getElementById('ui-files-list-info');
        if (rawData.info && listInfo) {
            listInfo.innerText = "Занято : " + (rawData.info.usedBytes / 1000) + " / " + (rawData.info.totalBytes / 1000) + "кб";            
        }
        
        rawData = rawData.list;
        var list = [];
        for(var i = 0; i < rawData.length; i++) {
            
            var dt = KellyTools.validateInt(rawData[i].dt_update ? rawData[i].dt_update : rawData[i].dt_create);
                dt = dt ? new Date(dt) : new Date();
                dt = dt.toISOString().slice(0, 19).replace('T', ' ');
            
            list.push([rawData[i].filename, rawData[i].size / 1000, rawData[i].bits, dt]);
        }
        
        return list;
    };
    
    handler.init = function() {
        
        // gid('ui-files-load-list').onclick = getList;        

        gid('ui-files-manager-opener').addEventListener('click', function() {
            setTimeout(function() {
                console.log(document.getElementById('disp-ui-options').classList.contains('show'));
                if (document.getElementById('disp-ui-options').classList.contains('show')) {
                    getList();
                }
            }, 100);
        });
        
        gid('ui-files-save').onclick = function() {
            
             if (!window.ENV.getUploadSettings().uploadReady) {
                 KellyTools.showNotice('Нечего сохранять');
                 return;
             }
             
             if (KellyTools.locked()) return;
             var selected = handler.filelist.containerEl.querySelector('.dselect-item.selected');
             var defaultName = '';
             if (selected) defaultName = selected.getAttribute('data-value');
             
             var onNoticeShow = function() {                      
                 gid('ui-files-save-ok').onclick = function() {
                     if (KellyTools.locked()) return false;
                     if (!gid('ui-files-newname').value) return false;
                     
                      window.ENV.defaultFileName = gid('ui-files-newname').value.trim(); 
                      window.ENV.uploadCurrent(function(success, response) {
                            if (success) {
                                if (response && response.reboot_required) {
                                    return;
                                } else setTimeout(getList, 100);
                            }
                            
                            window.ENV.defaultFileName = "";
                            // handler.filelist.list.push([window.ENV.defaultFileName, ENV.getSB().bufferLen / 1000, ENVSCREEN.bitPerPixel, new Date().toISOString().slice(0, 19).replace('T', ' ')]);
                            // handler.filelist.show();
                      }, {displayAfterUpload : false});
                     
                     return false;
                 };
             };
             
             KellyTools.showNotice('Введите название файла <input type="text" id="ui-files-newname" value="' + defaultName + '"> <a href="#" id="ui-files-save-ok">OK</a>', false, onNoticeShow);
        };
        
        if (gid('ui-files-format')) gid('ui-files-format').onclick = function() {
             if (KellyTools.locked()) return;
             
            var onNoticeShow = function() {
                gid('ui-format-confirm').onclick = function() {    
                    KellyTools.cLock = true;
                     var sform = new FormData();
                         sform.append("confirm-format", "1");
                         
                     KellyTools.cfetch("/api/cui/format", {method : 'POST', body : sform, responseType : 'json'}, function(response, error) {
                        KellyTools.cLock = false;
                        
                        if (!response) {
                            KellyTools.showNotice("Устройство не отвечает", true); 
                        } else {
                            KellyTools.showNotice("Готово!");
                            setTimeout(getList, 100);
                        }
                    });
                    return false;
                };
            };
             
            KellyTools.showNotice('Подтвердите полную очистку памяти <a href="#" class="k-important" id="ui-format-confirm">Форматировать</a>', false, onNoticeShow);
        };
        
        KellyTools.initSpoilers();
        
    };
}