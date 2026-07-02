echo off



call ./build.bat %*
if %errorlevel% neq 0 exit /b 
_build\app.exe