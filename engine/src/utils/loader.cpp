#include "utils/loader.hpp"
#include "utils/log.hpp"
#include <fstream>

namespace loader {

nlohmann::json Loader::loadJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        mylog::GetLogger().Error("Loader: cannot open file ", filepath);
        return nlohmann::json{};
    }
    try {
        return nlohmann::json::parse(file);
    } catch (const nlohmann::json::parse_error& e) {
        mylog::GetLogger().Error("Loader: JSON parse error in ", filepath, ": ", e.what());
        return nlohmann::json{};
    }
}

}
