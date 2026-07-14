@echo off

set "BUILD_TYPE=Debug"
set "STATIC_LINKING=ON"
set "BUILD_DIR_BASE=_build\"
set "BUILD_DIR=Debug"
set "WEB=False"
set "MSVC=False"

:parse
if "%1"=="" goto :endparse
if "%1"=="release" set "BUILD_TYPE=Release" & set "BUILD_DIR=Release" & shift & goto :parse
if "%1"=="debug" set "BUILD_TYPE=Debug" & set "BUILD_DIR=Debug" & shift & goto :parse
if "%1"=="static" set "STATIC_LINKING=ON" & shift & goto :parse
if "%1"=="dynamic" set "STATIC_LINKING=OFF" & shift & goto :parse
if "%1"=="web" set "WEB=True" & set "BUILD_DIR_BASE=_build\Web\" & shift & goto :parse
if "%1"=="msvc" set "MSVC=True" & shift & goto :parse
:endparse

if "%MSVC%"=="True" set "BUILD_DIR=msvc-%BUILD_DIR%"
if "%WEB%"=="True" goto :webBuild
if "%MSVC%"=="True" goto :msvcBuild
goto :desktop

:webBuild

echo Building %BUILD_TYPE% configuration... For Web
emcmake cmake -S . -B %BUILD_DIR_BASE%%BUILD_DIR% -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DPLATFORM=Web
if %errorlevel% neq 0 exit /b 
cmake --build %BUILD_DIR_BASE%%BUILD_DIR% --target app --parallel
goto :endBuild

:msvcBuild

echo Building %BUILD_TYPE% configuration... MSVC
cmake -S . -B %BUILD_DIR_BASE%%BUILD_DIR% -G "Visual Studio 17 2022" -A x64 -DIS_STATIC=%STATIC_LINKING%
if %errorlevel% neq 0 exit /b 
cmake --build %BUILD_DIR_BASE%%BUILD_DIR% --config %BUILD_TYPE% --parallel
goto :endBuild

:desktop

echo Building %BUILD_TYPE% configuration...

cmake -S . -B%BUILD_DIR_BASE%%BUILD_DIR% -G "MinGW Makefiles" -DIS_STATIC=%STATIC_LINKING%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% 
if %errorlevel% neq 0 exit /b 

cmake --build %BUILD_DIR_BASE%%BUILD_DIR% --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b 

:endBuild