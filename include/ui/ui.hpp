#pragma once

#include "raylib.h"
#include <string>
#include "core/structs.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

using Buttons=std::vector<Button>;
using IndexMap=std::unordered_map<std::string, size_t>;

class ILayer;

class MyUI
{
public:
    MyUI();

    Button &getButton(std::string name);
    void addButton(std::string name, Button &button, ILayer *layer);
    const Buttons& getButtons() {return m_buttons;}
private:
    Buttons m_buttons;
    IndexMap m_buttonMap;   
};
