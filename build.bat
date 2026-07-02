@echo off


set BUILD_TYPE=Debug
set STATIC_LINKING=ON

:parse
if "%1"=="" goto :endparse
if "%1"=="release" set BUILD_TYPE=Release & shift & goto :parse
if "%1"=="debug" set BUILD_TYPE=Debug & shift & goto :parse
if "%1"=="static" set STATIC_LINKING=ON & shift & goto :parse
if "%1"=="dynamic" set STATIC_LINKING=OFF & shift & goto :parse
:endparse

echo on

echo Building %BUILD_TYPE% configuration...

cmake -S . -B_build -G "MinGW Makefiles" -DIS_STATIC=%STATIC_LINKING%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% 
if %errorlevel% neq 0 exit /b 

cmake --build _build --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b 

xcopy "assets" "_build/assets/" /s /e /h /I /Y
if %errorlevel% neq 0 exit /b 

