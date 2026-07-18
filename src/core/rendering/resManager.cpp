#include "core/rendering/resManager.hpp"
#include "core/utils/util.hpp"
#include "core/utils/log.hpp"
#include "core/platform.hpp"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    auto keys = getKeys(m_textures);
    for (auto key: keys)
    {
        unloadTexture(key);
    }
    for (auto& [name, fontMap] : m_fonts)
    {
        for (auto& [size, font] : fontMap)
        {
            ::UnloadFont(font);
        }
    }
}



void ResourceManager::init()
{
    FontMap defFonts;
    defFonts[10] = ::GetFontDefault();
    m_fonts["default"] = defFonts;

    

    loadShader("brightness", platform::getShaderPath("brightness.fs"),
                            {{"brightness",{1.0f}}});
    loadShader(
    "processing",
    platform::getShaderPath("processing.fs"),
    {
        {"brightness", 1.0f},
        {"contrast",   1.0f},
        {"saturation", 1.0f},
        {"gamma",      1.0f},
        {"tint",       Vector3{1.0f, 1.0f, 1.0f}},
        {"alpha",      1.0f}
    }
);
    loadShader("vignette",
        platform::getShaderPath("vignette.fs"),
        {
            {"vignetteIntensity", 0.5f},
            {"vignetteRoundness", 0.5f},
            {"vignetteSoftness",  0.5f},
        }
    );
}

void ResourceManager::load()
{
    this->loadFont("TNR", "assets/TNR/timesnewromanpsmt.ttf");
    loadTextures();
}

bool ResourceManager::loadTexture(std::string name)
{
    std::string path = ::GetWorkingDirectory();
    path = path + "/assets/" + name + ".png";
    return loadTexture(name, path);
    
}

bool ResourceManager::loadTexture(std::string name, std::string filepath)
{
    mylog::GetLogger().Info("loading texture: ", name + " path: " + filepath);
    if (m_textures.count(name) != 0)
    {
        mylog::GetLogger().Warn("Texture is already loaded, texture: ", name);
        return false;
    }
    Texture2D newTexture = ::LoadTexture(filepath.c_str());
    if (!::IsTextureValid(newTexture)) 
    {
        mylog::GetLogger().Error("Failed to load texture: " + name +  " path: " + filepath);
        return false;
    }
    m_textures[name] = newTexture;
    return true;
}   

bool ResourceManager::unloadTexture(std::string name)
{
    mylog::GetLogger().Info("Unloading texture: ", name);
    if (m_textures.count(name) == 0) 
    {
        mylog::GetLogger().Warn("Failed no unload cause: no texture: " + name);
        return false;
    }
    ::UnloadTexture(m_textures[name]);
    m_textures.erase(name);
    return true;
}

void ResourceManager::loadFont(std::string name, std::string filepath, std::vector<int> fontSizes)
{
    mylog::GetLogger().Info("loading Font: ", name, " path: ", filepath, " sizes: ", fontSizes);
    if (!::FileExists(filepath.c_str())){
        mylog::GetLogger().Error("Path Does Not exist (loading Font: ", name, " path: ", filepath, ")");
        return;
    }
    auto font = m_fonts.find(name);
    if (font == m_fonts.end()){ 
        // Not found, load All
        FontMap newFontMap;
        auto anyLoaded = false;
        for (auto size: fontSizes){
            Font newFont = ::LoadFontEx(filepath.c_str(), size, nullptr, 0);
            ::SetTextureFilter(newFont.texture, ::TEXTURE_FILTER_BILINEAR);
            if (!::IsFontValid(newFont)){
                mylog::GetLogger().Error("Failed to load font: " + name +  " path: " + filepath, "size: ", size);
                continue;
            }
            anyLoaded = true;
            newFontMap[size] = newFont;
        }
        if (anyLoaded) {
            m_fonts[name] = std::move(newFontMap);
        }
        else{
            mylog::GetLogger().Error("Failed to load font for all sizes: " + name +  " path: " + filepath);
        }
    }else{
        auto& fontMap = font->second;
        for (auto size: fontSizes){
            auto pos = fontMap.find(size);
            if (pos == fontMap.end()){
                //Not found, load
                Font newFont = ::LoadFontEx(filepath.c_str(), size, nullptr, 0);
                ::SetTextureFilter(newFont.texture, ::TEXTURE_FILTER_TRILINEAR);
                if (!::IsFontValid(newFont)){
                    mylog::GetLogger().Error("Failed to load font: " + name +  " path: " + filepath, "size: ", size);
                    continue;
                }
                fontMap[size] = newFont;
            }else{
                mylog::GetLogger().DebugInfo("Font already loaded: ", name, " , size: ", size);
            }
        }
    }
}

std::vector<std::string > ResourceManager::getLoadedFonts(){
    return getKeys(m_fonts);
}

Font ResourceManager::getFont(std::string name, int fontSize)
{
    if (name == "default") {
        return m_fonts.at("default").begin()->second;
    }

    auto fontIter = m_fonts.find(name);
    if (fontIter == m_fonts.end()) 
    {
        mylog::GetLogger().Error("Font not found:", name);
        return getFont("default", fontSize);
    }else{
        auto& fontMap = fontIter->second;
        auto fontSizeIter = fontMap.find(fontSize);
        if (fontSizeIter == fontMap.end()){
            mylog::GetLogger().Error("Font size not found: ", fontSize, " for font: ", name);
            return getFont("default", fontSize);
        }else{
            return fontSizeIter->second;
        }
    }
}

Texture2D ResourceManager::getTexture(std::string name)
{
    if (m_failed_textures.count(name) != 0)  //this texture not found
    {
        return getTexture("default");
    }

    if (m_textures.count(name) == 0) 
    {
        bool ok = lazyLoad(name);
        if (!ok){
            return getTexture("default");
        }
    }
    return m_textures[name];
}

bool ResourceManager::lazyLoad(std::string name)
{
    mylog::GetLogger().Info("LazyLoading texture: ", name);
    bool success = loadTexture(name);
    if (!success)
    {
        if (name == "default") 
        {
            mylog::GetLogger().Fatal("Default texture is missing");
            throw std::runtime_error("Default Texture missing, Can't fallback on texture loading failure");
        } // not found(default)s

        m_failed_textures.insert(name); 
        return false;
    }
    return true;
}

void ResourceManager::loadTextures()
{
    loadTexture("default");
    loadTexture("button_default");
    loadTexture("button_wide");
    loadTexture("menu");
}

void ResourceManager::loadShader(std::string shaderName, std::string filepath, 
    const Uniforms& uniforms)
{
    mylog::GetLogger().DebugInfo("Loading shader: ", filepath);
    ShaderProgram program;
    program.defaults = uniforms;
    auto shader = ::LoadShader(nullptr,filepath.c_str());
    if (!::IsShaderValid(shader)){
        mylog::GetLogger().Fatal("Failder to load shader, can't execute future shader draw calls");
        throw std::runtime_error("Failder to load shader, can't execute future shader draw calls");
    }
    program.shader = shader;
    for (auto& [name, val]: uniforms){
        auto loc = ::GetShaderLocation(shader, name.c_str());
        if (loc == -1){
            mylog::GetLogger().Warn("Shader uniform not found: ", name);
        }else{
            program.locations.emplace(name, loc);
        }
    }
    m_shaders[shaderName] = program;
}
ShaderProgram& ResourceManager::getShaderProgram(std::string name){
    auto iter = m_shaders.find(name);
    if (iter == m_shaders.end()){
        mylog::GetLogger().Fatal("Couldn't find shader, can't execute future shader draw calls");
        throw std::runtime_error("Couldn't find shader   , can't execute future shader draw calls");
    }
    return iter->second;
}