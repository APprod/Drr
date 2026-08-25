#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "raylib.h"
#include "ui/uiTheme.hpp"

struct Word{
    std::string word;
    Vector2 size{0,0};
};

struct Line{
    std::vector<Word> words;
    Vector2 size{0,0};
    std::string lineView;
};

std::vector<Line> splitLines(std::string& text);


enum class TextVAlign { Top, Center, Bottom };

class Text {
public:
    Text() = default;
    Text(std::string text, std::string role = "default",
         int sizeDelta = 0, Color color = RAYWHITE);

    void SetText(std::string t) { 
        m_text = std::move(t); 
        m_dirtyText = true;
    }
    const std::string& GetText() const { return m_text; }

    std::vector<Line> constructConstrained(const std::vector<Line>& lines, Vector2 borders);
    Vector2 ReMeasure(Vector2 borders);
    void measureLines();
    void Draw(Vector2 position);
    void DrawCentered(Rectangle bounds);

    bool IsDirty() const { return m_dirtyFull; }
    void ClearDirty() { m_dirtyFull = false; }
    int GetFontSize() const { return m_lastFontSize; }
    const std::string& GetRole() const { return m_role; }
    void SetSizeDelta(int delta) { if (m_sizeDelta != delta){ m_sizeDelta = delta; m_dirtyText = true; } }
    int GetSizeDelta() const { return m_sizeDelta; }
    FontData fontData() const; // resolves role against the theme (may lazy-bake on first use)
    void SetVAlign(TextVAlign align) { m_valign = align; }
    TextVAlign GetVAlign() const { return m_valign; }
    Vector2 RealSize() { return m_lastMeasuredSize; }

private:
    std::string m_text;
    std::string m_role = "default";
    SizeRoleId m_sizeRole{0}; // resolved from m_role at construction
    int m_sizeDelta{0};       // extra ladder offset on top of the role
    Color m_color = RAYWHITE;

    Vector2 m_lastBorder{0, 0};
    Vector2 m_lastMeasuredSize{0, 0};
    Vector2 m_desiredFullSize{0, 0};

    std::vector<Line> m_lines;
    std::vector<Line> m_linesConstrained;
    TextVAlign m_valign{TextVAlign::Top};
    bool m_dirtyText = false;
    bool m_dirtyFull = false;
    int m_lastFontSize = 0;
    uint64_t m_themeVersion{0};
};
