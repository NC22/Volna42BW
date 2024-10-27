/* 
    common web environment for esp projects
    KellyEnv v1.0
*/

function KellyEnv(env) {
    
    var handler = this;
    
    var env = env;
    var lng;
    
    var cVarCls = 'kelly-commitable-option';
    
    var urlSave = '/api/commit';
    var urlGetWifi = '/api/wifilist';
    var urlGetInfo = '/api/getinfo';
    var urlReset = '/api/reset';
    var urlReboot = '/api/reboot';
    
    var hiddenKeys = ['modeList', 'modeListEnabled', 'externalHosts']; // 'ntpHosts', 
    var listKeys = ['modeList', 'externalHosts']; // , 'ntpHosts'
    var widgets = [];
        
    function initActions() {
        
        var act = document.getElementsByClassName('k-options-action');
        for (var i = 0; i < act.length; i++) {
            act[i].onclick = function(e) {
                 
                 var key = this.getAttribute('data-key');
                 if (KellyTools.locked()) return false;
                 
                 if (key == 'restore') {
                     
                     if (this.getAttribute('data-confirm')) {
                        KellyTools.showNotice(lloc('commit_reset_confirmed'));
                        
                        var formData = new FormData();
                            formData.append('reset', '1');
                            formData.append('reset-wifi', '1'); 
                            
                        KellyTools.cfetch(urlReset, {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {

                            KellyTools.showNotice(response ? lloc('commit_reset_ok') : lloc('conn_lost'), response ? false : true); 
                            if (error) console.log(error);
                        }); 
                        
                     } else {
                         
                        this.setAttribute('data-confirm', 1);
                        KellyTools.showNotice(lloc('commit_reset_confirm'));
                     }
                     
                 } else if (key == 'mqttSetDefaultsDomoticz' || key == 'mqttSetDefaultsHA') {
                     
                    if (key == 'mqttSetDefaultsHA') {
                        
                        gid('option-mqttDevicesIds').value = "";
                        gid('option-mqttPrefixIn').value = "";
                        gid('option-mqttHAPrefix').value = "homeassistant";
                        gid('option-mqttHADevice').value = "volna42_einkscreen" + (Math.round(Math.random() * (30000 - 20000) + 20000)) + ", Volna42 E-Ink Screen";
                        
                    } else {
                        
                        gid('option-mqttDevicesIds').value = "-1,-1,-1"; 
                        gid('option-mqttPrefixIn').value = "domoticz/in";
                        gid('option-mqttHAPrefix').value = "";
                        gid('option-mqttHADevice').value = "";
                        
                    }
                    
                    gid('option-mqttDevicesIds').setAttribute('data-updated', 1);
                    gid('option-mqttPrefixIn').setAttribute('data-updated', 1);
                    gid('option-mqttHAPrefix').setAttribute('data-updated', 1);
                    gid('option-mqttHADevice').setAttribute('data-updated', 1);
                        
                 } else if (key == 'reboot') {
                    
                     
                        var formData = new FormData();
                            formData.append('reboot', '1');  
                            
                        KellyTools.cfetch(urlReboot, {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {

                            KellyTools.showNotice(response ? lloc('reboot_ok') : lloc('conn_lost'), response ? false : true); 
                            if (error) console.log(error);
                        }); 
                    
                 } else if (this.getAttribute('data-action')) {
                     
                        var formData = new FormData();
                            formData.append('confirm', '1');  
                            
                        KellyTools.cfetch(this.getAttribute('data-action'), {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {
                            
                            KellyTools.showNotice(response ? lloc('command_ok') : lloc('conn_lost'), response ? false : true); 
                            if (error)console.log(error);
                        }); 
                 }
                    
                 return false;
            };
            
        }
    } 
    
    function initCommitable() {
        
        var input = document.getElementsByClassName(cVarCls);
        for (var i = 0; i < input.length; i++) {
            input[i].onchange = function(e) {
                 this.setAttribute('data-updated', 1);
            };
        }
    } 
    
    function saveForm() {
        
        if (KellyTools.locked()) return false;
        var commitBtn = gid('commit');
        if (commitBtn.classList.contains('loading')) return false;
        
        var cOptions = document.getElementsByClassName(cVarCls);
        var formData = new FormData();
        if (gid('commit-skip').checked) formData.append('commit', 'skip');
        
        for (var i = 0; i < cOptions.length; i++) {
            if (cOptions[i].getAttribute('data-updated') == "1") {
                var key = cOptions[i].getAttribute('data-key');
                formData.append(key, cOptions[i].value); 
                console.log(key + ' :: ' + cOptions[i].value);
            }
        }
       
        if (Array.from(formData.keys()).length <= 0){
            
            KellyTools.showNotice(lloc('commit_nothing')); // нажмите еще раз для принудительной отправки
            
        } else {
            
            commitBtn.classList.add('loading');
            KellyTools.showNotice(lloc('commit_save')); 
            
            KellyTools.cfetch(urlSave, {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {
                
                commitBtn.classList.remove('loading');
                
                if (response) {
                                        
                    KellyTools.showNotice(lloc('commit_save_ok') + ' ' + (response.restart ? '<b>' + lloc('commit_reboot_required') + '</b>' : '')); 
                    
                } else {
                    KellyTools.showNotice(lloc('conn_lost'), true);
                    console.log(error);
                }
            }); 
        }
    }
    
    function showOptions() {
               
        var html = '';
        for (var key in env.cfg) {
             
             var helpHtml = '';
             if (typeof env["loc_" + lng]['cfg_' + key + '_help'] != 'undefined') {
                 helpHtml = '&nbsp;&nbsp;<span href="#" class="k-options-row-help" data-help="cfg_' + key + '_help">(?)</span>';
             }
             
             if (key.indexOf('_/') === 0) {
                 
                 if (key == '_/wifi') {
                     
                     html += '<div class="wList">';
                     html += '<a href="#" id="wList-get">' + lloc('show_wifi_list') + '</a><ul id="wList-items"></ul>';
                     html += '</div>';
                 }
                 
                 html += '</div></div>';
                 
             } else if (key.indexOf('_') === 0) {
                                  
                 html += '<button class="k-options-additions-show" data-for="spoiler-' + key + '">▼ ' + lloc('cfg_' + key) + helpHtml + '</button>';
                 html += '<div class="k-options-additions-wrap" id="spoiler-' + key + '"><div class="k-options-additions">';
                 
                 if (key == '_text') {
                     
                     // todo - add text send form - textarea + send button
                     
                 } else if (key == '_log') {
                     
                     // todo - add log view - textarea
                     
                 }


             } else {
                 
                 if (env.cfgSaved && typeof env.cfgSaved['__EXCLUDE'] != 'undefined' && env.cfgSaved['__EXCLUDE'].indexOf(key) != -1) continue;
                 
                 var value = env.cfgSaved && typeof env.cfgSaved[key] != 'undefined' ? env.cfgSaved[key] : '';
                 var inputType = 'text';
                 
                      if (env.cfg[key].secret) inputType = 'password';
                 else if (hiddenKeys.indexOf(key) != -1) inputType = 'hidden';
                 
                 if (listKeys.indexOf(key) != -1) {   
                     html += '<div class="k-options-row-title">' + lloc('cfg_' + key) + '</div>';
                     html += '<div class="dtl-list-container" id="k-options-' + key + '-Manager"></div>';
                 }
                 
                 if (inputType == 'hidden') {
                     html += '<input type="hidden" data-key="' + key + '" class="' + cVarCls + '" id="option-' + key + '" value="' + value + '">';
                     continue;
                 } 
                   
                 var title = lloc('cfg_' + key) + helpHtml;
                              
                 var htmlCheckbox = '';                 
                 if (env.cfg[key].ftype == "bool") {                     
                     html += '<input type="checkbox" data-key="' + key + '" class="' + cVarCls + '" id="option-' + key + '-enabled" ' + (value ? 'checked' : '') +'> ';
                 } 
                 
                 var htmlHeader = '<div class="k-options-row option-row-' + key +'"><div class="k-options-row-title"><label>' + htmlCheckbox + title + '</label></div>';
                 
                 if (env.cfg[key].ftype == "bool") {
                     
                     html += htmlHeader + '</div>';
                     
                 } else if (env.cfg[key].ftype != 'action') {
                     
                     if (key == 'timestamp') {
                         if (!value) value = new Date().toISOString().slice(0, 19).replace('T', ' ');
                     }
                     
                     html += htmlHeader;
                     html += '<div class="k-options-row-input">';
                     html += '<input class="' + cVarCls + '" type="' + inputType + '" autocomplete="off" ';
                     html += 'data-key="' + key + '" id="option-' + key + '" placeholder="' + (env.cfg[key].placeholder ? env.cfg[key].placeholder : title) + '" value="' + value + '">';
                     html += '</div>' + '</div>';
                            
                 } else {
                     
                     if (env.cfg[key].location) {
                         
                         html += '<div class="k-options-row-title"><a href="' + env.cfg[key].location + '">' + lloc(key) + '</a></div>';
                         
                     } else {
                         
                        html += '<a class="k-options-action" href="#" data-key="' + key + '" data-action="' + (env.cfg[key].action ? env.cfg[key].action : "") + '">' + lloc(key) + '</a>';
                     }
                 }
             }             
        }
        
        html += '<div class="k-options-save"><label class="checkbox"><input type="checkbox" id="commit-skip">' + lloc('save_skip') + '</label><button id="commit">' + lloc('save') + '</button></div>';
                      
        gid('page').innerHTML = html;
        
        if (gid('option-modeList'))
        widgets.push(KellyTools.getListWidget({            
            locPrefix : 'mode_',            
            states : true,
            orderChange : true,
            editable : false,            
            inputId : 'option-modeList',            
            cId : 'k-options-modeList-Manager',
            // defaultInput : '',
            // defaultState : '0',
        }));
        
        /*
        widgets.push(KellyTools.getListWidget({            
            locPrefix : 'mode_',
            editable : true,            
            inputId : 'option-ntpHosts',            
            cId : 'k-options-ntpHosts-Manager',
            defaultInput : '',
        }));
        
        widgets.push(KellyTools.getListWidget({            
            locPrefix : 'mode_', 
            editable : true,            
            inputId : 'option-externalHosts',            
            cId : 'k-options-externalHosts-Manager',
            defaultInput : '',
        }));
        */
        handler.wifiSelect = KellyTools.getSelectWidget({
            tpl : '<div class="lvl lvl#0#"><div></div><div></div><div></div></div><a href="#">#1#</a> (#2# dbm)',
            valueIndex : 1,
            onLoadList : function(self, rawData) {
                
                if (!rawData || !rawData.networks) return [];
                                
                var list = [];
                for (var i = 0; i < rawData.networks.length; i++) {
                    var nt = rawData.networks[i];
                    list.push([3, nt.name, nt.dbm]);
                    // todo - calc network signal lvl by dbm
                }
                
                return list;
            },        
            onUpdate : function(self) {
                if (self.list && self.list.length <= 0) {
                    self.containerEl.innerHTML = 'Сетей не найдено';
                    return;
                }
                KellyTools.updateSpoilerBounds();
            },
            onSelectItem : function(self, item, selected) {
                var wn = gid('option-wifiNetwork');
                
                if (selected) {
                    
                    wn.value = item.getAttribute('data-value');
                    wn.setAttribute('data-updated', '1');
                    
                } else if (env.cfgSaved && typeof env.cfgSaved['wifiNetwork'] != 'undefined') {
                    
                    wn.value = env.cfgSaved['wifiNetwork'];
                    wn.setAttribute('data-updated', '0');
                }
            },
            // list : [[3, 'RetroWave', 'test'], [1, 'test2', 'test'], [2, 'test3', 'test'],[0, 'Hydro', 'test'],],
            cId : 'wList-items',
            getListUrl : urlGetWifi,
            // multiselect : false,
            inputId : 'option-externalHosts',  
        });
        
         handler.nSelect = KellyTools.getSelectWidget({
            tpl : '#1# [#2# ' + lloc('notifications_duration_sec') + ' | #3#] <button href="#" data-index="#0#">X</button><div class="clear"></div>',
            onLoadList : function(self, rawData) {return rawData;},        
            onUpdate : function(self) {
                if (self.list.length <= 0) {
                    self.containerEl.innerHTML = lloc('notifications_empty');
                    return;
                }
                
                var delBtns = self.containerEl.getElementsByTagName('button');
                for (var i = 0; i < delBtns.length; i++) {
                    delBtns[i].onclick = function() {
                        
                        if (KellyTools.locked()) return false;
                        KellyTools.cLock = lloc('notifications_remove_process');
                        
                        var formData = new FormData();
                            formData.append('index', this.getAttribute("data-index"));
                            
                        KellyTools.cfetch('/api/delete/notification', {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {
                            
                            KellyTools.cLock = false;
                            
                            if (error) {
                                console.log(error);
                            } else {                                    
                                console.log(response);
                                handler.nSelect.applyData(response.notifications);                                    
                            }
                        }); 
                        
                        return false;
                    }
                }
            },   
            onSelectItem : function(self, item, selected) {},
            list : [],
            cId : 'notification-info',
        });
        handler.nSelect.applyData = function(rawData) {
            if (typeof rawData == 'undefined') rawData = [];
            handler.nSelect.list = [];
            for(var i = 0; i < rawData.length; i++) {
                handler.nSelect.list.push([rawData[i].id, rawData[i].text, rawData[i].d, rawData[i].t > 0 ? rawData[i].tc + '/' + rawData[i].t : 'постоянно']);
            }
            handler.nSelect.updateList();
        };
        handler.nSelect.show();
        
        for (var i = 0; i < widgets.length; i++) widgets[i].render();
        
        gid('wList-get').onclick = function() {
            if (handler.wifiSelect.containerEl.classList.contains('shown')) {
                handler.wifiSelect.containerEl.classList.remove('shown');
                KellyTools.updateSpoilerBounds();
            } else {
                handler.wifiSelect.show();
            }
        };
        gid('commit').onclick = saveForm;
        
        gid('device-info-refresh').onclick = showDeviceInfo;
        gid('device-info-refresh').innerText = lloc('getinfo_refresh');
        
        KellyTools.initSpoilers();
        initActions();
        initCommitable();
        showDeviceInfo();     
        
    }
    
    function showDeviceInfo() {
        var panel = gid('device-info');
        if (panel.classList.contains('loading')) return false;
        
            panel.classList.add('loading');
            panel.innerHTML = "";       
            
        gid('device-info-container').classList.add('shown');
        
        KellyTools.cfetch(urlGetInfo, {method: 'GET', responseType : 'json'}, function(response, error) {
             
             var html = ""; panel.classList.remove('loading');
             
             if (response) {
                 
                 handler.nSelect.applyData(response.notifications);                 
                 var wifiStatus = parseInt(response.wifiConnected);
                 
                 html += '<div class="device-info-row">';
                 
                     html += '<div class="lvl ' + (wifiStatus === 1 ? 'lvl3' : 'lvl0')  + '"><div></div><div></div><div></div></div>' + response.wifiName;
                     
                     if (wifiStatus === 1) {
                         
                        html += '<div>' + lloc('getinfo_wifi_lvl') + ' : ' + response.wifiDbm + ' Dbm</div>';
                        
                     } else {
                         
                         html += '<div>' + lloc('getinfo_wifi_fail');
                         
                         var wifiStatusText = '';
                         if (wifiStatus === -1) wifiStatusText = lloc('getinfo_wifis1');
                         else if (wifiStatus === -2) wifiStatusText = lloc('getinfo_wifis2');
                         else if (wifiStatus === -3) wifiStatusText = lloc('getinfo_wifis3');
                         
                         if (wifiStatusText) html += ' [' + wifiStatusText + ']';
                         html += '</div>';
                     }
                     
                 html += '</div>';
                 html += '<div class="device-info-row dt-time">' + response.dt + '</div>';
                 html += '<div class="device-info-row">' + lloc('getinfo_sensors') + ' : </div>';
                 
                var sensors = 0; 
                var counter = {};
                var ignored = ['battery_sensor_v', 'battery_sensor_percent', 'ext_last_sync'];
                for (var key in response.sensors) {
                    
                    if (ignored.indexOf(key) != -1) continue;
                    
                    sensors++;
                    var kkey = key.indexOf('__') != -1 ? key.substring(0, key.indexOf('__')) : key;
                    var subkey = key.indexOf('__') != -1 ? key.split('__')[1] : false;
                    
                    if (!counter[kkey]) counter[kkey] = 1;
                    else counter[kkey]++;
                    
                    var cStr = subkey ? ' (' + lloc('getinfo_sensor') + ' ' + subkey + ')' : '';
                    var fstr = '';
                    
                    if (key.indexOf('temperature') != -1) {
                        fstr = lloc('getinfo_sensor_temp') + cStr + ' : <b>' + response.sensors[key] + '</b> &#8451;';
                    } else if (key.indexOf('humidity') != -1) {
                        fstr = lloc('getinfo_sensor_hum') + cStr + ' : <b>' + response.sensors[key] + '</b> %';
                    } else if (key.indexOf('co2') != -1) {
                        fstr = lloc('getinfo_sensor_co2') + cStr + ' : <b>' + response.sensors[key] + '</b> мд (ppm)';
                    } else if (key.indexOf('tvoc') != -1) {
                        fstr = lloc('getinfo_sensor_loc') + cStr + ' : <b>' + response.sensors[key] + '</b> мг/м3';
                    } else if (key == 'mqtt') {
                        fstr = "MQTT" + ' : <b>' + (response.sensors[key] ? lloc('getinfo_connected') : lloc('getinfo_not_connected')) + '</b>';
                    } else if (key == 'battery') {
                        
                        fstr = "<p>" + lloc('getinfo_bat') + ' : <b>' + (response.sensors[key] ? lloc('getinfo_connected') : lloc('getinfo_not_connected')) + '</b></p>';
                        
                        if (response.sensors[key]) {
                            
                            fstr += "<p>" + lloc('getinfo_v') + ' : <b>' + response.sensors['battery_sensor_v'] + 'V</b></p>';
                            fstr += "<p>" + lloc('getinfo_bat_percent') + ' : <b>' + response.sensors['battery_sensor_percent'] + '%</b></p>';
                        }
                        
                    } else if (key == 'ext_connect_times') {
                        
                        fstr = "<p>" + lloc('getinfo_external_sens') + ' : <b>' + (response.sensors[key] > 0 ? lloc('getinfo_connected') : lloc('getinfo_not_connected')) + '</b></p>';
                        
                        if (response.sensors[key] > 0) {
                            fstr += "<p>" + lloc('getinfo_external_sens_sync_times') + ' : <b>' + response.sensors[key] + '</b></p>';
                            fstr += "<p>" + lloc('getinfo_external_sens_sync_last') + ' : <b>' + response.sensors['ext_last_sync'] + '</b></p>';
                        }
                        
                    } else if (key.indexOf('pressure') != -1) {
                        
                        var hpa = response.sensors[key] / 100;
                        var rt = parseFloat(hpa) * 0.750062;
                        
                        fstr = lloc('getinfo_sensor_pres') + ' ' + cStr + ' : <b>' + hpa.toFixed(2) + '</b> hPa <b>' + rt.toFixed(2) + '</b> мм рт.ст.';
                        
                    } else if (key.indexOf('weather_icon') != -1) {
                        
                        fstr = lloc('getinfo_sensor_weather_state') + ' ' + cStr + ' : <b>' + response.sensors[key] + '</b>';
                        
                    } else {
                        fstr = key + cStr +' : ' + response.sensors[key];
                    }
                    
                    html += '<div class="device-info-row">' + fstr + '</div>';
                    
                }
                
                if (sensors <= 0) {
                    html += '<div class="device-info-row">' + lloc('getinfo_sensors_empty')+ '</div>';
                }
                
                panel.innerHTML = html;
                setTimeout(function() { panel.style.minHeight = parseInt(panel.getBoundingClientRect()) + 'px'; });
                 
             } else {
                 console.log(error);
                 panel.innerText = lloc('getinfo_fail');
             }
         });
         
         return false;
    }
    
    this.init = function() {
        lng = KellyTools.detectLanguage();     
        if (env.cfgSaved && env.cfgSaved["__PRODUCT_VERSION"]) {
            KellyTools.version = env.cfgSaved["__PRODUCT_VERSION"];            
        }
        
        KellyTools.showTitle();
        showOptions();
    }
  
}