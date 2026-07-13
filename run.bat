@echo off

set BUILD_DIR=_build\Debug

if "%2"=="release" set BUILD_DIR=_build\Release
if "%2"=="debug"   set BUILD_DIR=_build\Debug
if "%1"=="release" set BUILD_DIR=_build\Release
if "%1"=="debug"   set BUILD_DIR=_build\Debug

call ./build.bat %*
if %errorlevel% neq 0 exit /b
%BUILD_DIR%\app.exe