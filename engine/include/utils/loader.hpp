#pragma once
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace loader {

class Loader {
public:
    Loader() = default;

    nlohmann::json loadJson(const std::string& filepath);

    template<typename T>
    T loadAs(const std::string& filepath) {
        return loadJson(filepath).get<T>();
    }

    template<typename T>
    std::unordered_map<std::string, T> loadMap(const std::string& filepath) {
        auto json = loadJson(filepath);
        std::unordered_map<std::string, T> result;
        for (auto& [key, value] : json.items())
            result[key] = value.get<T>();
        return result;
    }
};

}
