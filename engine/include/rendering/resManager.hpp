#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "raylib.h"
#include "rendering/shader.hpp"
#include "rendering/textureSpec.hpp"

// Baked atlas sizes for one font family: px size -> Font. Owned by ResourceManager.
using FontMap = std::unordered_map<int, Font>;
struct FontEntry{
    FontMap map;
    std::string filepath;
};

class ResourceManager
{
public:
    ResourceManager();
    void init();
    void load();
    ~ResourceManager();

    // Retrieve texture. Lazy loads if the texture isnt loaded yet. (<name> -> assets/<name>.jpg), fallback to texture "default"
    Texture2D getTexture(const std::string& name);
    // Load texture. name -> assets/name.jps as path
    bool loadTexture(std::string name);
    // Load texture from specified path
    bool loadTexture(std::string name, std::string filepath);
    bool unloadTexture(std::string name); 

    // loads font and stores under <name> key for each font every size from fontSizes is loaded
    void loadFont(std::string name, std::string filepath, std::vector<int> fontSizes = {12,14,16,18,20,22,24,26,28,32,36,40,44,48,52,56,60,64,72,80,88,96});
    // Load additional sizes for a font under <name> font path is restored and dont have to be provided
    void loadFontSizes(const std::string& name, const std::vector<int>& fontSizes);
    // Retrieve font under specified name and size. Fallbacks to default raylib font if name+size isnt found
    Font getFont(const std::string& name, int fontSize);
    // Names of all registered font families.
    std::vector<std::string > getLoadedFonts();

    // loads shader, saves under <name>, sets defaults to <uniforms>, fallbacks to default raylib shader with warning 
    void loadShader(std::string name, std::string filepath, const Uniforms& uniforms);
    // get already loaded shader
    ShaderProgram& getShaderProgram(const std::string& name);

    // returns slice data for specified texture
    std::optional<SliceMargins> getSliceData(const std::string& name) const;
    // sets NPatch data source to <path> and replaces current one with it
    void loadNPatchData(std::string path);
private:
    bool lazyLoad(std::string name);
    void loadTextures();
    // Bakes one atlas of ASCII + Cyrillic glyphs at the given px size (LoadFontEx) and sets its filter.
    Font loadFontAtSize(const std::string& filepath, int size, TextureFilter filter);
    std::unordered_map<std::string, Texture2D> m_textures;
    std::unordered_set<std::string> m_failed_textures;

    std::unordered_map<std::string, FontEntry> m_fonts;
    std::unordered_map<std::string, ShaderProgram> m_shaders;
    std::unordered_map<std::string, SliceMargins> m_nPatchMap;
};