var loadPool = ['core.js', 'tools.js', 'language.js', 'env.js'];

var ENVINIT = function(result) {
    
    if (result) {
        
        ENVDATA.cfgSaved = ENVCFG;
        
        window.ENV = new KellyEnv(ENVDATA);
        window.ENV.init();
        
    } else console.log('load page fail');
    
};

load();