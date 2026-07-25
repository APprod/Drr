#include "ui/uiTheme.hpp"
#include "services.hpp"
#include "utils/log.hpp"

#include <algorithm>

UITheme::UITheme() {
    m_fontSizes = {12, 14, 16, 18, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96};
    m_roles["default"] = {0};
    m_roles["caption"] = {-1};
    m_roles["button"]  = {2};
    m_roles["header"]  = {2};
    m_roles["big header"]  = {4};
    m_roles["text"]    = {0};
}

void UITheme::setFontSizes(std::vector<int> sizes) {
    m_fontSizes = std::move(sizes);
}

void UITheme::setCurrentSizeIndex(int index) {
    m_currentSizeIndex = index;
}

void UITheme::setRole(const std::string& role, RoleDef def) {
    m_roles[role] = def;
}

RoleDef UITheme::getRole(const std::string& role) const {
    auto it = m_roles.find(role);
    if (it == m_roles.end()) {
        mylog::GetLogger().Warn("UITheme: role not found \"", role, "\", falling back to \"default\"");
        auto defIt = m_roles.find("default");
        if (defIt == m_roles.end()) {
            mylog::GetLogger().Warn("UITheme: \"default\" role not found either, using fallback");
            return {0};
        }
        return defIt->second;
    }
    return it->second;
}

Font UITheme::resolveFont(const std::string& role, int delta) const {
    auto def = getRole(role);

    if (m_fontSizes.empty()) {
        mylog::GetLogger().Warn("UITheme: fontSizes is empty, using  fallback");
        auto& rm = GetServices().recManager;
        return rm.getFont(m_fontName, 20);
    }

    int finalIndex = std::clamp(m_currentSizeIndex + def.sizeOffset + delta, 0,
                                static_cast<int>(m_fontSizes.size()) - 1);
    int size = m_fontSizes[finalIndex];

    auto& rm = GetServices().recManager;
    return rm.getFont(m_fontName, size);
}

std::string UITheme::resolveFontName(const std::string&) const {
    return m_fontName;
}

int UITheme::resolveSize(const std::string& role, int delta) const {
    auto def = getRole(role);

    if (m_fontSizes.empty()) {
        mylog::GetLogger().Warn("UITheme: fontSizes is empty, using fallback");
        return 20;
    }

    int finalIndex = std::clamp(m_currentSizeIndex + def.sizeOffset + delta, 0,
                                static_cast<int>(m_fontSizes.size()) - 1);
    return m_fontSizes[finalIndex];
}
