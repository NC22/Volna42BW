@echo off

:: Установите Build Tools for Visual Studio
:: https://visualstudio.microsoft.com/downloads/
:: Укажите путь к vcvarsall.bat, может отличатся подпапка

set VCVARS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
:: set VCVARS_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"

set ARCH=x64

:: Для релиза DEBUG = 0
set DEBUG=0

if "%1"=="Release" (
    set DEBUG=0
)

if "%1"=="Debug" (
    set DEBUG=1
)

call %VCVARS_PATH% %ARCH%

set SRC=htmlpack.cpp
set OUT=htmlpack.exe

if "%DEBUG%"=="0" (
    echo Compiling in Release mode...
    cl.exe /EHsc /O2 /MD %SRC% /Fe:%OUT%
) else (
    echo Compiling in Debug mode...
    cl.exe /EHsc /MDd %SRC% /Fe:%OUT%
)

if %ERRORLEVEL% equ 0 (
    echo Compilation successful.
    echo Run the program using %OUT%
) else (
    echo Compilation failed.
)

pause