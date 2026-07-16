#include "core/recManager.hpp"
#include "core/util.hpp"
#include "core/debug.hpp"
#include "core/platform.hpp"


RecourceManager::RecourceManager()
{
}

RecourceManager::~RecourceManager()
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



void RecourceManager::init()
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

void RecourceManager::load()
{
    this->loadFont("TNR", "assets/TNR/timesnewromanpsmt.ttf");
    loadTextures();
}

bool RecourceManager::loadTexture(std::string name)
{
    std::string path = ::GetWorkingDirectory();
    path = path + "/assets/" + name + ".png";
    return loadTexture(name, path);
    
}

bool RecourceManager::loadTexture(std::string name, std::string filepath)
{
    dbg::GetLogger().Info("loading texture: ", name + " path: " + filepath);
    if (m_textures.count(name) != 0)
    {
        dbg::GetLogger().Warn("Texture is already loaded, texture: ", name);
        return false;
    }
    Texture2D newTexture = ::LoadTexture(filepath.c_str());
    if (!::IsTextureValid(newTexture)) 
    {
        dbg::GetLogger().Error("Failed to load texture: " + name +  " path: " + filepath);
        return false;
    }
    m_textures[name] = newTexture;
    return true;
}   

bool RecourceManager::unloadTexture(std::string name)
{
    dbg::GetLogger().Info("Unloading texture: ", name);
    if (m_textures.count(name) == 0) 
    {
        dbg::GetLogger().Warn("Failed no unload cause: no texture: " + name);
        return false;
    }
    ::UnloadTexture(m_textures[name]);
    m_textures.erase(name);
    return true;
}

void RecourceManager::loadFont(std::string name, std::string filepath, std::vector<int> fontSizes)
{
    dbg::GetLogger().Info("loading Font: ", name, " path: ", filepath, " sizes: ", fontSizes);
    if (!::FileExists(filepath.c_str())){
        dbg::GetLogger().Error("Path Does Not exist (loading Font: ", name, " path: ", filepath, ")");
        return;
    }
    auto font = m_fonts.find(name);
    if (font == m_fonts.end()){ 
        // Not found, load All
        FontMap newFontMap;
        auto success = false;
        for (auto size: fontSizes){
            Font newFont = ::LoadFontEx(filepath.c_str(), size, nullptr, 0);
            ::SetTextureFilter(newFont.texture, ::TEXTURE_FILTER_BILINEAR);
            if (!::IsFontValid(newFont))
            {
                dbg::GetLogger().Error("Failed to load font: " + name +  " path: " + filepath, "size: ", size);
                continue;
            }
            success = true;
            newFontMap[size] = newFont;
        }
        if (success) {
            m_fonts[name] = std::move(newFontMap);
        }
        else{
            dbg::GetLogger().Error("Failed to load font for all sizes: " + name +  " path: " + filepath);
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
                    dbg::GetLogger().Error("Failed to load font: " + name +  " path: " + filepath, "size: ", size);
                    continue;
                }
                fontMap[size] = newFont;
            }else{
                dbg::GetLogger().Info("Font already loaded: ", name, " , size: ", size);
            }
        }
    }
}

std::vector<std::string > RecourceManager::getLoadedFonts(){
    return getKeys(m_fonts);
}

Font RecourceManager::getFont(std::string name, int fontSize)
{
    auto fontIter = m_fonts.find(name);
    if (fontIter == m_fonts.end()) 
    {
        dbg::GetLogger().Error("Font not found:", name);
        return m_fonts.at("default")[32];
    }else{
        auto& fontMap = fontIter->second;
        auto fontSizeIter = fontMap.find(fontSize);
        if (fontSizeIter == fontMap.end()){
            dbg::GetLogger().Error("Font size not found: ", fontSize);
            return m_fonts.at("default")[32];
        }else{
            return fontSizeIter->second;
        }
    }
}

Texture2D RecourceManager::getTexture(std::string name)
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

bool RecourceManager::lazyLoad(std::string name)
{
    dbg::GetLogger().Info("LazyLoading texture: ", name);
    bool success = loadTexture(name);
    if (!success)
    {
        if (name == "default") 
        {
            dbg::GetLogger().Fatal("Default texture is missing");
            throw std::runtime_error("Default Texture missing, Can't fallback on texture loading failure");
        } // not found(default)s

        m_failed_textures.insert(name); 
        return false;
    }
    return true;
}

void RecourceManager::loadTextures()
{
    loadTexture("default");
    loadTexture("button_default");
    loadTexture("button_wide");
    loadTexture("menu");
}

void RecourceManager::loadShader(std::string shaderName, std::string filepath, 
    const Uniforms& uniforms)
{
    dbg::GetLogger().DebugInfo("Loading shader: ", filepath);
    ShaderProgram program;
    program.defaults = uniforms;
    auto shader = ::LoadShader(nullptr,filepath.c_str());
    if (!::IsShaderValid(shader)){
        dbg::GetLogger().Fatal("Failder to load shader, can't execute future shader draw calls");
        throw std::runtime_error("Failder to load shader, can't execute future shader draw calls");
    }
    program.shader = shader;
    for (auto& [name, val]: uniforms){
        auto loc = ::GetShaderLocation(shader, name.c_str());
        if (loc == -1){
            dbg::GetLogger().Warn("Shader uniform not found: ", name);
        }else{
            program.locations.emplace(name, loc);
        }
    }
    m_shaders[shaderName] = program;
}
ShaderProgram& RecourceManager::getShaderProgram(std::string name){
    auto iter = m_shaders.find(name);
    if (iter == m_shaders.end()){
        dbg::GetLogger().Fatal("Couldn't find shader, can't execute future shader draw calls");
        throw std::runtime_error("Couldn't find shader   , can't execute future shader draw calls");
    }
    return iter->second;
}