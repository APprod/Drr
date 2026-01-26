echo on

rem rmdir /s /q "./_build"
rem if "%varstatic%" neq "on" del _build\CMakeCache.txt
set varstatic=%1

set res=
if "%varstatic%"=="on" set res=-DIS_STATIC=ON


cmake -H. -B_build  -G "MinGW Makefiles" %res% -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 exit /b 
cmake --build _build --parallel
if %errorlevel% neq 0 exit /b 
xcopy "assets" "_build/assets/" /s /e /h  /I /Y
if %errorlevel% neq 0 exit /b 
_build\app.exe