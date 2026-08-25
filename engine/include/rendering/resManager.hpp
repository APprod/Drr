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

    Texture2D getTexture(const std::string& name);
    bool loadTexture(std::string name);
    bool loadTexture(std::string name, std::string filepath);
    bool unloadTexture(std::string name); 

    // Registers a TTF under a logical name and bakes one atlas per ladder size (default 20 sizes).
    // First call for the name loads all listed sizes (bilinear); repeat calls only add missing sizes (trilinear).
    void loadFont(std::string name, std::string filepath, std::vector<int> fontSizes = {12,14,16,18,20,22,24,26,28,32,36,40,44,48,52,56,60,64,72,80,88,96});
    void loadFontSizes(const std::string& name, const std::vector<int>& fontSizes);
    // Non-owning handle lookup by (name, exact px size); never returns an invalid font —
    // unknown name/size logs Error and falls back to the raylib default font.
    // "default" ignores fontSize and returns the single GetFontDefault() bake registered in init().
    Font getFont(const std::string& name, int fontSize);
    // Names of all registered font families.
    std::vector<std::string > getLoadedFonts();

    void loadShader(std::string name, std::string filepath, const Uniforms& uniforms);
    ShaderProgram& getShaderProgram(const std::string& name);

    std::optional<SliceMargins> getSliceData(const std::string& name) const;
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