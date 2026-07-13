#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "raylib.h"
#include "core/shader.hpp"

using FontMap = std::unordered_map<int, Font>;

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

    void loadFont(std::string name, std::string filepath, std::vector<int> fontSizes = {12,14,16,20,26,32,40,48,56,64,128});
    Font getFont(std::string name, int fontSize);

    void loadShader(std::string name, std::string filepath, const Uniforms& uniforms);
    ShaderProgram& getShaderProgram(std::string name);

    // Loader loader;
private:
    bool lazyLoad(std::string name);
    void loadTextures();
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_set<std::string> m_failed_textures;

    std::unordered_map<std::string, FontMap> m_fonts;
    std::unordered_map<std::string, ShaderProgram> m_shaders;
};