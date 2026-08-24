@echo off

set "BUILD_TYPE=Debug"
set "STATIC_LINKING=ON"
set "BUILD_DIR_BASE=_build\"
set "BUILD_DIR=Debug"
set "WEB=False"
set "MSVC=False"
set "PROFILE=OFF"
set "APP_NAME="

set "START_TIME=%TIME%"

:parse
if "%1"=="" goto :endparse
if /I "%1"=="--target" (set "APP_NAME=%2" & shift & shift & goto :parse)
if /I "%1"=="release" set "BUILD_TYPE=Release" & set "BUILD_DIR=Release" & shift & goto :parse
if /I "%1"=="debug" set "BUILD_TYPE=Debug" & set "BUILD_DIR=Debug" & shift & goto :parse
if /I "%1"=="static" set "STATIC_LINKING=ON" & shift & goto :parse
if /I "%1"=="dynamic" set "STATIC_LINKING=OFF" & shift & goto :parse
if /I "%1"=="web" set "WEB=True" & set "BUILD_DIR_BASE=_build\Web\" & shift & goto :parse
if /I "%1"=="msvc" set "MSVC=True" & shift & goto :parse
if /I "%1"=="profile" set "PROFILE=ON" & shift & goto :parse
:endparse

set "TARGET_ARGS="
if not "%APP_NAME%"=="" set "TARGET_ARGS=--target %APP_NAME%"

if "%MSVC%"=="True" set "BUILD_DIR=msvc-%BUILD_DIR%"
if "%WEB%"=="True" goto :webBuild
if "%MSVC%"=="True" goto :msvcBuild
goto :desktop

:webBuild

echo Building %BUILD_TYPE% configuration... For Web
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if defined EMSDK (
    cmake -S . -B %BUILD_DIR_BASE%%BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DPLATFORM=Web "-DCMAKE_TOOLCHAIN_FILE=%EMSDK%\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake"
) else (
    emcmake cmake -S . -B %BUILD_DIR_BASE%%BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DPLATFORM=Web
)
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %BUILD_DIR_BASE%%BUILD_DIR% %TARGET_ARGS% --parallel
if %errorlevel% neq 0 exit /b %errorlevel%
goto :endBuild

:msvcBuild

echo Building %BUILD_TYPE% configuration... MSVC
cmake -S . -B %BUILD_DIR_BASE%%BUILD_DIR% -G "Visual Studio 17 2022" -A x64 -DIS_STATIC=%STATIC_LINKING% -DPROFILE=%PROFILE%
if %errorlevel% neq 0 exit /b %errorlevel%
cmake --build %BUILD_DIR_BASE%%BUILD_DIR% %TARGET_ARGS% --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b %errorlevel%
goto :endBuild

:desktop

echo Building %BUILD_TYPE% configuration...

cmake -S . -B%BUILD_DIR_BASE%%BUILD_DIR% -G "MinGW Makefiles" -DIS_STATIC=%STATIC_LINKING%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE%  -DPROFILE=%PROFILE%
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build %BUILD_DIR_BASE%%BUILD_DIR% %TARGET_ARGS% --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b %errorlevel%

:endBuild

set "END_TIME=%TIME%"
call :ElapsedTime "%START_TIME%" "%END_TIME%"
exit /b 0

:ElapsedTime
setlocal
set "start=%~1"
set "end=%~2"

for /f "tokens=1-4 delims=:.," %%a in ("%start%") do (
    set /a "start_secs=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)
for /f "tokens=1-4 delims=:.," %%a in ("%end%") do (
    set /a "end_secs=(((%%a*60)+1%%b %% 100)*60+1%%c %% 100)*100+1%%d %% 100"
)

set /a elapsed=end_secs-start_secs
if %elapsed% lss 0 set /a elapsed+=8640000

set /a cs=elapsed %% 100
set /a elapsed/=100
set /a secs=elapsed %% 60
set /a elapsed/=60
set /a mins=elapsed %% 60
set /a hours=elapsed/60

echo.
echo Total time: %hours%h %mins%m %secs%.%cs%s
endlocal
goto :eof