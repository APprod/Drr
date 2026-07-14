#include "core/platform.hpp"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace platform {

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
    #ifdef _WIN32
        result.append("/desktop/");
    #endif
    #ifdef PLATFORM_WEB 
        result.append("/web/");
    #endif
    result.append(shaderName);
    return result;
}


}

