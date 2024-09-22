function KellyImgUpl(env) {  var handler = this;  var env = env; var lng;  var screen = env.screen;  var PARAMS_MAX_SIZE = 64;   var widgets = [ {uuid : 1, id : "uiClock", width : 102, height : 80, x : 20, y : 20},  {uuid : 2, id : "uiInfo", width : 200, height : 40, x : 10, y : 220},  {uuid : 3, id : "uiTemp", width : 152, height : 80, x : 10, y : 200, mwidth : 60, mheight : 20,}, {uuid : 4, id : "uiHum", width : 152, height : 80, x : 10, y : 200, mwidth : 60, mheight : 20,}, {uuid : 5, id : "uiTempRemote", width : 152, height : 80, x : 10, y : 200, mwidth : 60, mheight : 20,}, {uuid : 6, id : "uiHumRemote", width : 152, height : 80, x : 10, y : 200, mwidth : 60, mheight : 20,}, {uuid : 7, id : "uiBatRemote", width : 100, height : 20, x : 20, y : 90},  {uuid : 8, id : "uiBat", width : 100, height : 20, x : 20, y : 100}, {uuid : 9, id : "uiInfoIP", width : 80, height : 20, x : 20, y : 110}, {uuid : 10, id : "uiInfoVoltage", width : 60, height : 20, x : 20, y : 120}, {uuid : 11, id : "uiLastSyncRemote", width : 60, height : 20, x : 20, y : 130}, {uuid : 12, id : "uiInfoSyncNumRemote", width : 60, height : 20, x : 20, y : 140}, {uuid : 13, id : "uiInfoMessage", width : 100, height : 20, x : 20, y : 150}, {uuid : 14, id : "uiShortInfoSyncRemote", width : 100, height : 20, x : 20, y : 160}, {uuid : 15, id : "uiPressure", width : 100, height : 20, x : 20, y : 160}, {uuid : 16, id : "uiDate", width : 100, height : 20, x : 20, y : 160},  {uuid : 17, id : "uiPressureRemote", width : 100, height : 20, x : 20, y : 160},  {uuid : 18, id : "uiSCD4XCO2", width : 100, height : 20, x : 20, y : 160}, {uuid : 19, id : "uiSCD4XTemp", width : 100, height : 20, x : 20, y : 160},  {uuid : 20, id : "uiSCD4XHum", width : 100, height : 20, x : 20, y : 160},  ];  var widgetsOrder = [ 'uiClock', 'uiBat', 'uiBatRemote', 'uiInfo', 'uiTemp', 'uiHum', 'uiTempRemote', 'uiHumRemote',  'uiInfoIP', 'uiInfoVoltage', 'uiLastSyncRemote', 'uiInfoSyncNumRemote', 'uiShortInfoSyncRemote',  'uiPressure', 'uiDate', 'uiPressureRemote', 'uiSCD4XCO2', 'uiSCD4XTemp', 'uiSCD4XHum', ];  handler.getWidgetByKey = function(key, keyName) { if (typeof keyName == 'undefined' || !keyName) keyName = 'id'; for (var i = 0; i < widgets.length; i++) { if (widgets[i][keyName] == key) return widgets[i]; }  return false; };  var colors = {WHITE : [231, 237, 239], BLACK : [13, 15, 16], GRAY1: [ 128, 128, 128 ], GRAY2: [ 192, 192, 192 ] }; var colorsBits = {WHITE : [1, 1], BLACK : [0, 0], GRAY1: [1, 0], GRAY2: [0, 1] };  handler.rPalete = colors;  var delayUpdateCurrent = function() { if (handler.lastSettings) { if (handler.delayUpdater) clearTimeout( handler.delayUpdater); handler.delayUpdater = setTimeout(handler.initPreviewFromInput, 200); } };  handler.preloadedJSON = false; handler.defaultFileName = "";  handler.sendPackResetWork = function() {  handler.sendPackStart = false; KellyTools.cLock = false;  if (handler.sendPackRequest) { handler.sendPackRequest.abort(); handler.sendPackRequest = false; }  };    var sbController = getFBController(screen);  function fitToScreen(canvas, image, resizeBy, cover) {  var ctx = canvas.getContext('2d');  var auto = false;  if (!resizeBy) {  auto = true;  if (image.width >= screen.localWidth) resizeBy = 'width'; else if (image.height >= screen.localHeight) resizeBy = 'height';  } else if (resizeBy == 'width' && image.width <= screen.localWidth) { resizeBy = false; } else if (resizeBy == 'height' && image.height <= screen.localHeight) { resizeBy = false; }  var fit = function(dt, by) {  if (by == 'width') {  var dW = screen.localWidth; var k = dt.width / screen.localWidth; var dH = Math.ceil(dt.height / k);  } else if (by == 'height') {  var dH = screen.localHeight; var k = dt.height / screen.localHeight; var dW = Math.ceil(dt.width / k);  }  return {width : dW, height : dH}; };  if (resizeBy == 'width') {   var fitData = fit(image, 'width'); if (!cover && fitData.height > screen.localHeight) fitData = fit(fitData, 'height');  } else if (resizeBy == 'height') {  var fitData = fit(image, 'height'); if (!cover && fitData.width > screen.localWidth) fitData = fit(fitData, 'width');  } else {  canvas.width = image.width;  canvas.height = image.height; ctx.drawImage(image, 0, 0);  return; }  canvas.width = fitData.width;  canvas.height = fitData.height;  ctx.drawImage(image, 0, 0, fitData.width, fitData.height); }  function getCanvasCursor(x, y, contextScreen) { return (((y - 1) * (contextScreen ? contextScreen.width : screen.width)) + x - 1) * 4; }    function drawPlaceholder(widget) {  widget.enabled = false; if (widgetsOrder.indexOf(widget.id) == -1) return; if (!gid('widget-' + widget.id +'-enabled').checked) return;  var canvas = document.createElement('canvas'); var ctx = canvas.getContext('2d'); var width = widget.width, x = KellyTools.validateInt(gid('widget-' + widget.id +'-x').value), y = KellyTools.validateInt(gid('widget-' + widget.id +'-y').value); var height = widget.height; var cornerRadius = 8;  widget.x = x; widget.y = y; widget.enabled = true; widget.params = "";  var params = gid('widget-' + widget.id +'-params'); if (params) widget.params = params.value;  if (widget.params && widget.params.indexOf('-m') != -1 && widget.mwidth) { width = widget.mwidth; height = widget.mheight; }  ctx.rect(0, 0, screen.width, screen.height); ctx.fillStyle = 'rgba(0,0,0,0)'; ctx.fill(); ctx.clearRect(0, 0, width, height);  ctx.beginPath(); ctx.moveTo(cornerRadius, 0); ctx.arcTo(width, 0, width, height, cornerRadius); ctx.arcTo(width, height, 0, height, cornerRadius); ctx.arcTo(0, height, 0, 0, cornerRadius); ctx.arcTo(0, 0, width, 0, cornerRadius); ctx.closePath(); ctx.fillStyle = 'rgba(31,31,31,0.7)'; ctx.fill();   ctx.font = '16px ' + "'Open Sans',Sans-serif";  ctx.strokeStyle = '#fff'; ctx.lineWidth = 1; ctx.strokeText('<' + lloc('widget_' + widget.id) + '>', 6, height+16); ctx.fillStyle = 'black'; ctx.fillText('<' + lloc('widget_' + widget.id) + '>', 6, height+16);  var mctx = gid('img-up-screen').getContext('2d');  if (screen.landscape) { mctx.drawImage(canvas, x, y); } else { var angleInRadians = Math.PI / 2; mctx.rotate(angleInRadians); mctx.drawImage(canvas, x, y-screen.localHeight); mctx.rotate(-angleInRadians); } }  function renderPreview() {  var screenEl = gid('img-up-screen'); screenEl.width = screen.width; screenEl.height = screen.height;  var screenCtx = screenEl.getContext('2d');  screenCtx.rect(0, 0, screen.width, screen.height); screenCtx.fillStyle = 'rgb(' + handler.rPalete.WHITE[0] + ',' + handler.rPalete.WHITE[1] + ',' + handler.rPalete.WHITE[2] + ')'; screenCtx.fill();  var bitCursor = 0, byteN = 0;  var pixelData = screenCtx.getImageData(0, 0, screen.width, screen.height);  for (var bufferY = 0; bufferY < screen.height; bufferY++) {  for (var bufferX = 0; bufferX < screen.width; bufferX++) {   var g = getCanvasCursor(bufferX+1, bufferY+1, screen);  if (screen.bitPerPixel == 2) {  var bitData = [sbController.getBit(sbController.buffer[byteN], bitCursor), sbController.getBit(sbController.buffer[byteN], bitCursor+1)]; var color = handler.rPalete.BLACK;  if (colorsBits.WHITE[0] == bitData[0] && colorsBits.WHITE[1] == bitData[1]) color = handler.rPalete.WHITE; else if (colorsBits.GRAY1[0] == bitData[0] && colorsBits.GRAY1[1] == bitData[1]) color = handler.rPalete.GRAY1; else if (colorsBits.GRAY2[0] == bitData[0] && colorsBits.GRAY2[1] == bitData[1]) color = handler.rPalete.GRAY2;  pixelData.data[g + 0] = color[0]; pixelData.data[g + 1] = color[1]; pixelData.data[g + 2] = color[2];   } else {  var pixel = !sbController.getBit(sbController.buffer[byteN], bitCursor) ? true : false; if (pixel) { pixelData.data[g + 0] = colors.BLACK[0]; pixelData.data[g + 1] = colors.BLACK[1]; pixelData.data[g + 2] = colors.BLACK[2];  } }  bitCursor+=screen.bitPerPixel;  if (bitCursor > 7) { byteN++; bitCursor = 0;  }  } }  screenCtx.clearRect(0, 0, screen.width, screen.height); screenCtx.putImageData(pixelData, 0, 0 );   for (var i =0; i < widgets.length; i++) drawPlaceholder(widgets[i]); }  function renderImageElToBuffer(imageEl, settings) { var canvas = document.createElement('canvas'); var ctx = canvas.getContext('2d');  if (settings.fit == 'fitImage' || settings.fit == 'fitCover' || settings.fit == 'width' || settings.fit == 'height') {  var fitMode = settings.fit == 'width' || settings.fit == 'height' ? settings.fit : false; var fitCover = fitMode !== false || settings.fit == 'fitCover' ? true : false; fitToScreen(canvas, imageEl, fitMode, fitCover);  } else {  canvas.width = imageEl.width;  canvas.height = imageEl.height; ctx.drawImage(imageEl, 0, 0);  }  var pixelsImgEl = ctx.getImageData(0, 0, canvas.width, canvas.height);   sbController.init(screen.width, screen.height); sbController.reset(settings.invertBg == 'black' ? true : false);  if (settings.mode == 'atkinson') {  settings.threshhold = 0.5; if (settings.gs) greyscaleLuminance(pixelsImgEl);  if (screen.bitPerPixel == 1) { if (settings.atkinsonAuto) ditherAtkinson(pixelsImgEl, canvas.width, settings.atkinsonDrawp); else ditherAtkinson(pixelsImgEl, canvas.width, settings.atkinsonDrawp, settings.atkinsonCoef, true);  } else { ditherAtkinsonColor4(pixelsImgEl, canvas.width, settings.atkinsonDrawp, settings.atkinsonCoef);  }  }  for(var y = 1; y <= canvas.height; y++) {  for(var x = 1; x <= canvas.width; x++) {  var bitData = true; var i = getCanvasCursor(x, y, canvas);  if (screen.bitPerPixel == 2) {  var pixel = pixelsImgEl.data.slice(i, i + 4); if (pixelsImgEl.data[i + 3] <= 128) { pixel[0] = 255; pixel[1] = 255; pixel[2] = 255; }  bitData = colorsBits[findNearestColor(pixel, true, colors)];  } else { if (pixelsImgEl.data[i + 3] <= 128) {  if (settings.invertImg == 'white') { bitData = false; }  } else {  var hsv = rgbToHsv(pixelsImgEl.data[i + 0], pixelsImgEl.data[i + 1], pixelsImgEl.data[i + 2]);  if (settings.invertImg == 'black' && hsv.v < settings.threshhold) { bitData = false; }  if (settings.invertImg == 'white' && hsv.v > settings.threshhold) { bitData = false; } } }  var rX = settings.offsetX + x; var rY = settings.offsetY + y;  if (screen.landscape == false) { var tmp = rX; rX = screen.width - rY; rY = tmp;  } else {  }  if (rX >= 1 && rX <= screen.width && rY >= 1 && rY <= screen.height) {  sbController.setScreenBufferPixel(rX-1, rY-1, bitData); } }  }  }  function initWidgets() { var html = '<div><label><input type="checkbox" id="widgets-hidden-params-show"> ' + lloc('widgets_show_hidden_params') + '</label></div>'; for (var i = 0; i < widgetsOrder.length; i++) { var widget = handler.getWidgetByKey(widgetsOrder[i]);  html+= '<div class="widget">\ X <input id="widget-' + widget.id + '-x" type="text" class="offset-controll" value="' + widget.x + '" data-step="1">\ Y <input id="widget-' + widget.id + '-y" type="text" class="offset-controll" value="' + widget.y +'" data-step="1">\ <label><input type="checkbox" class="widget-switch" id="widget-' + widget.id + '-enabled">' + lloc('widget_' + widget.id) + '</label>\ <div class="widget-hidden-params"><input type="text" class="widget-params" placeholder="Params of ' + widget.id + '" id="widget-' + widget.id + '-params"></div>\ </div>'; }  gid('widgets').innerHTML = html; gid('widgets-hidden-params-show').onclick = function() { if (this.checked) { gid('widgets').classList.add('widgets-show-hidden'); KellyTools.updateSpoilerBounds();  } else { gid('widgets').classList.remove('widgets-show-hidden'); KellyTools.updateSpoilerBounds();  } };  var els = document.getElementsByClassName('offset-controll'); for (var i = 0; i < els.length; i++) {  els[i].onclick = function() { handler.scrollTarget = this; delayUpdateCurrent(); }; els[i].onchange = delayUpdateCurrent;  }  els = document.getElementsByClassName('widget-switch'); for (var i = 0; i < els.length; i++) { els[i].onclick = delayUpdateCurrent;  } }  function initUploadedFile(settings, onReady) { if (settings.mode == 'json-preloaded') { onReady(handler.preloadedJSON); return; } else if (settings.files.length <= 0) {  onReady(false); return; }  var reader = new FileReader(); reader.addEventListener("loadend", function() {  if (settings.mode == 'json') {  try { var cfg = JSON.parse(reader.result.trim()); onReady(cfg); } catch (e) { console.log(e); onReady(false); }  } else if (settings.mode == 'rawbin') {  onReady(new Uint8Array(reader.result));  } else if (settings.mode == 'rawtxt') {  onReady(reader.result);  } else {  var src = new Image(); src.onload = function() { onReady(this); };  src.onerror = function() { onReady(false); };  src.src = reader.result; } });  if (settings.mode == 'json' || settings.mode == 'rawtxt') { reader.readAsText(settings.files[0]); } else if (settings.mode == 'rawbin') { reader.readAsArrayBuffer(settings.files[0]); } else { reader.readAsDataURL(settings.files[0]); } };  function updateDispInfo() {  var screenEl = gid('img-up-screen'); var changeOption = function(e) { if (this.id == 'disp-orient') { screen.landscape = !screen.landscape; delayUpdateCurrent(); } else if (this.id == 'disp-bits') { screen.bitPerPixel = screen.bitPerPixel == 1 && screen.color4 ? 2 : 1; sbController.init(screen.width, screen.height); delayUpdateCurrent(); } else { screen.flip = !screen.flip; } updateDispInfo(); };  gid('disp-flip').onclick = changeOption; gid('disp-orient').onclick = changeOption; gid('disp-bits').onclick = changeOption;  screenEl.classList.remove('portrait-mode'); screenEl.classList.remove('portrait-mode-flip'); screenEl.classList.remove('landscape-mode-flip'); screenEl.classList.remove('landscape-mode');  if (!screen.landscape) {  screen.localWidth = screen.height; screen.localHeight = screen.width; screenEl.classList.add('portrait-mode');  if (screen.flip) screenEl.classList.add('portrait-mode-flip');  } else {  screen.localWidth = screen.width; screen.localHeight = screen.height;  screenEl.classList.add('landscape-mode'); if (screen.flip) screenEl.classList.add('landscape-mode-flip'); }  var t = " [" + screen.localWidth +"x" + screen.localHeight + "]"; gid('disp-orient').innerText = screen.landscape ? 'Landscape ' + t : 'Portrait' + t; gid('disp-flip').innerText = screen.flip ? 'Flip screen' : 'No Flip screen'; gid('disp-bits').innerText = screen.bitPerPixel == 1 ? '1-bit per pixel' : '2-bit per pixel';  gid('bit-1-bit').style.display = 'none'; gid('bit-2-bit').style.display = 'none';  gid('bit-' + screen.bitPerPixel + '-bit').style.display = '';   KellyTools.updateSpoilerBounds();  };  handler.getUploadSettings = function() {  var file = gid('img-up-image'); var settings = { mode : document.querySelector('input[name=img-up-method]:checked').value,  atkinsonCoef : KellyTools.validateFloat(gid('img-up-matkinson-coefficient').value), atkinsonDrawp : gid('img-up-matkinson-drawp').checked, atkinsonAuto : gid('img-up-matkinson-auto').checked, gs : gid('img-up-gs').checked, threshhold : KellyTools.validateFloat(gid('img-up-threshhold-value').value), invertImg : gid('img-up-invert-color').checked ? 'black' : 'white',  invertBg : gid('img-up-invert-bg-color').checked ? 'black' : 'white',  fit : gid('img-up-fit-mode').value, filename : file.files.length <= 0 ? false : file.files[0].name, ext : file.files.length <= 0 ? '' : KellyTools.getExt(file.files[0].name), files : file.files, offsetX : KellyTools.validateInt(gid('img-up-offset-x').value), offsetY : KellyTools.validateInt(gid('img-up-offset-y').value), uploadReady : true, };  if (handler.preloadedJSON) settings.mode = 'json-preloaded'; if (settings.ext == 'json') settings.mode = 'json'; if (settings.ext == 'bin') settings.mode = 'rawbin'; if (settings.ext == 'txt') settings.mode = 'rawtxt';  if (settings.mode != 'json-preloaded' && settings.files.length <= 0) settings.uploadReady = false;  return settings; };  handler.uploadCurrent = function(onRequestsReady, additionalSettings) {  if (typeof additionalSettings == 'undefined' || !additionalSettings) additionalSettings = { displayAfterUpload : true};   if (typeof onRequestsReady == 'undefined') onRequestsReady = function() {};  if (handler.sendPackRequest) { handler.sendPackResetWork(); KellyTools.showNotice(lloc('cui_cancel'), true);  return false; }  var sendImage = function(attemp, offset) {  if (!offset) offset = 0; var oldOffset = offset;  var target = "buffer"; var data = sbController[target]; if (screen.bitPerPixel > 1) {  data = new Uint8Array(sbController[target].length / screen.bitPerPixel); for(var i = 0; i < data.length; i++) { data[i] = sbController[target][offset + i]; } offset += data.length; }  var blob = new Blob([data], {type : 'plain/text'}); var fileOfBlob = new File([blob], 'image.bin'); var packData = new FormData(); packData.append("image", fileOfBlob);  handler.sendPackRequest = KellyTools.cfetch("/api/direct/image", {method : 'POST', body : packData, responseType : 'json'}, function(response, error) {  if (!handler.sendPackStart) return;  if (!response || response.status == "fail") {  if (!attemp) { attemp = 1; } else attemp++;  if (attemp < 3) { setTimeout(function() {sendImage(attemp, oldOffset)}, 1000);  } else { KellyTools.showNotice(lloc('cui_image_data_send_fail') + (response && response.err ? response.err : error), true);  onRequestsReady(false); handler.sendPackResetWork(); }  return; } else { if (screen.bitPerPixel > 1 && offset < sbController.buffer.length) { console.log("send " + offset + 'vs' + sbController.buffer.length); handler.sendPackRequest = { timer : setTimeout(function() {sendImage(0, offset)}, 1000), abort : function() {clearTimeout(handler.sendPackRequest.timer);},  }; KellyTools.showNotice(offset + "/" + sbController.buffer.length + " " + lloc('cui_bytes_sended'));  } else { onRequestsReady(true, response); handler.sendPackResetWork();  if (response.reboot_required) {  var formData = new FormData(); formData.append('reboot', '1');   KellyTools.cfetch("/api/reboot", {method : 'POST', body : formData, responseType : 'json'}, function(response, error) {});  KellyTools.showNotice(lloc('cui_restart_required'));  } else { KellyTools.showNotice(lloc('cui_done'));  }  }  } }, {timeout : 50 * 1000});  };  var sendWidgets = function(onContinue) {  if (!onContinue) onContinue = function() {};  var widgetsData = new FormData(); widgetsData.append("commit", '1'); widgetsData.append("screen-landscape", screen.landscape ? '1' : '0'); widgetsData.append("screen-flip", screen.flip ? '1' : '0'); widgetsData.append("screen-bits", screen.bitPerPixel);  widgetsData.append("display-after-upload", !additionalSettings.displayAfterUpload ? '0' : '1');  if (handler.defaultFileName) { widgetsData.append("filename", handler.defaultFileName); }  for (var i=0; i < widgets.length; i++) { if (widgetsOrder.indexOf(widgets[i].id) == -1) continue; if (!gid('widget-' + widgets[i].id + '-enabled').checked) continue; var data = '1;' + gid('widget-' + widgets[i].id + '-x').value + ';' + gid('widget-' + widgets[i].id + '-y').value + ';';  var params = gid('widget-' + widgets[i].id + '-params'); if (params) { data += params.value.substr(0, PARAMS_MAX_SIZE) + ';' } else data += ';';  widgetsData.append("widget-" + widgets[i].uuid, data);   }  handler.sendPackRequest = KellyTools.cfetch("/api/direct/widgets", {method : 'POST', body : widgetsData, responseType : 'json'}, function(response, error) { if (!handler.sendPackStart) return;  if (!response) { KellyTools.showNotice(lloc('cui_device_is_not_responding'), true);  onRequestsReady(false); handler.sendPackResetWork(); } else if (response.status != "enabled"){ onRequestsReady(false); handler.sendPackResetWork(); KellyTools.showNotice(lloc('cui_accept_widgets_fail'), true);  } else { onContinue(); } }); };  handler.sendPackResetWork();  KellyTools.cLock = true;  handler.sendPackStart = true;  KellyTools.showNotice(lloc('cui_process') + "...");   sendWidgets(function() { handler.sendPackRequest = { timer : setTimeout(sendImage, 500), abort : function() {clearTimeout(handler.sendPackRequest.timer);},  }; });  return true; };  this.getSB = function() {return sbController;};  this.initPreviewFromInput = function(onReady, initiator){  if (!onReady) onReady = function() {};  if (KellyTools.locked(true)) { onReady(false); return; }  var settings = handler.getUploadSettings(); handler.lastSettings = settings;  if (!settings.uploadReady) { KellyTools.showNotice(lloc('cui_file_not_selected')); onReady(false, settings); return; }  KellyTools.cLock = true; initUploadedFile(settings, function(result) {  KellyTools.cLock = false;  if (settings.mode == 'json' || settings.mode == 'json-preloaded') {  if (!result || !result.simpleBuffer) {  KellyTools.showNotice(lloc('cui_parse_fail_json')); onReady(false, settings);  } else {  if (!handler.widgetsUpdatedByCfg) {  if (result.screen) {  if (!screen.color4 && result.screen.bitPerPixel == 2) { KellyTools.showNotice(lloc('cui_parse_fail_2bit')); onReady(false, settings); return; }   if (screen.width != result.screen.width || screen.height != result.screen.height) { KellyTools.showNotice(lloc('cui_parse_fail_proportions') + ' ' + result.screen.width + 'x' + result.screen.height); onReady(false, settings); return; }  screen.landscape = result.screen.landscape; screen.flip = result.screen.flip; if (!result.screen.bitPerPixel) result.screen.bitPerPixel = 1; screen.bitPerPixel = result.screen.bitPerPixel;  updateDispInfo(); }  sbController.init(screen.width, screen.height);  for (var i = 0; i < sbController.buffer.length; i++) sbController.buffer[i] = result.simpleBuffer[i]; for (var i = 0; i < widgets.length; i++) {  var widgetEnabled = gid('widget-' + widgets[i].id + '-enabled'); if (!widgetEnabled) continue;  widgetEnabled.checked = false;  var key = -1; for (var b = 0; b < result.widgets.length; b++) { if (result.widgets[b].id == widgets[i].id) key = b; }  if (key == -1) continue; if (widgetsOrder.indexOf(widgets[i].id) == -1) continue;  widgetEnabled.checked = result.widgets[key].enabled; gid('widget-' + widgets[i].id + '-x').value = result.widgets[key].x; gid('widget-' + widgets[i].id + '-y').value = result.widgets[key].y;  var params = gid('widget-' + widgets[i].id + '-params'); if (params) params.value = result.widgets[key].params ? result.widgets[key].params.substr(0, PARAMS_MAX_SIZE) : ''; }  handler.widgetsUpdatedByCfg = true; }  renderPreview(); onReady(true, settings); }  } else if (settings.mode == 'rawbin'|| settings.mode == 'rawtxt') {  if (settings.mode == 'rawtxt') {  var buffer1 = new Uint8Array( sbController.bufferLen );; var txtData = result.split(',');  for (var i = 0; i < sbController.buffer.length; i++) {  if (i > txtData.length-1) {  buffer1[i] = 0;  } else {  buffer1[i] = txtData[i].trim(); var byteData = parseInt(txtData[i]);  if (isNaN(byteData)) byteData = 0; if (byteData > 255) byteData = 255;  buffer1[i] = byteData; } }  result = buffer1; }  if (!result || result.length != sbController.buffer.length) {  KellyTools.showNotice(lloc('cui_parse_fail_buffer_size') + ' [' + result.length + lloc('cui_byte') + ' \ ' + sbController.buffer.length + lloc('cui_byte') + ']'); onReady(false, settings);  } else {  sbController.buffer = result; renderPreview(); onReady(true, settings); }  } else {  if (!result) {  KellyTools.showNotice(lloc('cui_parse_fail_image_read_error')); onReady(false, settings);  } else {  renderImageElToBuffer(result, settings); renderPreview(); onReady(true, settings); } } }); };  this.initScreen = function() {  var screenEl = gid('img-up-screen'); screenEl.width = screen.width; screenEl.height = screen.height;  updateDispInfo(); };  this.init = function() {  lng = KellyTools.detectLanguage();  if (env.cfgSaved && env.cfgSaved["__PRODUCT_VERSION"]) { KellyTools.version = env.cfgSaved["__PRODUCT_VERSION"];  }  KellyTools.showTitle();  var translatable = document.querySelectorAll('[data-loc]'); for (var i = 0; i < translatable.length; i++) { translatable[i].innerHTML = lloc(translatable[i].getAttribute('data-loc')); }  gid('device-info-container').classList.add('shown'); gid('img-up-preview').style.display = ''; handler.initScreen();   var imgForm = gid('img-up');  gid('page').appendChild(imgForm);  imgForm.style.display = '';  gid('device-info-container').innerHTML = ''; gid('device-info-container').appendChild(gid('img-up-preview'));  gid('img-up-image').onchange = function() { handler.preloadedJSON = false; gid('img-up-offset-x').value = 0; gid('img-up-offset-y').value = 0; handler.widgetsUpdatedByCfg = false; };  gid('img-up-matkinson-coefficient').onclick = delayUpdateCurrent; gid('img-up-gs').onclick = delayUpdateCurrent; gid('img-up-matkinson-auto').onclick = delayUpdateCurrent; gid('img-up-matkinson-drawp').onclick = delayUpdateCurrent;  gid('img-up-fit-mode').onchange = delayUpdateCurrent; gid('img-up-offset-x').onchange = delayUpdateCurrent; gid('img-up-offset-y').onchange = delayUpdateCurrent; gid('img-up-invert-color').onchange = delayUpdateCurrent; gid('img-up-invert-bg-color').onchange = delayUpdateCurrent; gid('img-up-matkinson').onclick = delayUpdateCurrent; gid('img-up-mthreshhold').onclick = delayUpdateCurrent;  handler.lastScrollTop = window.pageYOffset || document.documentElement.scrollTop;  document.addEventListener("click", function(e){ handler.scrollTarget = e.target.id && ['img-up-offset-x', 'img-up-offset-y', 'img-up-threshhold-value'].indexOf(e.target.id) != -1 ? e.target : false; }, false);  document.addEventListener("wheel", function(e){  if (KellyTools.locked(true) || !handler.scrollTarget) return;  var st = window.pageYOffset || document.documentElement.scrollTop;  var stVal = handler.scrollTarget.getAttribute('data-step'); var isFloat = stVal.indexOf('.') !== -1; var step = isFloat ? KellyTools.validateFloat(stVal) : KellyTools.validateInt(stVal); var value = isFloat ? KellyTools.validateFloat(handler.scrollTarget.value) : KellyTools.validateInt(handler.scrollTarget.value);  value += step * (event.deltaY < 0 ? 1 : -1);  handler.scrollTarget.value = value; lastScrollTop = st <= 0 ? 0 : st;  delayUpdateCurrent(); e.preventDefault(); }, { passive: false });  gid('img-up-save').onclick = function() { KellyTools.locked(); handler.initPreviewFromInput(function(result, settings){  if (!result) return;  KellyTools.downloadFile(sbController.buffer, (settings.filename ? settings.filename : 'default') + '_' + screen.width +'x' + screen.height + '_' + 'bw' + '.bin', "application/octet-stream"); }); };  gid('img-up-save-all').onclick = function() { KellyTools.locked(); handler.initPreviewFromInput(function(result, settings){ if (!result) return; var array = []; for (var i = 0; i < sbController.buffer.length; i++) array[i] = sbController.buffer[i]; KellyTools.downloadFile(JSON.stringify({widgets : widgets, simpleBuffer : array, screen : screen}), (settings.filename ? settings.filename : 'default') + '_' + screen.width +'x' + screen.height + '_' + 'bw' + '.json', "application/json"); }); };  gid('img-up-show-preview').onclick = function() { KellyTools.locked(); handler.initPreviewFromInput(false, 'preview'); };  gid('img-up-upload').onclick = function() {  if (!handler.getUploadSettings().uploadReady) { KellyTools.showNotice(lloc('cui_upload_nothing')); return; }  handler.uploadCurrent(); };  gid('img-up-reset').onclick = function() {  if (handler.sendPackRequest) { handler.sendPackResetWork(); KellyTools.showNotice(lloc('cui_cancel'), true);  return; }   var widgetsData = new FormData(); widgetsData.append("commit", '0');  handler.sendPackRequest = KellyTools.cfetch("/api/direct/widgets", {method : 'POST', body : widgetsData, responseType : 'json'}, function(response, error) { if (response.status == "disabled") { KellyTools.showNotice(lloc('cui_done'));  } else KellyTools.showNotice("Error reset sreen", true);  handler.sendPackResetWork(); }); };  initWidgets();  } }