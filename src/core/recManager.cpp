#include "core/recManager.hpp"
#include "core/recManager.hpp"
#include "core/util.hpp"
#include "debug.hpp"

static bool MDBG = true;
//TODO: remove exceptions, add return bools

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
}

void RecourceManager::init()
{
    m_fonts["default"] = ::GetFontDefault();
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
    fwk::D_Pr("\nDBG loading texture: ", name + " path: " + filepath, MDBG);
    if (m_textures.count(name) != 0)
    {
        fwk::D_Pr("\nWRN Texture is already loaded, texture: ", name);
        return false;
    }
    Texture2D newTexture = ::LoadTexture(filepath.c_str());
    if (!::IsTextureValid(newTexture)) 
    {
        fwk::D_Pr("\nWRN Failed to load texture: " + name +  " path: " + filepath);
        return false;
    }
    m_textures[name] = newTexture;
    return true;
}   

bool RecourceManager::unloadTexture(std::string name)
{
    fwk::D_Pr("\nDBG Unloading texture: ", name , MDBG);
    if (m_textures.count(name) == 0) 
    {
        fwk::D_Pr("\nWRN Failed no unload cause: no texture: " + name);
        return false;
    }
    ::UnloadTexture(m_textures[name]);
    m_textures.erase(name);
    return true;
}

void RecourceManager::loadFont(std::string name, std::string filepath)
{
    fwk::D_Pr("\nDBG loading Font: ", name + " path: " + filepath, MDBG);
    if (m_fonts.count(name) != 0)
    {
        fwk::D_Pr("\nWRN Font is already loaded, font: ", name);
        return;
    }
    Font newFont = ::LoadFontEx(filepath.c_str(), 32*4, nullptr, 0);
    ::SetTextureFilter(newFont.texture, ::TEXTURE_FILTER_BILINEAR);
    // Font newFont = ::LoadFont(filepath.c_str());

    if (!::IsFontValid(newFont))
    {
        fwk::D_Pr("\nWRN Failed to load font: " + name +  " path: " + filepath);
        return;
    }
    m_fonts[name] = newFont;

}
Font RecourceManager::getFont(std::string name)
{
    if (m_fonts.count(name) == 0) 
    {
        fwk::D_Pr("\nWRN Font not found:", name, MDBG);
        return m_fonts["default"];
    }
    return m_fonts[name];
}

Texture2D RecourceManager::getTexture(std::string name)
{
    if (m_failed_textures.count(name) != 0)  //this texture not found
    {
        return getTexture("default");
    }

    if (m_textures.count(name) == 0) 
    {
       lazyLoad(name);
    }
    return m_textures[name];

}

bool RecourceManager::lazyLoad(std::string name)
{
    fwk::D_Pr("\nDBG LazyLoading texture: ", name , MDBG);
    bool success = loadTexture(name);
    if (!success)
    {
        if (name == "default") 
        {
            fwk::D_Pr("\nWRN Default texture is missing");
            exit(-1);
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
