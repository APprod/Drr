#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "raylib.h"
// #include "loader.hpp"

class RecourceManager
{
public:
    RecourceManager();
    void init();
    void load();
    ~RecourceManager();

    Texture2D getTexture(std::string name);
    bool loadTexture(std::string name);
    bool loadTexture(std::string name, std::string filepath);
    bool unloadTexture(std::string name); 

    void loadFont(std::string name, std::string filepath);
    Font getFont(std::string name);

    // Loader loader;
private:
    bool lazyLoad(std::string name);
    void loadTextures();
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_set<std::string> m_failed_textures;

    std::unordered_map<std::string, ::Font> m_fonts;
    
};