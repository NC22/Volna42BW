# Volna42 web environment & helper tools

To convert web-data resources to C++ headers files, execute

minify_einkdisplay.bat - on Windows

minify_einkdisplay.sh - on Linux (untested, check the *htmlpack* subfolder to compile the packer tool)

This updates the *.h files in the "Volna42\src\ui\out" folder with the contents of the html, js, css files of the web interface for loading them from the device's FLASH memory later


PREVIEW___ - files to test js compile \ basic functions

# Web page structure & logic

Default js resources loading order 

env.js (ENV.js) -> language.js (locale/locale[#defaultLocale].js) -> tools.js (KellyTools.js) -> core.js (KellyEnv.js)

loading order and initialization logic in init.js file

Main page structure

[client.html][var ENVCFG][init.js -> js resources loading -> ENV.init()][client_end.html]

ENVDATA - contains localization variables (preset or initialized by KellyTools.detectLanguage from var ENVLOCALE) and settings page structure
ENVDATA.cfgSaved = ENVCFG - current saved settings