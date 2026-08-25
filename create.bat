@echo off
setlocal

rem Creates a ready app project from the ApEngine template.
rem Usage (run from your project repo root): <path-to-ApEngine>\create.bat [<appName>] [--ci]

set "APP_NAME="
set "CI=0"
set "TEMPLATE_DIR=%~dp0template"

:parse_args
if "%~1"=="" goto :end_parse_args
if /I "%~1"=="--ci" set "CI=1" & shift & goto :parse_args
if "%APP_NAME%"=="" set "APP_NAME=%~1" & shift & goto :parse_args
echo [create] Unknown argument "%~1"
shift
goto :parse_args
:end_parse_args

if "%APP_NAME%"=="" (
    if "%CI%"=="1" goto :ci_only
    echo Usage: create.bat ^<appName^> [--ci]
    echo Run from your project repo root. Creates .\apps\^<appName^> from the
    echo ApEngine template and copies root scripts only where they are missing.
    echo Use --ci to also copy GitHub CI workflows from template\ci.
    echo Use --ci alone to only copy CI workflows without creating an app.
    exit /b 1
)

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
for %%F in (CMakeLists.txt build.bat run.bat build.sh .gitignore) do (
    if exist "%TEMPLATE_DIR%\%%F" (
        if exist "%%F" (
            echo [create] Root %%F already exists - keeping yours.
        ) else (
            copy "%TEMPLATE_DIR%\%%F" ".\" >nul
            echo [create] Copied %%F to repo root.
            if /I "%%F"=="CMakeLists.txt" set COPIED_ROOT_CMAKE=1
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

rem Fresh root CMakeLists still points at the template folder - retarget it.
if "%COPIED_ROOT_CMAKE%"=="1" (
    powershell -NoProfile -Command "(Get-Content 'CMakeLists.txt') -replace 'apps/template', 'apps/%APP_NAME%' -replace 'project\(AppTest\)', 'project(%APP_NAME%)' | Set-Content 'CMakeLists.txt'"
)

:ci_only
if "%CI%"=="1" (
    if not exist "%TEMPLATE_DIR%\ci" (
        echo [create] --ci requested but template\ci not found - skipping.
    ) else (
        if not exist ".github\workflows" mkdir ".github\workflows" 2>nul
        for %%F in ("%TEMPLATE_DIR%\ci\*") do (
            if exist ".github\workflows\%%~nxF" (
                echo [create] .github\workflows\%%~nxF already exists - keeping yours.
            ) else (
                copy "%%F" ".github\workflows\" >nul
                echo [create] Copied ci\%%~nxF to .github\workflows\
            )
        )
    )
)

if not "%APP_NAME%"=="" (
    echo [create] Done: apps\%APP_NAME%
    echo [create] Build it with:  build.bat debug static --target %APP_NAME%
) else (
    echo [create] Done: CI workflows installed.
)
endlocal
