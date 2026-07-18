#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "raylib.h"

struct RoleDef {
    int sizeOffset = 0;
};

class UITheme {
public:
    UITheme();
    void setFontSizes(std::vector<int> sizes);
    void setCurrentSizeIndex(int index);
    int  getCurrentSizeIndex() const { return m_currentSizeIndex; }

    void    setRole(const std::string& role, RoleDef def);
    RoleDef getRole(const std::string& role) const;

    Font resolveFont(const std::string& role, int delta = 0) const;
    int  resolveSize(const std::string& role, int delta = 0) const;
    std::string resolveFontName(const std::string& role) const;

    int m_currentSizeIndex{5};
    std::string m_fontName = "TNR";

private:
    std::vector<int> m_fontSizes;
    std::unordered_map<std::string, RoleDef> m_roles;
};
