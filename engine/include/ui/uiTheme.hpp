#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include "raylib.h"
#include "utils/util.hpp"
#include "rendering/resManager.hpp"

using SizeRoleId = int;
using FontRoleId = int;
using SizeIdx = int; //Offset

struct FontRole{
    std::string fontName;
    std::vector<Font> fonts; // Idx -> sizeIdx
    float spacing{1};
};

struct FontData{
    Font font;
    float spacing;
    float size;
}; // This is what you get when You get a Font, everything resolved here

struct FontSpec{
    SizeRoleId sizeRole{0};
    FontRoleId fontRole{0}; //Has spacing included with it //Add default params maybe explicitly
};  

// Only default

class UITheme{
public:
    UITheme();
    FontData getFont(FontSpec spec, int sizeDelta = 0);
    // Pre-bakes every registered font at every ladder size; call once after RM.load() to avoid a first-draw hitch
    void warmup();
    int resolveIdx(SizeRoleId roleId, int sizeDelta = 0) const;
    int resolveSize(SizeRoleId roleId, int sizeDelta = 0) const;
    //// WE have registering new sizeroles and font roles ////
    SizeRoleId registerSizeRole(std::string roleName, SizeIdx sizeOffset); // Adds new pair - Id - sizeRole (insert-or-get)
    FontRoleId registerFontRole(std::string roleName, std::string fontName, float spacing); // Adds under that role name its Data (insert-or-get)
    //// Will be used to set Ids when creating UI ( once per Scene layout creation) ////
    SizeRoleId getSizeRole(std::string roleName);
    FontRoleId getFontRole(std::string roleName);
    //// Dynamically changing retrieving data ////
    bool setFontRole(std::string roleName, std::string fontName, float spacing);
    bool setSizeRole(std::string roleName, SizeIdx role);
    void setSelectedSize(SizeIdx selected);
    void setSizes(std::vector<int> sizes);
    //Returns bool if cache is different + auto updates
    bool needsUpdate(uint64_t& consumerCacheVersion);
    SizeIdx getSelectedSize() const { return selectedSizeIdx; }
    int getSizesCount() const { return static_cast<int>(m_fontSizes.size()); }
    const std::string& getFontName(FontRoleId id) const;
private:
    SizeIdx selectedSizeIdx{5};
    std::vector<int> m_fontSizes;
    std::vector<SizeIdx> m_sizeRoles;
    std::vector<FontRole> m_fontRoles;

    std::unordered_map<std::string, SizeRoleId> m_sizesMap;
    std::unordered_map<std::string, FontRoleId> m_fontsMap;
    uint64_t m_cacheVersion{0};
};


