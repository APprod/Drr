/* Platform dependant functions*/
#pragma once

#include <string>

namespace platform {
// Sets console output to utf 8 on windows
void SetupConsole();
std::string CurrentDateTime();
// Returns shader path with proper (web or desktop folder)
std::string getShaderPath(std::string shaderName);

}

