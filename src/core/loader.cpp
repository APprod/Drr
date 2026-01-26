#include "core/loader.hpp"
#include <fstream>
#include <iostream>
#include "raylib.h"

#define NLOHMANN_JSON_IMPL_INCLUDED

#include "core/json.hpp"

using json = nlohmann::json;


#define getFLOAT(name) newButton.name = button[#name].get<float>();
#define getSTRING(name) newButton.name = button[#name].get<std::string>();
#define getV2(name) newButton.name.x = button[#name][0].get<float>(); newButton.name.y = button[#name][1].get<float>();
#define getColor(name) newButton.name.r = button[#name][0].get<float>();\
        newButton.name.g = button[#name][1].get<float>();\
        newButton.name.b = button[#name][2].get<float>();\
        newButton.name.a = button[#name][3].get<float>();\

std::unordered_map<std::string, Button> Loader::getButtons(std::string path)
{

    std::unordered_map<std::string, Button> result;

    std::ifstream f(path);
    json data = json::parse(f);
    
    auto buttonData = data["buttons"];
    Button newButton;
    for (auto &[key, button]: buttonData.items())
    {
        // std::cerr << button.dump() << std::endl;
        getSTRING(message)
        getSTRING(textureName)
        getSTRING(fontName)
        getV2(pos)
        getV2(size)
        getColor(textTint)
        getFLOAT(textSize);
        
        result[key] = newButton;
    }
    f.close();
    return result;
}

Loader::Loader()
{
}

Loader::~Loader()
{
}
