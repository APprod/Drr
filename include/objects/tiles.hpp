#pragma once
#include <vector>
#include <string>
#include "raylib.h"
#include "core/structs.hpp"
#include "objects/city.hpp"



namespace Bioms
{
   
} // namespace Bioms


class BiomeInstance
{
public:
    std::vector<Vector2> positions; //closed list
    Rectangle bounds; // for optimisation
    std::string type;
    int id; // for tiles sharing same bioms
};

class Tile 
{
public:
    static const uint32_t tileSize = 512u;
    static const uint32_t tileXCount = 20u;
    static const uint32_t tileYCount = 10u;

    std::vector<BiomeInstance> bioms;
    std::vector<City> cities;

    Ivec2 pos;

private:

};