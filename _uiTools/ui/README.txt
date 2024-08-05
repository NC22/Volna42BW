Порядок подгрузки :

env.js (ENV.js) -> language.js (locale/locale[#defaultLocale].js) -> tools.js (KellyTools.js) -> core.js (KellyEnv.js)

порядок загрузки и логика инициализации в файле init.js

структура страницы [client.html][var ENVCFG][init.js][client_end.html]

ENVDATA - переменные локализации (предустановленные или проинициализированные KellyTools.detectLanguage из var ENVLOCALE) и структура страницы настроек
ENVDATA.cfgSaved = ENVCFG - текущие сохраненные настройки