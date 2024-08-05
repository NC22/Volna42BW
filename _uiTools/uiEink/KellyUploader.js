function KellyImgUpl(env) {
    
    var handler = this;
    
    var env = env;
    var lng;
    
    var screen = env.screen;
    
    var PARAMS_MAX_SIZE = 64;
    
    // uuid - unique widget id in format stored in device memory
    // mast be the same as in EnvConfigOptions.h for specified widget
    
    var widgets = [
        {uuid : 1, id : "uiClock", width : 102, height : 80, x : 20, y : 20}, 
        {uuid : 2, id : "uiInfo", width : 200, height : 40, x : 10, y : 220}, 
        {uuid : 3, id : "uiTemp", width : 152, height : 80, x : 10, y : 200},
        {uuid : 4, id : "uiHum", width : 152, height : 80, x : 10, y : 200},
        {uuid : 5, id : "uiTempRemote", width : 152, height : 80, x : 10, y : 200},
        {uuid : 6, id : "uiHumRemote", width : 152, height : 80, x : 10, y : 200},
        {uuid : 7, id : "uiBatRemote", width : 100, height : 20, x : 20, y : 90}, 
        {uuid : 8, id : "uiBat", width : 100, height : 20, x : 20, y : 100},
        {uuid : 9, id : "uiInfoIP", width : 80, height : 20, x : 20, y : 110},
        {uuid : 10, id : "uiInfoVoltage", width : 60, height : 20, x : 20, y : 120},
        {uuid : 11, id : "uiLastSyncRemote", width : 60, height : 20, x : 20, y : 130},
        {uuid : 12, id : "uiInfoSyncNumRemote", width : 60, height : 20, x : 20, y : 140},
        {uuid : 13, id : "uiInfoMessage", width : 100, height : 20, x : 20, y : 150},
        {uuid : 14, id : "uiShortInfoSyncRemote", width : 100, height : 20, x : 20, y : 160},
        
    ];
    
    var widgetsOrder  = [
        'uiClock',
        'uiBat',
        'uiBatRemote',
        'uiInfo',
        'uiTemp',
        'uiHum',
        'uiTempRemote',
        'uiHumRemote',
        
        'uiInfoIP',
        'uiInfoVoltage',
        'uiLastSyncRemote',
        'uiInfoSyncNumRemote',
        'uiShortInfoSyncRemote',
        // 'uiInfoMessage', // custom sign - text parametrs editing not implemented now
    ];
    
    handler.getWidgetByKey = function(key, keyName) {
        if (typeof keyName == 'undefined' || !keyName) keyName = 'id';
        for (var i = 0; i < widgets.length; i++) {
            if (widgets[i][keyName] == key) return widgets[i];
        }
        
        return false;
    };
    
    var colors = {WHITE : [231, 237, 239], BLACK : [13, 15, 16], GRAY1: [ 128, 128, 128 ], GRAY2: [ 192, 192, 192 ] };
    var colorsBits = {WHITE : [1, 1], BLACK : [0, 0], GRAY1: [1, 0], GRAY2: [0, 1] };

    var selectScrollControllerEvent = function(e) {
        handler.scrollTarget = this;
    };
    
    var delayUpdateCurrent = function() {
       if (handler.lastSettings) {
           if (handler.delayUpdater) clearTimeout( handler.delayUpdater);
           handler.delayUpdater = setTimeout(handler.initPreviewFromInput, 200);
       }
    };
    
    handler.preloadedJSON = false;
    handler.defaultFileName = "";
    
    handler.sendPackResetWork = function() {
        
        handler.sendPackStart = false;
        KellyTools.cLock = false;
        
        if (handler.sendPackRequest) {
            handler.sendPackRequest.abort();
            handler.sendPackRequest = false;
        }         
    };   
        
        
    var sbController = {};
    sbController.init = function(w, h) {
        
        if (sbController.bufferBitPerPixel && sbController.bufferBitPerPixel == screen.bitPerPixel) {
            console.log('skip init - same buffer size');
        } else {
            sbController.screenWidth = w;
            sbController.screenHeight = h;
            sbController.bufferLen = (sbController.screenWidth * sbController.screenHeight / 8) * screen.bitPerPixel;
            sbController.buffer = new Uint8Array( sbController.bufferLen );
            if (screen.bufferMod) sbController.bufferMod = new Uint8Array( sbController.bufferLen ); 
            else sbController.bufferMod = false;
            sbController.bufferBitPerPixel = screen.bitPerPixel;
            console.log('Init screen buffer [' + sbController.screenWidth + 'x' + sbController.screenHeight + '] - Bytes : ' + sbController.bufferLen);
        }
    };
    
    sbController.getBit = function(byte, position) {
    
        return (byte >> (7 - position)) & 0x1;
    };

    sbController.setBit = function(number, position, state){
    
        position = (7 - position);
        
        if (!state) {
            var mask = ~(1 << position);
            return number & mask;
        } else {
            return number | (1 << position) ;
        }        
    };
    
    sbController.reset = function(bgBlack) {
        for (var i = 0; i < sbController.bufferLen; i++) {
            // if (screen.bitPerPixel == 2) {
            //     sbController.buffer[i] = bgBlack ? 255 : 0;
            //} else {
                 sbController.buffer[i] = bgBlack ? 0 : 255;
                 if (sbController.bufferMod !== false) sbController.bufferMod[i] = sbController.buffer[i];
            //}            
        }
    };   

    // probably may be solved by another way, need to test more 1.5 inch disp
    // screen.bufferMod - optionaly buffer if we need to send buffer in specified order to properly display and it differs from default display order in web interface
    
    sbController.setScreenBufferPixel = function(x, y, val, mod) {
        
        if (mod && screen.bufferMod == "flipx") { 
            x = Math.abs(x-sbController.screenWidth);
        } else if (mod && screen.bufferMod == "flipy") { 
            y = Math.abs(y-sbController.screenHeight);
        } else if (mod && screen.bufferMod == "flipxy") {
            x = Math.abs(x-sbController.screenWidth);
            y = Math.abs(y-sbController.screenHeight);
        }

        var bitPos = (y * sbController.screenWidth) + x;  
            bitPos = screen.bitPerPixel * bitPos;
        
        var bufferCursorByte = 0;
        var bufferCursorBit = 0;
        
        if (bitPos % 8) {
            
            bufferCursorByte = Math.floor(bitPos / 8);
            bufferCursorBit = bitPos - (bufferCursorByte * 8);
    
        } else {
    
            bufferCursorBit = 0;
            bufferCursorByte = bitPos / 8;
        }
        
        // console.log(bitPos);
        // console.log('edit ' + bufferCursorByte + ' - ' + bufferCursorBit);

        if (bufferCursorByte >= sbController.buffer.length) {

            console.log('Out of bounds ' + x + ' - ' + y);
        } else {
            
            var target = mod ? "bufferMod" : "buffer";
            
            if (screen.bitPerPixel == 2) {
                var byteMod = sbController[target][bufferCursorByte];
                    byteMod = sbController.setBit(byteMod, bufferCursorBit, val[0]);
                    byteMod = sbController.setBit(byteMod, bufferCursorBit+1, val[1]);
                    
                sbController[target][bufferCursorByte] = byteMod;
            } else {
                sbController[target][bufferCursorByte] = sbController.setBit(sbController[target][bufferCursorByte], bufferCursorBit, val ? 0 : 1);
            }
        }
        
        if (!mod && screen.bufferMod !== false) {
            sbController.setScreenBufferPixel(x, y, val, true);
        }
    };
    
    sbController.init(screen.width, screen.height);

    // algo taken from - https://github.com/ticky project - https://github.com/ticky/canvas-dither
    
    function greyscaleLuminance(image) {

        for (var i = 0; i <= image.data.length; i += 4) {

            image.data[i] = image.data[i + 1] = image.data[i + 2] = parseInt(image.data[i] * 0.21 + image.data[i + 1] * 0.71 + image.data[i + 2] * 0.07, 10);

        }

        return image;
    }
    
    function findNearestColor(pixel, asKey) {
        var minDistance = Infinity;
        var nearestColor;
        var nearestColorKey;
        
        for (var key in colors) {
            var color = colors[key];
            var distance = Math.pow(color[0] - pixel[0], 2) + Math.pow(color[1] - pixel[1], 2) + Math.pow(color[2] - pixel[2], 2);
            if (distance < minDistance) {
                minDistance = distance;
                nearestColor = color;
                nearestColorKey = key;
            }
        }
        return asKey ? nearestColorKey : nearestColor;
    }
    
    
    function ditherAtkinson(image, imageWidth, drawColour, coefficient, devide) {
        
        if (!coefficient) coefficient = 0.125;
        
        skipPixels = 4;
        if (!drawColour)
            drawColour = false;

        if(drawColour == true)
            skipPixels = 1;

        imageLength    = image.data.length;

        for (currentPixel = 0; currentPixel <= imageLength; currentPixel += skipPixels) {

            if (image.data[currentPixel] <= 128) {
                newPixelColour = 0;
            } else {
                newPixelColour = 255;
            }

            err = parseInt((image.data[currentPixel] - newPixelColour)  * coefficient, 10);
            image.data[currentPixel] = newPixelColour;
            
            if (devide) {
                
                image.data[currentPixel + 4]                      += err * 1 / 8;
                image.data[currentPixel + 8]                      += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth) - 4]   += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth)]       += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth) + 4]   += err * 1 / 8;
                image.data[currentPixel + (8 * imageWidth)]       += err * 1 / 8;
                
            } else {
                    
                image.data[currentPixel + 4]                        += err;
                image.data[currentPixel + 8]                        += err;
                image.data[currentPixel + (4 * imageWidth) - 4]        += err;
                image.data[currentPixel + (4 * imageWidth)]            += err;
                image.data[currentPixel + (4 * imageWidth) + 4]        += err;
                image.data[currentPixel + (8 * imageWidth)]            += err;
            }
            
            if (drawColour == false)
                image.data[currentPixel + 1] = image.data[currentPixel + 2] = image.data[currentPixel];

        }

        return image.data;
    }
    
    
    function ditherAtkinsonColor4(image, imageWidth, drawColour, coefficient ) {
        
        if (!coefficient) coefficient = 0.125;
        
        skipPixels = 4;
        if (!drawColour)
            drawColour = false;

        if(drawColour == true)
            skipPixels = 1;

        imageLength = image.data.length;

        for (currentPixel = 0; currentPixel <= imageLength; currentPixel += skipPixels) {
            
            var oldColor = image.data[currentPixel];
                var newColor;
                if (oldColor <= 128) {
                    newColor = 0;
                } else {
                    newColor = 255; 
                }

                err = parseInt((oldColor - newColor)  * coefficient, 10);
                image.data[currentPixel] = newColor;

                image.data[currentPixel + 4]                      += err * 1 / 8;
                image.data[currentPixel + 8]                      += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth) - 4]   += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth)]       += err * 1 / 8;
                image.data[currentPixel + (4 * imageWidth) + 4]   += err * 1 / 8;
                image.data[currentPixel + (8 * imageWidth)]       += err * 1 / 8;

                if (drawColour == false) {
                    image.data[currentPixel + 1] = oldColor;
                    image.data[currentPixel + 2] = oldColor;
            }
        }
    }

    
    function rgbToHsv(r, g, b) {
        
        if (r && g === undefined && b === undefined) {
            g = r.g, b = r.b, r = r.r;
        }

        r = r / 255, g = g / 255, b = b / 255;
        var max = Math.max(r, g, b), min = Math.min(r, g, b);
        var h, s, v = max;

        var d = max - min;
        s = max == 0 ? 0 : d / max;

        if (max == min) {
            h = 0; // achromatic
        } else {
            switch (max) {
                case r:
                    h = (g - b) / d + (g < b ? 6 : 0);
                    break;
                case g:
                    h = (b - r) / d + 2;
                    break;
                case b:
                    h = (r - g) / d + 4;
                    break;
            }
            h /= 6;
        }

        return {h: h, s: s, v: v};
    }
        
    function fitToScreen(canvas, image, resizeBy, cover) {
        
        var ctx = canvas.getContext('2d'); 
        var auto = false;
        
        if (!resizeBy) {
            
            auto = true;
            
                 if (image.width >= screen.localWidth) resizeBy = 'width';
            else if (image.height >= screen.localHeight) resizeBy = 'height';
            
        } else if (resizeBy == 'width' && image.width <= screen.localWidth) {
            resizeBy = false;
        } else if (resizeBy == 'height' && image.height <= screen.localHeight) {
            resizeBy = false;
        }
        
        var fit = function(dt, by) {
        
            if (by == 'width') {                
                var dW = screen.localWidth;
                var k = dt.width / screen.localWidth;
                var dH = Math.ceil(dt.height / k);                
            } else if (by == 'height') {            
                var dH = screen.localHeight;
                var k = dt.height / screen.localHeight;
                var dW = Math.ceil(dt.width / k);                
            }
            
            return {width : dW, height : dH};
        };
        
        if (resizeBy == 'width') {    
            
            var fitData = fit(image, 'width');
            if (!cover && fitData.height > screen.localHeight) fitData = fit(fitData, 'height');
            
        } else if (resizeBy == 'height') {
        
            var fitData = fit(image, 'height');
            if (!cover && fitData.width > screen.localWidth) fitData = fit(fitData, 'width');
            
        } else {
        
            canvas.width = image.width;                
            canvas.height = image.height;
            ctx.drawImage(image, 0, 0);
            
            return;
        }
        
        canvas.width = fitData.width;                
        canvas.height = fitData.height;
        
        ctx.drawImage(image, 0, 0, fitData.width, fitData.height);
    }
    
    function getCanvasCursor(x, y, contextScreen) {
        return (((y - 1) * (contextScreen ? contextScreen.width : screen.width)) + x - 1) * 4;
    }
    
    /*
        Draw widget & update widget presetted settings
    */
    
    function drawPlaceholder(widget) {
        
        widget.enabled = false;
        if (widgetsOrder.indexOf(widget.id) == -1) return;
        if (!gid('widget-' + widget.id +'-enabled').checked) return;
        
        var canvas = document.createElement('canvas');
        var ctx = canvas.getContext('2d');

        var width = widget.width, x = KellyTools.validateInt(gid('widget-' + widget.id +'-x').value), y = KellyTools.validateInt(gid('widget-' + widget.id +'-y').value);
        var height = widget.height;
        var cornerRadius = 8;
        
        widget.x = x;
        widget.y = y;
        widget.enabled = true;
        widget.params = "";
        
        var params = gid('widget-' + widget.id +'-params');
        if (params) widget.params = params;
        
        ctx.rect(0, 0, screen.width, screen.height);
        ctx.fillStyle = 'rgba(0,0,0,0)';
        ctx.fill();
        ctx.clearRect(0, 0, width, height); 
        ctx.beginPath();
        ctx.moveTo(cornerRadius, 0);
        ctx.arcTo(width, 0, width, height, cornerRadius);
        ctx.arcTo(width, height, 0, height, cornerRadius);
        ctx.arcTo(0, height, 0, 0, cornerRadius);
        ctx.arcTo(0, 0, width, 0, cornerRadius);
        ctx.closePath();
        ctx.fillStyle = 'rgba(31,31,31,0.7)';
        ctx.fill();
        
        
        ctx.font = '16px ' + "'Open Sans',Sans-serif";
 
        ctx.strokeStyle = '#fff';
        ctx.lineWidth = 1;
        ctx.strokeText('<' + lloc('widget_' + widget.id) + '>', 6, height+16);
               ctx.fillStyle = 'black';
        ctx.fillText('<' + lloc('widget_' + widget.id) + '>', 6, height+16);
        
        var mctx = gid('img-up-screen').getContext('2d');
        
        if (screen.landscape) {
            mctx.drawImage(canvas, x, y);
        } else {
            var angleInRadians = Math.PI / 2;
            mctx.rotate(angleInRadians);
            mctx.drawImage(canvas, x, y-screen.localHeight);
            mctx.rotate(-angleInRadians);
        }
    }
    
    function renderPreview() {
        
        var screenEl = gid('img-up-screen');
            screenEl.width = screen.width;
            screenEl.height = screen.height;
        
        var screenCtx = screenEl.getContext('2d');
        
            screenCtx.rect(0, 0, screen.width, screen.height);
            screenCtx.fillStyle = 'rgb(' + colors.WHITE[0] + ',' + colors.WHITE[1] + ',' + colors.WHITE[2] + ')';
            screenCtx.fill();
            
        var bitCursor = 0, byteN = 0;   
        var pixelData = screenCtx.getImageData(0, 0, screen.width, screen.height);
        var renderMod = false;
        if (screen.bufferMod && sbController.bufferMod === false) {
            renderMod = true;
            sbController.bufferMod = new Uint8Array( sbController.bufferLen ); 
        }
        
        for (var bufferY = 0; bufferY < screen.height; bufferY++) {
              
              for (var bufferX = 0; bufferX < screen.width; bufferX++) {                    
                        
                    var g = getCanvasCursor(bufferX+1, bufferY+1, screen);                            
                    if (screen.bitPerPixel == 2) {
                        
                        var bitData = [sbController.getBit(sbController.buffer[byteN], bitCursor), sbController.getBit(sbController.buffer[byteN], bitCursor+1)];
                        var color = colors.BLACK;
                        
                             if (colorsBits.WHITE[0] == bitData[0] && colorsBits.WHITE[1] == bitData[1]) color = colors.WHITE;
                        else if (colorsBits.GRAY1[0] == bitData[0] && colorsBits.GRAY1[1] == bitData[1]) color = colors.GRAY1;
                        else if (colorsBits.GRAY2[0] == bitData[0] && colorsBits.GRAY2[1] == bitData[1]) color = colors.GRAY2;
                        
                        pixelData.data[g + 0] = color[0];
                        pixelData.data[g + 1] = color[1];
                        pixelData.data[g + 2] = color[2];  
                        if (renderMod) sbController.setScreenBufferPixel(x, y, bitData, true);
                        
                    } else {
                       
                        var pixel = !sbController.getBit(sbController.buffer[byteN], bitCursor) ? true : false; // 0 - black, 1 - white
                        if (pixel) {
                            pixelData.data[g + 0] = colors.BLACK[0];
                            pixelData.data[g + 1] = colors.BLACK[1];
                            pixelData.data[g + 2] = colors.BLACK[2];                             
                        }
                        
                        if (renderMod) sbController.setScreenBufferPixel(x, y, pixel, true);
                    }
                                        
                    bitCursor+=screen.bitPerPixel;                    
                    if (bitCursor > 7) {
                        byteN++;
                        bitCursor = 0;                            
                    }                   
              }
        }
        
        screenCtx.clearRect(0, 0, screen.width, screen.height);
        screenCtx.putImageData(pixelData, 0, 0 );  
        
        for (var i =0; i < widgets.length; i++) drawPlaceholder(widgets[i]);
        //drawClockPlaceholder();
    }
        
    function renderImageElToBuffer(imageEl, settings) {

        var canvas = document.createElement('canvas');
        var ctx = canvas.getContext('2d');  

        if (settings.fit == 'fitImage' || settings.fit == 'fitCover' || settings.fit == 'width' || settings.fit == 'height') {
            
            var fitMode = settings.fit == 'width' || settings.fit == 'height' ? settings.fit : false;
            var fitCover = fitMode !== false || settings.fit == 'fitCover' ? true : false;
            fitToScreen(canvas, imageEl, fitMode, fitCover);
            
        } else {
            
            if (settings.fit == 'fitScreen') {
                updateCustomMonitorCfg(imageEl.width, imageEl.height);
                screenInit(imageEl.width, imageEl.height);                
            }
            
            canvas.width = imageEl.width;                
            canvas.height = imageEl.height;
            ctx.drawImage(imageEl, 0, 0);
            
        }
        
        var pixelsImgEl = ctx.getImageData(0, 0, canvas.width, canvas.height);        
        
        sbController.init(screen.width, screen.height);
        sbController.reset(settings.invertBg == 'black' ? true : false);
        
        if (settings.mode == 'atkinson') {
        
            settings.threshhold = 0.5;
            if (settings.gs) greyscaleLuminance(pixelsImgEl);
            
            if (screen.bitPerPixel == 1) {
                if (settings.atkinsonAuto) ditherAtkinson(pixelsImgEl, canvas.width, settings.atkinsonDrawp); // less noisy in some cases
                else ditherAtkinson(pixelsImgEl, canvas.width, settings.atkinsonDrawp, settings.atkinsonCoef, true);      
            } else {
                ditherAtkinsonColor4(pixelsImgEl, canvas.width, settings.atkinsonDrawp, settings.atkinsonCoef);       
            }            
        }
        
        for(var y = 1; y <= canvas.height; y++) {
            
            for(var x = 1; x <= canvas.width; x++) {
                 
                var bitData = true;
                var i = getCanvasCursor(x, y, canvas);
                
                if (screen.bitPerPixel == 2) {
                    
                    var pixel = pixelsImgEl.data.slice(i, i + 4);
                    if (pixelsImgEl.data[i + 3] <= 128) { // alpha opacity
                        pixel[0] = 255;
                        pixel[1] = 255;
                        pixel[2] = 255;
                    }
                    
                    bitData = colorsBits[findNearestColor(pixel, true)];
                    
                } else {

                    if (pixelsImgEl.data[i + 3] <= 128) { // alpha opacity
                   
                        if (settings.invertImg == 'white') {
                            bitData = false;
                        }
                        
                    } else {
                        
                        var hsv = rgbToHsv(pixelsImgEl.data[i + 0], pixelsImgEl.data[i + 1], pixelsImgEl.data[i + 2]);
                       
                        if (settings.invertImg == 'black' && hsv.v < settings.threshhold) {
                            bitData = false;
                        }
                        
                        if (settings.invertImg == 'white' && hsv.v > settings.threshhold) {
                            bitData = false;
                        }
                   }
               }
                
               var rX = settings.offsetX + x;
               var rY = settings.offsetY + y;
               
               if (screen.landscape == false) {
                    var tmp = rX;
                    rX = screen.width - rY;
                    rY = tmp;
                          
                  // if (screen.flip) {
                   //     rX = Math.abs(rX-screen.width);
                  // }
               } else {
                     
                  // if (screen.flip) {
                  //     rY = Math.abs(rY-screen.height);
                   //}
               }
                              
               if (rX >= 1 && rX <= screen.width && rY >= 1 && rY <= screen.height) {                  
                   sbController.setScreenBufferPixel(rX-1, rY-1, bitData);
               }
            }    
        }        
    }
        
    function initWidgets() {
        var html = '<div><label><input type="checkbox" id="widgets-hidden-params-show"> Показывать скрытые параметры</label></div>';
        for (var i = 0; i < widgetsOrder.length; i++) {
            var widget = handler.getWidgetByKey(widgetsOrder[i]);
            
            html+= '<div class="widget">\
                    X <input id="widget-' + widget.id + '-x" type="text" class="offset-controll" value="' + widget.x + '" data-step="1">\
                    Y <input id="widget-' + widget.id + '-y" type="text" class="offset-controll" value="' + widget.y +'" data-step="1">\
                    <label><input type="checkbox" class="widget-switch" id="widget-' + widget.id + '-enabled">' + lloc('widget_' + widget.id) + '</label>\
                    <div class="widget-hidden-params"><input type="text" class="widget-params" placeholder="Params of ' + widget.id + '" id="widget-' + widget.id + '-params"></div>\
                    </div>';
        }
        
        gid('widgets').innerHTML = html;
        gid('widgets-hidden-params-show').onclick = function() {
            if (this.checked) {
                gid('widgets').classList.add('widgets-show-hidden');
                KellyTools.updateSpoilerBounds();  
            } else {
                gid('widgets').classList.remove('widgets-show-hidden');
                KellyTools.updateSpoilerBounds();  
            }
        };
        
        var els = document.getElementsByClassName('offset-controll');
        for (var i = 0; i < els.length; i++) {
            
            els[i].onclick = function() {
                handler.scrollTarget = this;
                delayUpdateCurrent();
            };

            els[i].onchange = delayUpdateCurrent;    
        }
        
        els = document.getElementsByClassName('widget-switch');
        for (var i = 0; i < els.length; i++) {
            els[i].onclick = delayUpdateCurrent;        
        }
    }
    
    function initUploadedFile(settings, onReady) {
        if (settings.mode == 'json-preloaded') {
            onReady(handler.preloadedJSON);
            return;
        } else if (settings.files.length <= 0) {  
            onReady(false);
            return;
        }
        
        var reader = new FileReader();
            reader.addEventListener("loadend", function() {
                    
                    if (settings.mode == 'json') {
                       
                        try {
                            var cfg = JSON.parse(reader.result.trim());
                            onReady(cfg);
                        } catch (e) {
                            console.log(e);
                            onReady(false);
                        }
                    
                   } else if (settings.mode == 'rawbin') {
                       
                       onReady(new Uint8Array(reader.result));
                       
                   } else if (settings.mode == 'rawtxt') {
                       
                       onReady(reader.result);
                       
                   } else {
                       
                      var src = new Image();
                          src.onload = function() {
                             onReady(this);
                          };
                          
                          src.onerror = function() {
                              onReady(false);
                          };
                          
                          src.src = reader.result;
                   }
            });
            
                   if (settings.mode == 'json' || settings.mode == 'rawtxt') {
              reader.readAsText(settings.files[0]);
            } else if (settings.mode == 'rawbin') {
                reader.readAsArrayBuffer(settings.files[0]);
            } else {
                reader.readAsDataURL(settings.files[0]);
            }
    };
    
    function updateDispInfo() {
        
        var screenEl = gid('img-up-screen');
        var changeOption = function(e) {
             if (this.id == 'disp-orient') {
                 screen.landscape = !screen.landscape;
                 delayUpdateCurrent();
             } else if (this.id == 'disp-bits') {
                 screen.bitPerPixel = screen.bitPerPixel == 1 && screen.color4 ? 2 : 1;
                 sbController.init(screen.width, screen.height);
                 delayUpdateCurrent();
             } else {
                 screen.flip = !screen.flip;
             }
            updateDispInfo();
        };
        
        gid('disp-flip').onclick = changeOption;
        gid('disp-orient').onclick = changeOption;
        gid('disp-bits').onclick = changeOption;
        
        screenEl.classList.remove('portrait-mode');
        screenEl.classList.remove('portrait-mode-flip');
        screenEl.classList.remove('landscape-mode-flip');
        screenEl.classList.remove('landscape-mode');
        
        if (!screen.landscape) {            
            screen.localWidth = screen.height;
            screen.localHeight = screen.width;
            screenEl.classList.add('portrait-mode');            
            if (screen.flip)  screenEl.classList.add('portrait-mode-flip');            
        } else {            
            screen.localWidth = screen.width;
            screen.localHeight = screen.height;            
            screenEl.classList.add('landscape-mode');
            if (screen.flip) screenEl.classList.add('landscape-mode-flip');
        }
        
        var t = " [" + screen.localWidth +"x" + screen.localHeight + "]";
        gid('disp-orient').innerText = screen.landscape ? 'Landscape ' + t : 'Portrait' + t;
        gid('disp-flip').innerText = screen.flip ? 'Flip screen' : 'No Flip screen';
        gid('disp-bits').innerText = screen.bitPerPixel == 1 ? '1-bit per pixel' : '2-bit per pixel';
        
        gid('bit-1-bit').style.display = 'none';
        gid('bit-2-bit').style.display = 'none';        
        gid('bit-' + screen.bitPerPixel + '-bit').style.display = '';   
        
        KellyTools.updateSpoilerBounds();    
    };
    
    handler.getUploadSettings = function() {
        
       var file = gid('img-up-image');
       var settings = {
            mode : document.querySelector('input[name=img-up-method]:checked').value, 
            atkinsonCoef : KellyTools.validateFloat(gid('img-up-matkinson-coefficient').value),
            atkinsonDrawp : gid('img-up-matkinson-drawp').checked,
            atkinsonAuto : gid('img-up-matkinson-auto').checked,
            gs : gid('img-up-gs').checked,
            threshhold : KellyTools.validateFloat(gid('img-up-threshhold-value').value),
            invertImg : gid('img-up-invert-color').checked ? 'black' : 'white', 
            invertBg : gid('img-up-invert-bg-color').checked ? 'black' : 'white', 
            fit : gid('img-up-fit-mode').value,
            filename : file.files.length <= 0 ? false : file.files[0].name,
            ext : file.files.length <= 0 ? '' : KellyTools.getExt(file.files[0].name),
            files : file.files,
            offsetX : KellyTools.validateInt(gid('img-up-offset-x').value),
            offsetY : KellyTools.validateInt(gid('img-up-offset-y').value),
            uploadReady : true,
       };
       
       if (handler.preloadedJSON) settings.mode = 'json-preloaded';
       if (settings.ext == 'json') settings.mode = 'json';
       if (settings.ext == 'bin') settings.mode = 'rawbin';
       if (settings.ext == 'txt') settings.mode = 'rawtxt';
       
       if (settings.mode != 'json-preloaded' && settings.files.length <= 0) settings.uploadReady = false;
       
       return settings;
    };
    
    handler.uploadCurrent = function(onRequestsReady, additionalSettings) {
        
        if (typeof additionalSettings == 'undefined' || !additionalSettings) additionalSettings = { displayAfterUpload : true}; 
        
        if (typeof onRequestsReady == 'undefined') onRequestsReady = function() {};
        
        if (handler.sendPackRequest) {
            handler.sendPackResetWork();
            KellyTools.showNotice(lloc('cui_cancel'), true);     
            return false;
        }
        
        var sendImage = function(attemp, offset) {
            
            if (!offset) offset = 0;
            var oldOffset = offset;
            
            var target = screen.bufferMod ? "bufferMod" : "buffer";
            var data = sbController[target];
            if (screen.bitPerPixel > 1) {
               
                data = new Uint8Array(sbController[target].length / screen.bitPerPixel);
                for(var i = 0; i < data.length; i++) {
                    data[i] = sbController[target][offset + i];
                }
                offset += data.length;
            }
            
            var blob = new Blob([data], {type : 'plain/text'});
            var fileOfBlob = new File([blob], 'image.bin');

            var packData = new FormData();
                packData.append("image", fileOfBlob);
            
            handler.sendPackRequest = KellyTools.cfetch("/api/direct/image", {method : 'POST', body : packData, responseType : 'json'}, function(response, error) {
                
                if (!handler.sendPackStart) return;
                
                if (!response || response.status == "fail") {
                    
                    if (!attemp) {
                        attemp = 1;
                    } else attemp++;
                    
                    if (attemp < 3) {
                        setTimeout(function() {sendImage(attemp, oldOffset)}, 1000);
                        
                    } else {
                        KellyTools.showNotice(lloc('cui_image_data_send_fail') + (response && response.err ? response.err : error), true); 
                        onRequestsReady(false);
                        handler.sendPackResetWork();
                    }
                    
                    return;
                } else {
                    if (screen.bitPerPixel > 1 && offset < sbController.buffer.length) {
                        console.log("send " + offset + 'vs' + sbController.buffer.length);
                        handler.sendPackRequest = {
                            timer : setTimeout(function() {sendImage(0, offset)}, 1000),
                            abort : function() {clearTimeout(handler.sendPackRequest.timer);},                
                        };
                        KellyTools.showNotice(offset + "/" + sbController.buffer.length + " " + lloc('cui_bytes_sended')); 
                    } else {
                        onRequestsReady(true, response);
                        handler.sendPackResetWork();
                        
                        if (response.reboot_required) {
                            
                            var formData = new FormData();
                                formData.append('reboot', '1');  
                                
                            KellyTools.cfetch("/api/reboot", {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {}); 
                            KellyTools.showNotice(lloc('cui_restart_required'));
                            
                        } else {
                            KellyTools.showNotice(lloc('cui_done'));   
                        }                        
                    }                    
                }
            }, {timeout : 50 * 1000}); 
        };
        
        var sendWidgets = function(onContinue) {
            
            if (!onContinue) onContinue = function() {};
            
            var widgetsData = new FormData();
                widgetsData.append("commit", '1');
                widgetsData.append("screen-landscape", screen.landscape ? '1' : '0');
                widgetsData.append("screen-flip", screen.flip ? '1' : '0');
                widgetsData.append("screen-bits", screen.bitPerPixel);
                
                widgetsData.append("display-after-upload", !additionalSettings.displayAfterUpload ? '0' : '1');
            
            if (handler.defaultFileName) {
                widgetsData.append("filename", handler.defaultFileName);
            }
                
            for (var i=0; i < widgets.length; i++) {
                if (widgetsOrder.indexOf(widgets[i].id) == -1) continue;
                if (!gid('widget-' + widgets[i].id + '-enabled').checked) continue;
                widgetsData.append("widget-enabled-" + widgets[i].uuid, '1');
                widgetsData.append("widget-x-" + widgets[i].uuid, gid('widget-' + widgets[i].id + '-x').value);
                widgetsData.append("widget-y-" + widgets[i].uuid, gid('widget-' + widgets[i].id + '-y').value);
                
                var params = gid('widget-' + widgets[i].id + '-params');
                if (params) {
                    widgetsData.append("widget-params-" + widgets[i].uuid, params.value.substr(0, PARAMS_MAX_SIZE));
                }
            }
            
            handler.sendPackRequest = KellyTools.cfetch("/api/direct/widgets", {method : 'POST', body : widgetsData, responseType : 'json'}, function(response, error) {
                if (!handler.sendPackStart) return;
                
                if (!response) {
                    KellyTools.showNotice(lloc('cui_device_is_not_responding'), true); 
                    onRequestsReady(false);
                    handler.sendPackResetWork();
                } else if (response.status != "enabled"){
                    onRequestsReady(false);
                    handler.sendPackResetWork();
                    KellyTools.showNotice(lloc('cui_accept_widgets_fail'), true); 
                } else {
                    onContinue();
                }
            });
        };
                  
        handler.sendPackResetWork();
        
        KellyTools.cLock = true;   
        handler.sendPackStart = true;
        
        KellyTools.showNotice(lloc('cui_process') + "..."); 
        
        sendWidgets(function() {
            handler.sendPackRequest = {
                timer : setTimeout(sendImage, 500),
                abort : function() {clearTimeout(handler.sendPackRequest.timer);},                
            };
        });
        
        return true;
    };
    
    this.getSB = function() {return sbController;};
    
    this.initPreviewFromInput = function(onReady, initiator){
                
        if (!onReady) onReady = function() {};
        
        if (KellyTools.locked(true)) {
            onReady(false);
            return;
        }
        
        var settings = handler.getUploadSettings();
        handler.lastSettings = settings;
        
        if (!settings.uploadReady) {
             KellyTools.showNotice(lloc('cui_file_not_selected'));
             onReady(false, settings);
             return;
        }
        
        KellyTools.cLock = true;
        initUploadedFile(settings, function(result) {
            
            KellyTools.cLock = false;
            
            if (settings.mode == 'json' || settings.mode == 'json-preloaded') {
                
                if (!result || !result.simpleBuffer) {
                    
                    KellyTools.showNotice(lloc('cui_parse_fail_json'));
                    onReady(false, settings);
                    
                } else {
                    
                    if (!handler.widgetsUpdatedByCfg) {
                        
                        if (result.screen) {
                            
                            if (!screen.color4 && result.screen.bitPerPixel == 2) {
                                KellyTools.showNotice(lloc('cui_parse_fail_2bit'));
                                onReady(false, settings);
                                return;
                            } 
                            
                            if (screen.width != result.screen.width || screen.height != result.screen.height) {
                                KellyTools.showNotice(lloc('cui_parse_fail_proportions') + ' ' + result.screen.width + 'x' + result.screen.height);
                                onReady(false, settings);
                                return;
                            }
                            
                            screen.landscape = result.screen.landscape;
                            screen.flip = result.screen.flip;
                            if (!result.screen.bitPerPixel) result.screen.bitPerPixel = 1;
                            screen.bitPerPixel = result.screen.bitPerPixel;
                            
                            updateDispInfo();
                        }
                        
                        sbController.init(screen.width, screen.height);
                        screen.bufferMod = false;
                        
                        for (var i = 0; i < sbController.buffer.length; i++) sbController.buffer[i] = result.simpleBuffer[i];
                        for (var i = 0; i < widgets.length; i++) {
                            
                            var key = -1;
                            for (var b = 0; b < result.widgets.length; b++) {
                                if (result.widgets[b].id == widgets[i].id) key = b;
                            }
                            
                            if (key == -1) continue;
                            if (widgetsOrder.indexOf(widgets[i].id) == -1) continue;
                            
                            gid('widget-' + widgets[i].id + '-enabled').checked = result.widgets[key].enabled;
                            gid('widget-' + widgets[i].id + '-x').value = result.widgets[key].x;
                            gid('widget-' + widgets[i].id + '-y').value = result.widgets[key].y;
                            
                            var params = gid('widget-' + widgets[i].id + '-params');
                            if (params) params.value = result.widgets[key].params ? result.widgets[key].params.substr(0, PARAMS_MAX_SIZE) : '';
                        }
                        
                        handler.widgetsUpdatedByCfg = true;
                    }
                    
                    renderPreview();
                    onReady(true, settings);
                }
                
            } else if (settings.mode == 'rawbin'|| settings.mode == 'rawtxt') {
                
                if (settings.mode == 'rawtxt') {
                    
                     var buffer1 = new Uint8Array( sbController.bufferLen );;
                     var txtData = result.split(',');
                     
                     for (var i = 0; i < sbController.buffer.length; i++) {
                    
                        if (i > txtData.length-1) {
                        
                            buffer1[i] = 0;
                            
                        } else {
                        
                            buffer1[i] = txtData[i].trim();
                            var byteData = parseInt(txtData[i]);
                            
                            if (isNaN(byteData)) byteData = 0;
                            if (byteData > 255) byteData = 255; // max 0xFF
                            
                            buffer1[i] = byteData;
                        }
                    }
                    
                    result = buffer1;
                }
                
                if (!result || result.length != sbController.buffer.length) {
                    
                    KellyTools.showNotice(lloc('cui_parse_fail_buffer_size') + ' [' + result.length + lloc('cui_byte') + ' \ ' + sbController.buffer.length + lloc('cui_byte') + ']');
                    onReady(false, settings);
                    
                } else {
                    
                    sbController.buffer = result;
                    renderPreview();
                    onReady(true, settings);
                }
                
            } else {
                
                if (!result) {
                    
                    KellyTools.showNotice(lloc('cui_parse_fail_image_read_error'));
                    onReady(false, settings);
                    
                } else {
                    
                    renderImageElToBuffer(result, settings);
                    renderPreview();
                    onReady(true, settings);
                }
            }
        });
    };
        
    this.init = function() {
        
        lng = KellyTools.detectLanguage();     
        if (env.cfgSaved && env.cfgSaved["__PRODUCT_VERSION"]) {
            KellyTools.version = env.cfgSaved["__PRODUCT_VERSION"];            
        }
        
        KellyTools.showTitle();
        
        if (lng != 'ru') {
            var translatable = document.querySelectorAll('[data-loc]');
            for (var i = 0; i < translatable.length; i++) {
                translatable[i].innerHTML = lloc(translatable[i].getAttribute('data-loc'));
            }
        }
        
        gid('device-info-container').classList.add('shown');
        gid('img-up-preview').style.display = '';
                
        var screenEl = gid('img-up-screen');
            screenEl.width = screen.width;
            screenEl.height = screen.height;
                 
        updateDispInfo();
        
        var imgForm = gid('img-up');        
        gid('page').appendChild(imgForm);
        
        imgForm.style.display = '';
        
        gid('device-info-container').innerHTML = '';
        gid('device-info-container').appendChild(gid('img-up-preview'));
                
        gid('img-up-image').onchange = function() {
            handler.preloadedJSON = false;
            gid('img-up-offset-x').value = 0;
            gid('img-up-offset-y').value = 0;
            handler.widgetsUpdatedByCfg = false;
        };
        
        gid('img-up-offset-x').onclick = selectScrollControllerEvent;
        gid('img-up-offset-y').onclick = selectScrollControllerEvent;
        gid('img-up-threshhold-value').onclick = selectScrollControllerEvent;    
        gid('img-up-matkinson-coefficient').onclick = delayUpdateCurrent;
        gid('img-up-gs').onclick = delayUpdateCurrent;
        gid('img-up-matkinson-auto').onclick = delayUpdateCurrent;
        gid('img-up-matkinson-drawp').onclick = delayUpdateCurrent;
                 
        gid('img-up-fit-mode').onchange = delayUpdateCurrent;
        gid('img-up-offset-x').onchange = delayUpdateCurrent;
        gid('img-up-offset-y').onchange = delayUpdateCurrent;
        gid('img-up-invert-color').onchange = delayUpdateCurrent;
        gid('img-up-invert-bg-color').onchange = delayUpdateCurrent;
        gid('img-up-matkinson').onclick = delayUpdateCurrent;
        gid('img-up-mthreshhold').onclick = delayUpdateCurrent;
            
        handler.lastScrollTop = window.pageYOffset || document.documentElement.scrollTop;

        document.addEventListener("wheel", function(e){
            
           if (KellyTools.locked(true) || !handler.scrollTarget) return;
           
           var st = window.pageYOffset || document.documentElement.scrollTop; 
           var isFloat = handler.scrollTarget.getAttribute('data-step').indexOf('.') !== -1;
           var step = isFloat ?  KellyTools.validateFloat(handler.scrollTarget.getAttribute('data-step')) : KellyTools.validateInt(handler.scrollTarget.getAttribute('data-step'));
           var value = isFloat ? KellyTools.validateFloat(handler.scrollTarget.value) : KellyTools.validateInt(handler.scrollTarget.value);
           
           if (event.deltaY < 0) {
               value+=step;
           } else {
              value-=step;
           } 
           
           handler.scrollTarget.value = value;
           lastScrollTop = st <= 0 ? 0 : st; 
           delayUpdateCurrent();
           
        }, false);
        
        gid('img-up-save').onclick = function() {
            KellyTools.locked();
            handler.initPreviewFromInput(function(result, settings){                
                if (!result) return;                
                KellyTools.downloadFile(sbController.buffer, (settings.filename ? settings.filename : 'default') + '_' + screen.width +'x' + screen.height + '_' + 'bw' + '.bin', "application/octet-stream");
            });
        };
        
       gid('img-up-save-all').onclick = function() {
            KellyTools.locked();
            handler.initPreviewFromInput(function(result, settings){
                 if (!result) return;
                 var array = [];
                 for (var i = 0; i < sbController.buffer.length; i++) array[i] = sbController.buffer[i];
                 KellyTools.downloadFile(JSON.stringify({widgets : widgets, simpleBuffer : array, screen : screen}), (settings.filename ? settings.filename : 'default') + '_' + screen.width +'x' + screen.height + '_' + 'bw' + '.json', "application/json");
            });
        };
          
        gid('img-up-show-preview').onclick = function() {
            KellyTools.locked();
            handler.initPreviewFromInput(false, 'preview');
        };
        
        gid('img-up-upload').onclick = function() {
                        
            if (!handler.getUploadSettings().uploadReady) {
                 KellyTools.showNotice(lloc('cui_upload_nothing'));
                 return;
            }
             
            handler.uploadCurrent();
        };
        
        gid('img-up-reset').onclick = function() {
                    
            if (handler.sendPackRequest) {
                handler.sendPackResetWork();
                KellyTools.showNotice(lloc('cui_cancel'), true);     
                return;
            }   
            
            var widgetsData = new FormData();
                widgetsData.append("commit", '0');
                
            handler.sendPackRequest = KellyTools.cfetch("/api/direct/widgets", {method : 'POST', body : widgetsData, responseType : 'json'}, function(response, error) {

                if (response.status == "disabled") {
                    KellyTools.showNotice(lloc('cui_done'));                     
                } else KellyTools.showNotice("Error reset sreen", true);
                
                handler.sendPackResetWork();
            });
        };
        
        initWidgets();
           
    }
  
}