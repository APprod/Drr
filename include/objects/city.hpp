#pragma once
#include "raylib.h"
#include <string>
#include <cstdint>

class City
{
public:
    City();
    ~City();
private:
    std::string name;   
    uint32_t tier;
    uint32_t id;
    Vector2 position;
    float radius;
};
