@echo off


set "BUILD_TYPE=Debug"
set "STATIC_LINKING=ON"
set "BUILD_DIR=_build\Debug"

:parse
if "%1"=="" goto :endparse
if "%1"=="release" set "BUILD_TYPE=Release" & set "BUILD_DIR=_build\Release" & shift & goto :parse
if "%1"=="debug" set "BUILD_TYPE=Debug" & set "BUILD_DIR=_build\Debug" & shift & goto :parse
if "%1"=="static" set "STATIC_LINKING=ON" & shift & goto :parse
if "%1"=="dynamic" set "STATIC_LINKING=OFF" & shift & goto :parse
:endparse

echo on

echo Building %BUILD_TYPE% configuration...

cmake -S . -B%BUILD_DIR% -G "MinGW Makefiles" -DIS_STATIC=%STATIC_LINKING%  -DCMAKE_BUILD_TYPE=%BUILD_TYPE% 
if %errorlevel% neq 0 exit /b 

cmake --build %BUILD_DIR% --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b 

