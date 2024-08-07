/* 
    default system tools
    KellyTools v1.0
*/

function gid(n) {
    return document.getElementById(n);
}

function lloc(n) {
    return typeof ENVDATA["loc_" + ENVDATA.lng][n] == 'undefined' ? n : ENVDATA["loc_" + ENVDATA.lng][n];
}   

KellyTools = { cLock : false, spoilEvents : [], version : false };

KellyTools.locked = function(silent) {
    if (KellyTools.cLock !== false) {
        if (!silent) KellyTools.showNotice(KellyTools.cLock == true ? 'Дождитесь окончания операции...' : KellyTools.cLock); 
        return true;
    } else return false;        
};

   
KellyTools.updateSpoilerBounds = function() {
    var swrap = document.querySelectorAll('.k-options-additions-wrap.show');
    for (var i = 0; i < swrap.length; i++) {
        swrap[i].style.maxHeight = (swrap[i].children[0].getBoundingClientRect().height + 6) + 'px';
    }
};

KellyTools.initSpoilers = function() {
    
    var sEvents = KellyTools.spoilEvents;    
    for (var i = 0; i < sEvents.length; i++) sEvents[i][0].removeEventListener('click', sEvents[i][1]);
    
    KellyTools.spoilEvents = [];
    var spoilers = document.getElementsByClassName('k-options-additions-show');
    var sclick = function(e) {
            
            if (e.target.classList.contains('k-options-row-help')) {
                KellyTools.showNotice(lloc(e.target.getAttribute('data-help')));
                return false;
            }
            
            for (var i = 0; i < spoilers.length; i++) {
                if (spoilers[i] != this) {
                    var t = document.getElementById(spoilers[i].getAttribute('data-for'));
                        t.classList.remove('show');
                        t.style.maxHeight = '';
                }
            }
            
            var additions = document.getElementById(this.getAttribute('data-for'));
            if (additions.classList.contains('show')) {
                
              additions.classList.remove('show');
              additions.style.maxHeight = '';
              
            } else {
                
               additions.classList.add('show'); 
            }
            
            KellyTools.updateSpoilerBounds();
    };
        
    for (var i = 0; i < spoilers.length; i++) {        
        spoilers[i].addEventListener('click', sclick);  
        KellyTools.spoilEvents.push([spoilers[i], sclick]);
    }
};
    
KellyTools.showTitle = function(subpage) {
    document.title = subpage ? lloc('title') + ' - ' + subpage : lloc('title');   
    var html =  '<a href="/" class="kelly-app-name">' + lloc('title') + (KellyTools.version ? ' <span class="kelly-app-version">v' + KellyTools.version : '') + '</span></a>';
        html += '<span class="kelly-copyright">created by <a href="https:/' + '/kelly.catface.ru/" target="_blank">nradiowave</a></span>';
    document.getElementById('header').innerHTML = html;
};
    
KellyTools.downloadFile = function(data, name, mime) {
    
     var link = document.createElement("A");
         link.style.display = 'none';
         link.onclick = function() {
        
            var url = window.URL.createObjectURL(new Blob([data], {type: mime}));
            
            this.href = url;
            this.download = name;
            
            setTimeout(function() {  window.URL.revokeObjectURL(url);  }, 4000); // link.parentElement.removeChild(link); 
        };
        
        link.click();
};
    
KellyTools.showNotice = function(notice, error, onDomUpdated) {
    
    if (!onDomUpdated) onDomUpdated = function() {};
    var result = gid('result'); 
    
    if (!KellyTools.noticeInit) {
        
        KellyTools.noticeInit = true;
        
        document.addEventListener('click', function (e) {
             if (result.classList.contains('show')) {
                if (e.target.id == 'result-close' || !KellyTools.getParentByClass(e.target, 'notice')) KellyTools.showNotice(false);
             }
        });
    }
           
    if (notice) {
        
        if (result.classList.contains('show')) {
            
            KellyTools.showNotice(false);
            setTimeout(function() { KellyTools.showNotice(notice, error); }, 500);
            return;
        }
        
        result.children[1].innerHTML = notice;
        onDomUpdated();
        setTimeout(function() { result.style.bottom = '12px'; result.classList.add('show'); }, 100);      
        
        error ? result.classList.add('error') : result.classList.remove('error'); 
        
    } else {
        
        result.style.bottom = "-" + result.getBoundingClientRect().height + "px";
        result.classList.remove('show');
    }
};

KellyTools.getExt = function(str) {
    
    if (!str) return '';
    var dot = str.lastIndexOf('.');
    if (dot === -1) return '';
    
    var ext =  str.substr(dot).split(".");
    if (ext.length < 2) return '';
    
    ext = ext[1];
    return ext.toLocaleLowerCase().trim();
};

KellyTools.validateInt = function(val) {

    if (typeof val == 'number') return val;
    if (!val) return 0;
    
    val = val.trim();
    val = parseInt(val);
    
    if (isNaN(val) || !val) return 0;
    return val;    
};
    
KellyTools.validateFloat = function(val) {

    if (typeof val == 'number') return val;
    if (!val) return 0.0;
    
    val = val.trim();
    val = val.replace(',', '.');
    val = parseFloat(val);
    
    if (isNaN(val) || !val) return 0.0;
    
    return val;    
};
 
KellyTools.detectLanguage = function() {

    var language = window.navigator.userLanguage || window.navigator.language;
    if (typeof ENVDATA.cfgSaved != 'undefined' && typeof ENVDATA.cfgSaved["__LOCALE"] != 'undefined') {
        language = ENVDATA.cfgSaved["__LOCALE"];
    }
    
    if (language) {
        if (language.indexOf('-') != -1) language = language.split('-')[0];            
        language = language.trim();

        ENVDATA.lng = language;
        
    } else ENVDATA.lng = 'en';
    
    if (typeof ENVDATA["loc_" + ENVDATA.lng] == "undefined") {
        ENVDATA["loc_" + ENVDATA.lng] = ENVLOCALE;        
    }
    
    return ENVDATA.lng;
};

KellyTools.getParentByClass = function(el, className) {
    
    var parent = el; 
    while (parent && !parent.classList.contains(className)) {
        parent = parent.parentElement;
    }
    
    return parent;
};
    
KellyTools.cfetch = function(url, cfg, callback, aCfg) {
    
     var frequest = {controller : new AbortController()};     
     cfg.signal = frequest.controller.signal;
     
     fetch(url, cfg).then(function(response) {
        
        clearTimeout(frequest.timeout);
        
        if (response.status == 200) {
            
                 if (cfg.responseType == 'blob') return response.blob().then(callback);
            else if (cfg.responseType == 'json') return response.json().then(callback);
            else if (cfg.responseType == 'text') return response.text().then(callback);
            else if (cfg.responseType == 'binary' || cfg.responseType == 'arrayBuffer') return response.arrayBuffer().then(callback);

        } else {
            
            callback(false, 'Устройство не доступно ' + response.status);
        }        
        
    }).then(function(text) {})
    .catch(function(error) {
        clearTimeout(frequest.timeout);
        callback(false, error);
    }); 
    
    frequest.abort = function(bytimeout) {
        if (!frequest.controller) return;
        frequest.controller.abort();
        frequest.controller = false;
        
        if (bytimeout) debugger;
    };
    
    frequest.timeout = setTimeout(function() {
        frequest.abort(true);
    }, aCfg && aCfg.timeout ? aCfg.timeout : 10000); 
  
    return frequest;
};

// для таймзон, списка вайфай сетей

KellyTools.getSelectWidget = function(cfg) {
                        
    var wg = {}; 
        wg.create = function(cfg) {
            
            wg.cfg = cfg;
            if (cfg.list) wg.list = cfg.list;
            wg.inputEl = gid(cfg.inputId);  
            wg.selected = [];
            wg.valueIndex = wg.cfg.valueIndex ? wg.cfg.valueIndex : 0;
            wg.containerEl = gid(cfg.cId);
        };
        
        wg.commit = function() {
            
            var result = '';
            for (var i = 0; i < wg.selected.length; i++) {
                result += (result ? ',' : '') + wg.list[i][wg.valueIndex];
                if (!wg.cfg.multiselect) break;
            }
            if (wg.inputEl) {
                wg.inputEl.setAttribute('data-updated', '1');
                wg.inputEl.value = result;
            }
        };
        
        wg.updateList = function() {
            
            wg.containerEl.innerHTML = '';
            
            if (wg.lastError) {
                
                wg.containerEl.innerText = 'Не удалось загрузить список : ' + wg.lastError;
                
            } else if (!wg.list) {
                
                wg.containerEl.innerText = 'Ошибка обработки данных списка';
                
            } else if (wg.list.length > 0) {
                
                var html = '';
                for (var i = 0; i < wg.list.length; i++) {
                    
                    var htmlTitle = wg.cfg.tpl;
                    for (var b = 0; b < wg.list[i].length; b++) {
                        htmlTitle = htmlTitle.replace('#' + b + '#', wg.list[i][b]);
                    }
                    
                    htmlTitle = htmlTitle.replace('#index#', i);
                    
                    html += '<li class="dselect-item' + (wg.selected.indexOf(i) != -1 ? ' selected' : '') + '" data-index="' + i + '" data-value="' + wg.list[i][wg.valueIndex] +'">';
                    html += htmlTitle + '</li>';
                }
                
                wg.containerEl.innerHTML = html;
                var items = wg.containerEl.getElementsByClassName('dselect-item');
                for (var i = 0; i < items.length; i++) {                        
                    items[i].onclick = function() {
                        
                        var index = parseInt(this.getAttribute('data-index'));
                        if (wg.selected.indexOf(index) == -1) {
                            if (wg.cfg.multiselect) wg.selected.push(index);
                            else wg.selected = [index];
                        } else {
                            if (wg.cfg.multiselect) wg.selected.splice(index, 1);
                            else wg.selected = [];
                        }
                        
                        wg.commit();
                        if (wg.cfg.onSelectItem && wg.cfg.onSelectItem(wg, this, wg.selected.indexOf(index) == -1 ? false : true)) return false;
                                                
                        wg.updateList();
                    };
                }
                
            }
            
            if (wg.cfg.onUpdate) wg.cfg.onUpdate(wg);
        };
        
        wg.show = function(onLoaded) {
            
            if (typeof onLoaded == 'undefined') onLoaded = function() {};
            
            wg.containerEl.classList.add('shown');
            if (wg.containerEl.classList.contains('loading')) return;
            if (!wg.list) {
                 
                 wg.containerEl.classList.add('loading');
                 wg.lastError = false;
                 
                 if (wg.cfg.onUpdate) wg.cfg.onUpdate(wg);
                 
                 KellyTools.cfetch(wg.cfg.getListUrl, {method: 'GET', responseType : 'json'}, function(response, error) {
                     wg.containerEl.classList.remove('loading');
                     if (response) {
                         
                         if (wg.cfg.onLoadList) response = wg.cfg.onLoadList(wg, response);
                         
                         wg.list = response;
                         wg.updateList();
                         onLoaded(true);
                        
                     } else {
                         
                         wg.list = [];
                         wg.lastError = error;
                         console.log('List error : ' + error);
                         wg.updateList();
                         onLoaded(false);
                     }
                     
                 });
            } else {
                onLoaded(true);
                wg.updateList();
            }
            
            return false;
        };
        
        wg.create(cfg);
        return wg;
};

// доп. запросы, список нтп серверов, порядок режимов работы

KellyTools.getListWidget = function(cfg) {
    
    var wg = {}; 
        wg.create = function(cfg) {
            
            wg.cfg = cfg;
            
            wg.inputEl = gid(cfg.inputId);
            wg.itemsArr = wg.inputEl.value.trim().length <= 0 ? [] : wg.inputEl.value.split(',');
            
            wg.containerEl = gid(cfg.cId);
            if (cfg.editable) {
                wg.containerEl.classList.add('dtl-list-editable');
            }
            
            
            wg.stateEl = false;
            wg.stateArr = false;
            
            if (cfg.states) {
                wg.stateEl = gid(cfg.inputId + 'Enabled');
                wg.stateArr = wg.itemsArr.length <= 0 ? [] : wg.stateEl.value.split('');
            }
                
        };
        
        wg.getIndex = function(el) {
            return parseInt(KellyTools.getParentByClass(el, 'data-list-item').getAttribute('data-index'));
        };
        
        wg.commit = function() {
            wg.inputEl.setAttribute('data-updated', '1');
            wg.inputEl.value = wg.itemsArr.join(',');
            if (wg.stateEl) {
                wg.stateEl.setAttribute('data-updated', '1');
                wg.stateEl.value = wg.stateArr.join('');
            }
        };
        
        wg.render = function() {                
                  
            var html = '';          
            for (var i = 0; i < wg.itemsArr.length; i++) {
                      
                var k = cfg.cId + '-' + i;
                html += '<div class="data-list-item" data-index="' + i + '" data-input="' + cfg.inputId + '">';            

                    html += '<label for="dtl-enabled-' + k + '">';
                    
                    if (cfg.states) {
                        html += '<input type="checkbox" class="dtl-enabled" id="dtl-enabled-' + k +'" ' + (parseInt(wg.stateArr[i]) > 0 ? 'checked' : '') + '>';
                    }
                    
                    html += '<div class="dtl-name">';
                    
                    if (cfg.editable) {                        
                    
                        html += '<div class="k-options-row-input"><input class="dtl-input" type="text" value="' + wg.itemsArr[i] + '"></div>';
                        
                    } else {  
                        html += '<span class="dtl-name">' + lloc(cfg.locPrefix + wg.itemsArr[i]) + '</span>';
                    }
                    
                    html += '</div>';      

                    if (cfg.editable) {
                        
                       html += '<button class="dtl-delete">x</button>';
                                
                    }
                    
                   if (cfg.orderChange) {
                        
                       html += '<div class="dtl-actions">';
                        html += '<button class="dtl-priority dtl-priority-up">▲</button>';
                        html += '<button class="dtl-priority dtl-priority-down">▼</button>';
                       html += '</div>';
                                
                   }
                   
                   html += '</label>';

               html += '</div>';   
                
            }
            
            if (cfg.editable) {
                 html += '<div class="k-options-row-input dtl-add-row">';
                 html += '<button class="dtl-add">+</button></div>';
            }
            
            wg.containerEl.innerHTML = html;
                 
            var mInputs = wg.containerEl.getElementsByClassName('dtl-priority');
            for (var i = mInputs.length-1; i >= 0; i--) {
                mInputs[i].onclick = function() { 
                    
                    var up = this.classList.contains('dtl-priority-up');
                    var index = wg.getIndex(this);
                    var value = wg.itemsArr[index];
                    
                    if (index == -1 || (up && index == 0)) {
                        return false;
                    }
                    
                    if (!up && index == wg.itemsArr.length - 1) {
                        return false;
                    }
                    
                    if (!up && index == wg.itemsArr.length - 1) {
                        return false;
                    }
                    
                    var switchIndex = up ? index - 1 : index + 1;
                    var switchValue = wg.itemsArr[switchIndex]; 
                    
                    wg.itemsArr[index] = switchValue;
                    wg.itemsArr[switchIndex] = value;
                    
                    if (wg.stateArr) {
                        var switchState = wg.stateArr[switchIndex]; 
                        var state = wg.stateArr[index];
                        
                        wg.stateArr[index] = switchState;
                        wg.stateArr[switchIndex] = state;
                    }
                    
                    var itemEl = wg.containerEl.querySelector('[data-index="' + index + '"]');
                    // var itemIndexEl = itemEl.getElementsByClassName('dtl-index');
                    //     itemIndexEl[0].innerText = switchIndex+1;
                        
                    var switchItemEl = wg.containerEl.querySelector('[data-index="' + switchIndex + '"]');
                    // var switchItemIndexEl = switchItemEl.getElementsByClassName('dtl-index');
                    //     switchItemIndexEl[0].innerText = index+1;
                    
                    if (up) itemEl.parentNode.insertBefore(itemEl, switchItemEl);
                    else itemEl.parentNode.insertBefore(switchItemEl, itemEl);
                    
                    itemEl.setAttribute('data-index', switchIndex);
                    switchItemEl.setAttribute('data-index', index);
                    itemEl.classList.add('fade');
                    switchItemEl.classList.add('fade');
                    setTimeout(function() { itemEl.classList.remove('fade'); switchItemEl.classList.remove('fade'); }, 200);
                    
                    wg.commit();
                    return false;
                };
            };
            
            var mInputs = wg.containerEl.getElementsByClassName('dtl-enabled');
            for (var i = mInputs.length-1; i >= 0; i--) {
                mInputs[i].onchange = function() {
                    wg.stateArr[wg.getIndex(this)] = this.checked ? '1' : '0';
                    wg.commit();
                }
            }
            
            if (cfg.editable) {
                wg.containerEl.getElementsByClassName('dtl-add')[0].onclick = function() {
                    
                    wg.itemsArr.push(wg.cfg.defaultInput);
                    
                    if (wg.stateEl) {
                        wg.stateArr.push(defaultState);
                    }
                    
                    wg.commit();
                    wg.render();                       
                }; 
                
                mInputs = wg.containerEl.getElementsByClassName('dtl-input');
                for (var i = mInputs.length-1; i >= 0; i--) {
                    mInputs[i].onchange = function() {
                        wg.itemsArr[wg.getIndex(this)] = this.value;
                        wg.commit();
                    };
                }
                
                mInputs = wg.containerEl.getElementsByClassName('dtl-delete');
                for (var i = mInputs.length-1; i >= 0; i--) {
                    mInputs[i].onclick = function() {
                        var index = wg.getIndex(this);
                        wg.itemsArr.splice(index, 1);
                        
                        if (wg.stateEl) {
                            wg.stateArr.splice(index, 1);
                        }
                        
                        wg.commit();
                        wg.render(); 
                    };
                }
            }
        };
        
        wg.create(cfg);
        return wg;
};