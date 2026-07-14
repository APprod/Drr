@echo off


call ./build.bat %*
if %errorlevel% neq 0 exit /b

if "%WEB%"=="True" (
    emrun %BUILD_DIR_BASE%%BUILD_DIR%\app.html
) else (
    %BUILD_DIR_BASE%%BUILD_DIR%\app.exe
)
