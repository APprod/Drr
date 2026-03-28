@echo on


set varstatic=%1
set res=
if "%varstatic%"=="on" set res=-DIS_STATIC=ON

set BUILD_TYPE=Debug
if NOT "%2"=="" set BUILD_TYPE=%2

echo Building %BUILD_TYPE% configuration...

cmake -H. -B_build -G "MinGW Makefiles" %res% -DCMAKE_BUILD_TYPE=Debug 
if %errorlevel% neq 0 exit /b 

cmake --build _build --config %BUILD_TYPE% --parallel
if %errorlevel% neq 0 exit /b 

xcopy "assets" "_build/assets/" /s /e /h /I /Y
if %errorlevel% neq 0 exit /b 

