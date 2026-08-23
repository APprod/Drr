#include "ui/uiTheme.hpp"
#include "services.hpp"
#include "utils/log.hpp"

#include <algorithm>

UITheme::UITheme() {
    registerSizeRole("default", 0);
    registerFontRole("default", "default", 2);
    setSizes({12,14,16,18,20,22,24,26,28,32,36,40,44,48,52,56,60,64,72,80,88,96});
}

int UITheme::resolveIdx(SizeRoleId roleId, int sizeDelta) const {
    int offset = m_sizeRoles[roleId];
    int idx = offset + selectedSizeIdx + sizeDelta;
    myClamp(idx, 0, static_cast<int>(m_fontSizes.size() - 1));
    return idx;
}

int UITheme::resolveSize(SizeRoleId roleId, int sizeDelta) const {
    return m_fontSizes[resolveIdx(roleId,sizeDelta)];
}

//// WE have registering new sizeroles and font roles ////
SizeRoleId UITheme::registerSizeRole(std::string roleName, SizeIdx sizeOffset){ // Adds new pair - Id - sizeRole (insert-or-get)
    auto existing = m_sizesMap.find(roleName);
    if (existing != m_sizesMap.end()){
        m_sizeRoles[existing->second] = sizeOffset;
        m_cacheVersion++;
        return existing->second;
    }
    auto Id = static_cast<SizeRoleId>(m_sizeRoles.size());
    m_sizesMap[roleName] = Id;
    m_sizeRoles.push_back(sizeOffset);
    return Id;
}

FontRoleId UITheme::registerFontRole(std::string roleName, std::string fontName, float spacing){ // Adds under that role name its Data (insert-or-get)
    auto existing = m_fontsMap.find(roleName);
    if (existing != m_fontsMap.end()){
        m_fontRoles[existing->second] = FontRole{fontName, std::vector<Font>(m_fontSizes.size()), spacing};
        m_cacheVersion++;
        return existing->second;
    }
    auto Id = static_cast<FontRoleId>(m_fontRoles.size());
    m_fontsMap[roleName] = Id;
    m_fontRoles.push_back(FontRole{fontName,std::vector<Font>(m_fontSizes.size()),spacing});
    return Id;
}

//// Will be used to set Ids when creating UI ( once per Scene layout creation) ////
SizeRoleId UITheme::getSizeRole(std::string roleName){
    auto Id = m_sizesMap.find(roleName);
    if (Id == m_sizesMap.end()) return 0;
    return Id->second;
}

FontRoleId UITheme::getFontRole(std::string roleName){
    auto Id = m_fontsMap.find(roleName);
    if (Id == m_fontsMap.end()) return 0;
    return Id->second;
}

//// Dynamically changing retrieving data ////
bool UITheme::setFontRole(std::string roleName, std::string fontName, float spacing){
    auto Id = m_fontsMap.find(roleName);
    if (Id == m_fontsMap.end()) return false;
    m_fontRoles[Id->second] = FontRole{fontName,std::vector<Font>(m_fontSizes.size()),spacing};
    m_cacheVersion++;
    return true;
}

bool UITheme::setSizeRole(std::string roleName, SizeIdx role){
    auto Id = m_sizesMap.find(roleName);
    if (Id == m_sizesMap.end()) return false;
    m_sizeRoles[Id->second] = role;
    m_cacheVersion++;
    return true;
}

void UITheme::setSelectedSize(SizeIdx selected){
    selectedSizeIdx = selected;
    myClamp(selectedSizeIdx, 0, static_cast<int>(m_fontSizes.size() - 1));
    m_cacheVersion++;
}

void UITheme::setSizes(std::vector<int> sizes){
    if (sizes == m_fontSizes) return; // identical ladder — keep baked slots
    myClamp(selectedSizeIdx, 0, static_cast<int>(sizes.size() - 1));
    for (auto& role : m_fontRoles){
        role.fonts.clear();
        role.fonts.resize(sizes.size());
    }
    m_fontSizes = std::move(sizes);
    m_cacheVersion++;
}

//Returns bool if cache is different + auto updates
bool UITheme::needsUpdate(uint64_t& consumerCacheVersion){
    if (consumerCacheVersion != m_cacheVersion){
        consumerCacheVersion = m_cacheVersion;
        return true;
    }
    return false;
}

const std::string& UITheme::getFontName(FontRoleId id) const {
    static const std::string empty;
    if (id < 0 || id >= static_cast<int>(m_fontRoles.size())) return empty;
    return m_fontRoles[id].fontName;
}

void UITheme::warmup(){
    for (auto& role : m_fontRoles)
        GetServices().resManager.loadFontSizes(role.fontName, m_fontSizes);
}

FontData UITheme::getFont(FontSpec spec, int sizeDelta){
    myClamp(spec.sizeRole, 0, static_cast<int>(m_sizeRoles.size()) - 1);
    myClamp(spec.fontRole, 0, static_cast<int>(m_fontRoles.size()) - 1);
    int sizeIdx = resolveIdx(spec.sizeRole, sizeDelta);
    float size = static_cast<float>(m_fontSizes[sizeIdx]);
    FontRole& fontRole = m_fontRoles[spec.fontRole];
    auto& fonts = fontRole.fonts;
    if (fonts.size() < m_fontSizes.size()) fonts.resize(m_fontSizes.size());
    Font& font = fonts[sizeIdx];
    if (!IsFontValid(font)){
        auto& rm = GetServices().resManager;
        rm.loadFontSizes(fontRole.fontName, {static_cast<int>(size)});
        font = rm.getFont(fontRole.fontName, static_cast<int>(size));
    }
    return FontData{
        .font = font,
        .spacing = fontRole.spacing,
        .size = size
    };
}
