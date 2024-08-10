var loadPool = ['uploaderfm.js', 'uploader.js', 'language.js', 'tools.js', 'env.js', 'style.css'];
var ENVINIT = function(result) {
    
    if (result) {
        
        ENVDATA.cfgSaved = ENVCFG;
        ENVDATA.screen = ENVSCREEN;
        
        window.ENV = new KellyImgUpl(ENVDATA);
        window.ENV.init();
        window.ENVFM = new KellyUIFM();
        window.ENVFM.init();  
        
    } else console.log('load page fail');
    
};

load();