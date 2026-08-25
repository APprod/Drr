@echo off

call ./build.bat %*
if %errorlevel% neq 0 exit /b

if "%APP_NAME%"=="" (
    echo Usage: run.bat --target ^<app_name^> [debug^|release^|static^|dynamic^|web^|msvc^|profile]
    exit /b 1
)

if "%WEB%"=="True" (
    emrun %BUILD_DIR_BASE%%BUILD_DIR%\apps\%APP_NAME%\%APP_NAME%.html
) else (
    %BUILD_DIR_BASE%%BUILD_DIR%\apps\%APP_NAME%\%APP_NAME%.exe
)
