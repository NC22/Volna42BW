# Tool for convert html, js, css data

<b>htmlpack</b> - command-line tool written in C++ for convert html, js, css data to C++ progmem packed string with remove comments, ready to use in Arduino Framework. 

# on Windows
For compile on Windows check instructions & configuration inside Makefile.bat file, after check configuration, run Makefile.bat

see example.bat for command example

# on Linux
For compile on Linux related system, be sure you have installed g++ compiler

Usually on Ubuntu \ Debian like systems

sudo apt update<br>
sudo apt install build-essential<br>

navigate to source file directory and enter command "make"

enter "./example.sh" to run test. This will pack test.html to h file in out folder