var loadPool = ['core.js', 'tools.js', 'language.js', 'env.js', 'style.css'];

var ENVINIT = function(result) {
    
    if (result) {
        
        ENVDATA.cfgSaved = ENVCFG;
        
        window.ENV = new KellyEnv(ENVDATA);
        window.ENV.init();
        
    } else console.log('load page fail');
    
};

load();