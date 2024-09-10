// список с перемещением элементов вверх \ вниз - доп. запросы, список нтп серверов, порядок режимов работы

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