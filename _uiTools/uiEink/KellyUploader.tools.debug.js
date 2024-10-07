function KellyImgUplDebug() {
    
    // used for tests with color displays, paletes like GB used
    var palete = [    
        [[123, 255, 255], [0, 0, 0], [90, 89, 181], [49, 48, 99]],
        [[176, 192, 160], [40, 56, 24], [136, 152, 120], [96, 112, 80]],
        [[224, 248, 207], [7, 24, 33],  [48, 104, 80], [134, 192, 108]],
    ];
    
    // WHITE, BLACK, GRAY1 (Darker), GRAY2 (Lighter)
    
    var defaultPal = false;
    var uploader = false;
    var handler = this;
    var debugTools = document.createElement('DIV');
    
    handler.initPalSettings = function() {
        
        var html = "";
        var i = 0;
        for (var key in ENV.rPalete) {
            html += "<div>";
            
            var rgb = ENV.rPalete[key][0] + ',' + ENV.rPalete[key][1] + ',' + ENV.rPalete[key][2];
            html += '<div id="debug-screen-palet-' + key + '-col" style="width : 24px; height : 24px;  vertical-align: middle; background-color : rgb(' + rgb + '); display : inline-block; margin-right : 4px;"></div>';
            html += '<input type="text" id="debug-screen-palet-' + key + '" palceholder="screenPalet-' + key + '" value="' + rgb + '">';
            html += "</div>";
            
            i++;
        }
        gid('palet-settigs').innerHTML = html;
        
        gid('debug-confirm-palete').onclick = function() {
            
            var newData = {};
            for (var key in ENV.rPalete) {
               
                var paleteData =  document.getElementById('debug-screen-palet-' + key).value.split(',');
                var rgb = [];
                for (var i = 0; i < 3; i++) {
                    if (i > paleteData.length-1) {
                        rgb[i] = 0;
                    } else {
                        rgb[i] = parseInt(paleteData[i]);
                        if (isNaN(rgb[i]) || rgb[i] < 0) rgb[i] = 0;
                    }
                }
                
                var rgbStr = rgb[0] + ',' + rgb[1] + ',' + rgb[2];
                gid('debug-screen-palet-' + key + '-col').style.backgroundColor = 'rgb(' + rgbStr + ')';
                newData[key] = rgb;
            }
            
            ENV.rPalete = newData;
            gid('img-up-fit-mode').onchange();
        }        
    }
    
    handler.applyPal = function(src) {  
        
        var newData = {};
        newData['WHITE'] = src[0];
        newData['BLACK'] = src[1];
        newData['GRAY1'] = src[2];
        newData['GRAY2'] = src[3];
        ENV.rPalete = newData;
    }
    
    handler.clonePal = function(src, key) {
        
        return [src[key][0], src[key][1], src[key][2],];
    }
    
    handler.bufferToText1PixelPerByte = function() {
         
        // 0x7FFF, 0x5AD6, 0x318C, 0x0000
        // white, darker, more darker, darkes
        var palete = {WHITE : 0, GRAY1 : 1, GRAY2 : 2, BLACK : 3};
        var colorsBits = {WHITE : [1, 1], BLACK : [0, 0], GRAY1: [1, 0], GRAY2: [0, 1] };
        
        var sbController = window.ENV.getSB();
        var bufferData = [];
        var sb = window.ENV.getSB();
        var byteN = 0, bitCursor = 0;
        
        for (var bufferY = 0; bufferY < sb.screenHeight; bufferY++) {
              
              for (var bufferX = 0; bufferX < sb.screenWidth; bufferX++) {                    
                                             
                    if (sbController.bufferBitPerPixel == 2) {
                        
                        var bitData = [sbController.getBit(sbController.buffer[byteN], bitCursor), sbController.getBit(sbController.buffer[byteN], bitCursor+1)];
                        var color = palete.BLACK;
                        
                             if (colorsBits.WHITE[0] == bitData[0] && colorsBits.WHITE[1] == bitData[1]) color = palete.WHITE;
                        else if (colorsBits.GRAY1[0] == bitData[0] && colorsBits.GRAY1[1] == bitData[1]) color = palete.GRAY1;
                        else if (colorsBits.GRAY2[0] == bitData[0] && colorsBits.GRAY2[1] == bitData[1]) color = palete.GRAY2;
                        
                    } else {
                       
                        var pixel = !sbController.getBit(sbController.buffer[byteN], bitCursor) ? true : false; // 0 - black, 1 - white
                        var color = palete.WHITE;
                        if (pixel) {
                            color = palete.BLACK;                           
                        }
                    }
                    
                    bufferData.push(color);
                                        
                    bitCursor+=sbController.bufferBitPerPixel;                    
                    if (bitCursor > 7) {
                        byteN++;
                        bitCursor = 0;                            
                    }                   
              }
        }
        
        handler.bufferToText(bufferData);
    }
    
    handler.bufferToText = function(bufferData) {
         
        var bits = '';  
        var byteData = '';
        var sb = window.ENV.getSB();
        if (!bufferData) bufferData = sb.buffer;
        
        var t = 1;
        if (bufferData.length <= 0) return;
        for(var y = 0; y < bufferData.length; y++) {

             byteData += '0x' + bufferData[y].toString(16).toUpperCase() + ', ';   
             t += 1;
             
             if (t > 20) {
                byteData += "\r\n";
                t = 1;
             } else t++;
             
        }
               
        KellyTools.downloadFile(byteData, 'screen_buffer' + sb.screenWidth +'x' + sb.screenHeight + '_' + '.txt', "text/plain");
        return byteData;
    }
    
    handler.init = function() {
        
            debugTools.id = "debug-tools";
            
        // document.getElementsByClassName('kelly-app-name')[0].innerHTML = "Debug mode";
        
        var main = document.getElementById('page');
        if (main && typeof window.ENV != 'undefined') {
            main.parentNode.insertBefore(debugTools, main.nextSibling);
            
        } else {
            console.log('not on uploader page');
            return;
        }
        
        defaultPal = [handler.clonePal(ENV.rPalete, 'WHITE'), handler.clonePal(ENV.rPalete, 'BLACK'), handler.clonePal(ENV.rPalete, 'GRAY1'), handler.clonePal(ENV.rPalete, 'GRAY2')];
        palete.push(defaultPal);
        
        uploader = window.ENV;
        
        var html = "<style>#debug-tools {max-width: 400px; padding: 16px; padding-left: 50px;} #debug-tools input { height : 32px; }</style>";
            html += "<p>Параметры экрана</p>";
            html += '<div>Ширина экрана : <br><input type="text" id="debug-screen-width" value="' + ENVDATA.screen.width + '" placeholder="screenWidth"></div>';
            html += '<div>Высота экрана : <br><input type="text"  id="debug-screen-height" value="' + ENVDATA.screen.height + '" palceholder="screenHeight"></div>';
             
            html += '<button id="debug-confirm-screen">Применить изменения</button>';
            
            html += "<p>Цветовая палитра для превью</p>";
            
            html += '<div id="palet-settigs">';
            html += '</div>';
            
            html += "<p>Пресет палитры</p>";
            for (var i = 0; i < palete.length; i++) {
                html += '<div id="palete-' + i + '" data-palete="' + i + '" style="cursor : pointer;">';
                
                
                for (var b = 0; b < palete[i].length; b++) {
                    
                    var rgbColor = palete[i][b][0] + ',' + palete[i][b][1] + ',' + palete[i][b][2];
                    html += '<div style="width : 16px; height : 16px;  vertical-align: middle; background-color : rgb(' + rgbColor + '); display : inline-block;"></div>';
                }
                    
                html += "</div>";
            }
            
            html += '<button id="debug-confirm-palete">Применить изменения</button>';
            
        debugTools.innerHTML = html;
        for (var i = 0; i < palete.length; i++) {
            gid('palete-' + i).onclick = function() {
                handler.applyPal(palete[parseInt(this.getAttribute('data-palete'))]);
                handler.initPalSettings();
                gid('img-up-fit-mode').onchange();
            }
        }  
         
        handler.initPalSettings();
        
        document.getElementById('debug-confirm-screen').onclick = function() {
            
            var sWidth =  parseInt(document.getElementById('debug-screen-width').value);
            if (!isNaN(sWidth) && sWidth > 0) ENVDATA.screen.width = sWidth;
            var sHeight = parseInt(document.getElementById('debug-screen-height').value);
            if (!isNaN(sHeight) && sHeight > 0) ENVDATA.screen.height = sHeight;
            
            ENV.initScreen();
            ENV.getSB().bufferBitPerPixel = 0; // reset cache buffer
            ENV.getSB().init(ENVDATA.screen.width, ENVDATA.screen.height);
            gid('img-up-fit-mode').onchange();              
        }
        
        var downloadTxt = document.createElement('BUTTON');
            downloadTxt.innerText = 'Скачать текстовый массив (txt)';
            downloadTxt.onclick = handler.bufferToText;
                    
        var downloadTxt2 = document.createElement('BUTTON');
            downloadTxt2.innerText = 'Скачать 1 Pixel per Byte (txt)';
            downloadTxt2.onclick = handler.bufferToText1PixelPerByte;
            
        var downloadsBlock = document.getElementById('disp-cui-download');
            downloadsBlock.children[0].appendChild(downloadTxt);
            downloadsBlock.children[0].appendChild(downloadTxt2);
        
    }
    
}