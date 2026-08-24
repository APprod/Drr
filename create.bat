@echo off
setlocal

rem Creates a ready app project from the ApEngine template.
rem Usage (run from your project repo root): <path-to-ApEngine>\create.bat <appName>

if "%~1"=="" (
    echo Usage: create.bat ^<appName^>
    echo Run from your project repo root. Creates .\apps\^<appName^> from the
    echo ApEngine template and copies root scripts only where they are missing.
    exit /b 1
)

set "APP_NAME=%~1"
set "TEMPLATE_DIR=%~dp0template"

echo %APP_NAME%| findstr /R /C:"^[A-Za-z0-9_-]*$" >nul
if errorlevel 1 (
    echo [create] Invalid app name "%APP_NAME%" - use letters, digits, _ or -.
    exit /b 1
)

if not exist "%TEMPLATE_DIR%\app" (
    echo [create] Template not found near create.bat: %TEMPLATE_DIR%\app
    exit /b 1
)

if exist "apps\%APP_NAME%" (
    echo [create] apps\%APP_NAME% already exists - aborting.
    exit /b 1
)

echo [create] Copying template app to apps\%APP_NAME% ...
xcopy "%TEMPLATE_DIR%\app" "apps\%APP_NAME%\" /E /I /Q /Y >nul
if errorlevel 1 (
    echo [create] Copy failed.
    exit /b 1
)

set COPIED_ROOT_CMAKE=0
set COPIED_ROOT_BAT=0
for %%F in (CMakeLists.txt build.bat run.bat build.sh .gitignore) do (
    if exist "%TEMPLATE_DIR%\%%F" (
        if exist "%%F" (
            echo [create] Root %%F already exists - keeping yours.
        ) else (
            copy "%TEMPLATE_DIR%\%%F" ".\" >nul
            echo [create] Copied %%F to repo root.
            if /I "%%F"=="CMakeLists.txt" set COPIED_ROOT_CMAKE=1
            if /I "%%F"=="build.bat"     set COPIED_ROOT_BAT=1
        )
    )
)

rem Register the new app in a pre-existing root CMakeLists.txt (idempotent).
if "%COPIED_ROOT_CMAKE%"=="0" (
    findstr /C:"add_subdirectory(apps/%APP_NAME%)" "CMakeLists.txt" >nul 2>&1
    if errorlevel 1 (
        powershell -NoProfile -Command "Add-Content 'CMakeLists.txt' ''; Add-Content 'CMakeLists.txt' 'add_subdirectory(apps/%APP_NAME%)'"
        echo [create] Appended add_subdirectory^(apps/%APP_NAME%^) to root CMakeLists.txt.
    )
)

rem Bake the app name into the fresh copies so scripts and CMake just work.
powershell -NoProfile -Command "(Get-Content 'apps\%APP_NAME%\CMakeLists.txt') -replace 'add_ap_project\(template\)', 'add_ap_project(%APP_NAME%)' | Set-Content 'apps\%APP_NAME%\CMakeLists.txt'"

if "%COPIED_ROOT_CMAKE%"=="1" (
    powershell -NoProfile -Command "(Get-Content 'CMakeLists.txt') -replace 'apps/template', 'apps/%APP_NAME%' -replace 'project\(AppTest\)', 'project(%APP_NAME%)' | Set-Content 'CMakeLists.txt'"
)
if "%COPIED_ROOT_BAT%"=="1" (
    powershell -NoProfile -Command "(Get-Content 'build.bat') -replace 'set \"APP_NAME=\"', 'set \"APP_NAME=%APP_NAME%\"' | Set-Content 'build.bat'"
)

echo [create] Done: apps\%APP_NAME%
echo [create] Build it with:  build.bat debug static --target %APP_NAME%
endlocal
