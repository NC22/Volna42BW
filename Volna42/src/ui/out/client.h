const char webdata_client_html[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, minimum-scale=1, initial-scale=1\"> <style>body, html {background: #262626; color: #bfbfbf; } #result {opacity : 0;}</style> </head> <body>  <div class=\"page-wrapper\">  <div class=\"page-header\" > <div id=\"header\">0%</div> </div>  <div id=\"device-info-container\">  <div id=\"notification-info\"></div>  <div id=\"device-info\"></div>  <div class=\"device-info-actions\"><a href=\"#\" id=\"device-info-refresh\"></a></div> </div>  <div class=\"page-options\" id=\"page\"></div>  <div id=\"result\" class=\"notice\"><div id=\"result-close\">X</div><div></div></div>  </div> <script> var load = function(src, max) { max = !max ? 1 : max+1; var v = typeof window.ENVCFG !== 'undefined' && window.ENVCFG['__V'] ? '/' + ENVCFG['__V'] : ''; if (typeof src == 'undefined') window.ENVITEMS = loadPool.length; else document.getElementById('header').innerText = Math.ceil((100 / window.ENVITEMS) * (window.ENVITEMS - loadPool.length)) + \"%\"; if (!src && loadPool.length <= 0) return ENVINIT(true); if (max >= 30) return ENVINIT(false); src = src ? src : v + '/out/' + loadPool.pop(); var dt = document.createElement(src.indexOf('css') == -1 ? 'SCRIPT' : 'LINK'); if (dt.tagName == 'SCRIPT') { dt.src = src; } else { dt.href = src; dt.rel = 'stylesheet'; dt.type = 'text/css'; } dt.onload = function() { load(false); }; dt.onerror = function() { load(src, max); };  document.getElementsByTagName('head')[0].appendChild(dt); }; ";
const unsigned int webdataSize_client_html PROGMEM = 1506;