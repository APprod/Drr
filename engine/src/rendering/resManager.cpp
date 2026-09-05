#include "rendering/resManager.hpp"
#include "services.hpp"
#include "utils/util.hpp"
#include "utils/log.hpp"
#include "platform.hpp"
#include "threading/threadPool.hpp"

namespace {
const std::vector<int>& fontCodepoints()
{
    static const std::vector<int> cps = []{
        std::vector<int> v;
        v.reserve(95 + 256);
        for (int c = 32; c <= 126; ++c) v.push_back(c);
        for (int c = 0x0400; c <= 0x04FF; ++c) v.push_back(c);
        return v;
    }();
    return cps;
}
}

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
        for (auto& [size, font] : fontMap.map)
        {
            ::UnloadFont(font);
        }
    }
}



void ResourceManager::init()
{
    //Default for every project
    FontMap defFonts;
    defFonts[10] = ::GetFontDefault();
    m_fonts["default"].map = defFonts;

    //Shaders required by the engine itself: Scene::OnDrawToScreen uses "processing",
    //Button hover animation uses "brightness"
    loadShader("brightness", platform::getShaderPath("brightness.fs"),
                            {{"brightness",{1.0f}}});
    loadShader("processing",
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
}

void ResourceManager::load()
{
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
    if (name == "default"){
        Texture2D newTexture = ::LoadTexture(filepath.c_str());
        if (!::IsTextureValid(newTexture)) 
        {
            mylog::GetLogger().Error("Failed to load texture: " + name +  " path: " + filepath);
            return false;
        }
        m_textures[name] = newTexture;
        return true;
    }
    loadTextureThreaded(name, filepath);
    m_textures[name] = getTexture("default");
    return true;
    // Texture2D newTexture = loadTextureThreaded(filepath);
    // if (!::IsTextureValid(newTexture)) 
    // {
    //     mylog::GetLogger().Error("Failed to load texture: " + name +  " path: " + filepath);
    //     return false;
    // }
    // m_textures[name] = newTexture;
    // return true;
}   

void ResourceManager::GPUUpload(std::string name, Image image, bool success){
    Tester test("GPUUpload texture: "  + name,0,true);
    Texture2D texture = LoadTextureFromImage(image);
    success &= IsTextureValid(texture);
    if (success){
        m_textures[name] = texture;
    }else{
        m_failed_textures.insert(name); 
    }
    UnloadImage(image);
}

void ResourceManager::loadTextureThreaded(std::string name, std::string filepath){
    GetThreadPool().Async(
        [this, name, filepath](){
            Tester test("CPU decode texture: " + name,0,true);
            Image decoded = LoadImage(filepath.c_str());
            bool ok = IsImageValid(decoded);
            GetThreadPool().SubmitToMain([this, name, ok, decoded](){
                this->GPUUpload(name, decoded, ok);
            });
        }
    );
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
            Font newFont = loadFontAtSize(filepath, size, TEXTURE_FILTER_BILINEAR);
            if (!::IsFontValid(newFont)){
                mylog::GetLogger().Error("Failed to load font: " + name +  " path: " + filepath, "size: ", size);
                continue;
            }
            anyLoaded = true;
            newFontMap[size] = newFont;
        }
        if (anyLoaded) {
            auto& fontEntry = m_fonts[name];
            fontEntry.map = std::move(newFontMap);
            fontEntry.filepath = filepath;
        }
        else{
            mylog::GetLogger().Error("Failed to load font for all sizes: " + name +  " path: " + filepath);
        }
    }else{
        font->second.filepath = filepath;
        loadFontSizes(name, fontSizes);
    }
}

void ResourceManager::loadFontSizes(const std::string& name, const std::vector<int>& fontSizes)
{
    auto font = m_fonts.find(name);
    if (font == m_fonts.end()){
        mylog::GetLogger().Warn("Font not found, can't load sizes: ", name);
        return;
    }
    auto& fontEntry = font->second;
    if (fontEntry.filepath.empty()){
        // e.g. the "default" raylib font: registered without a source file, cannot be rebaked
        mylog::GetLogger().DebugInfo("Font has no source path, sizes kept as-is: ", name);
        return;
    }
    mylog::GetLogger().Info("loading Font sizes: ", name, " path: ", fontEntry.filepath, " sizes: ", fontSizes);
    for (auto size: fontSizes){
        auto pos = fontEntry.map.find(size);
        if (pos == fontEntry.map.end()){
            //Not found, load
            Font newFont = loadFontAtSize(fontEntry.filepath, size, TEXTURE_FILTER_TRILINEAR);
            if (!::IsFontValid(newFont)){
                mylog::GetLogger().Error("Failed to load font: " + name +  " path: " + fontEntry.filepath, "size: ", size);
                continue;
            }
            fontEntry.map[size] = newFont;
        }else{
            mylog::GetLogger().DebugInfo("Font already loaded: ", name, " , size: ", size);
        }
    }
}

Font ResourceManager::loadFontAtSize(const std::string& filepath, int size, TextureFilter filter)
{
    const std::vector<int>& cps = fontCodepoints();
    Font font = ::LoadFontEx(filepath.c_str(), size,
                             const_cast<int*>(cps.data()), static_cast<int>(cps.size()));
    ::SetTextureFilter(font.texture, filter);
    return font;
}

std::vector<std::string > ResourceManager::getLoadedFonts(){
    return getKeys(m_fonts);
}

Font ResourceManager::getFont(const std::string& name, int fontSize)
{
    if (name == "default") {
        return m_fonts.at("default").map.begin()->second;
    }

    auto fontIter = m_fonts.find(name);
    if (fontIter == m_fonts.end()) 
    {
        mylog::GetLogger().Error("Font not found:", name);
        return getFont("default", fontSize);
    }else{
        auto& fontMap = fontIter->second;
        auto fontSizeIter = fontMap.map.find(fontSize);
        if (fontSizeIter == fontMap.map.end()){
            mylog::GetLogger().Error("Font size not found: ", fontSize, " for font: ", name);
            return getFont("default", fontSize);
        }else{
            return fontSizeIter->second;
        }
    }
}

Texture2D ResourceManager::getTexture(const std::string& name)
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

std::optional<SliceMargins> ResourceManager::getSliceData(const std::string& name) const {
    auto it = m_nPatchMap.find(name);
    if (it != m_nPatchMap.end()) return it->second;
    return std::nullopt;
}

void ResourceManager::loadNPatchData(std::string path)
{
    m_nPatchMap = GetServices().loader.loadMap<SliceMargins>(path);
}

void ResourceManager::loadTextures()
{
    loadTexture("default");
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
ShaderProgram& ResourceManager::getShaderProgram(const std::string& name){
    auto iter = m_shaders.find(name);
    if (iter == m_shaders.end()){
        mylog::GetLogger().Fatal("Couldn't find shader, can't execute future shader draw calls");
        throw std::runtime_error("Couldn't find shader   , can't execute future shader draw calls");
    }
    return iter->second;
}