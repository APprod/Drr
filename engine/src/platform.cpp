#include "platform.hpp"

#ifdef _WIN32
// Exclusion macros keep windows.h from clashing with raylib declarations
// (Rectangle, CloseWindow, ShowCursor) in TUs that include both
#define WIN32_LEAN_AND_MEAN
#ifndef NOGDI
#define NOGDI
#endif
#ifndef NOUSER
#define NOUSER
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace platform {

void SetupConsole()
{
#ifdef _WIN32
    // Console interprets our narrow UTF-8 log output correctly (Cyrillic etc.)
    SetConsoleOutputCP(CP_UTF8);
#endif
}

std::string CurrentDateTime()
{
    using clock = std::chrono::system_clock;

    auto now = clock::now();
    std::time_t tt = clock::to_time_t(now);

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string getShaderPath(std::string shaderName){
    std::string result{"assets/shaders"};
    #ifdef PLATFORM_WEB
        result.append("/web/");
    #else
        result.append("/desktop/");
    #endif
    result.append(shaderName);
    return result;
}


}

