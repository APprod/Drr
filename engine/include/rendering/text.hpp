#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "raylib.h"
#include "ui/uiTheme.hpp"
namespace mytext
{
struct Word{
    std::string word;
    Vector2 size{0,0};
};

struct Line{
    std::vector<Word> words;
    Vector2 size{0,0};
    std::string lineView;
};
}
std::vector<mytext::Line> splitLines(std::string& text);

enum class TextVAlign { Top, Center, Bottom };

class Text {
public:
    Text() = default;
    // sizeRole and fontRole will be resolved through UI theme, size delta is additonal shift in sizes ladder adds up with resolved SizeRole
    Text(std::string text, std::string sizeRole = "default",
         std::string fontRole = "default", int sizeDelta = 0, Color color = RAYWHITE);

    void SetText(std::string t) { 
        m_text = std::move(t); 
        m_dirtyText = true;
    }
    const std::string& GetText() const { return m_text; }
    void SetVAlign(TextVAlign align) { m_valign = align; }
    TextVAlign GetVAlign() const { return m_valign; }

    // After ReMeasure: if text was changed - all m_mytext::Lines are recalculated
    // If text wasnt changed, only constrained are recalculated
    // Returns desired size;
    Vector2 ReMeasure(Vector2 borders);
    bool IsDirty() const { return m_dirtyFull; }
    void ClearDirty() { m_dirtyFull = false; }

    void Draw(Vector2 position);
    void DrawCentered(Rectangle bounds);

    void SetSizeDelta(int delta) { if (m_sizeDelta != delta){ m_sizeDelta = delta; m_dirtyText = true; } }
    int GetFontSize() const { return m_lastFontSize; }
    const std::string& GetSizeRole() const { return m_sizeRole; }
    const std::string& GetFontRole() const { return m_fontRole; }
    int GetSizeDelta() const { return m_sizeDelta; }
    FontData fontData() const; // resolves role from the theme (may lazyload on first use)
    Vector2 RealSize() { return m_lastMeasuredSize; }

private:
    std::vector<mytext::Line> constructConstrained(const std::vector<mytext::Line>& lines, Vector2 borders);
    void measureLines();

    std::string m_text;
    std::string m_sizeRole = "default";
    SizeRoleId m_sizeRoleId{0}; // resolved from m_sizeRole at construction
    std::string m_fontRole = "default";
    FontRoleId m_fontRoleId{0}; // resolved from m_fontRole at construction
    int m_sizeDelta{0};       // extra ladder offset on top of the role
    Color m_color = RAYWHITE;

    Vector2 m_lastBorder{0, 0};
    Vector2 m_lastMeasuredSize{0, 0};
    Vector2 m_desiredFullSize{0, 0};

    std::vector<mytext::Line> m_lines;
    std::vector<mytext::Line> m_linesConstrained;
    TextVAlign m_valign{TextVAlign::Top};
    bool m_dirtyText = false;
    bool m_dirtyFull = false;
    int m_lastFontSize = 0;
    uint64_t m_themeVersion{0};
};
