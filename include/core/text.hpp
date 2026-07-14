#pragma once

#include <string>
#include <vector>
#include "raylib.h"

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


class Text {
public:
    Text() = default;
    Text(std::string text, std::string fontName = "default",
         int fontSize = 20, float fontSpacing = 2, Color color = RAYWHITE);

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
    Vector2 RealSize() { return m_lastMeasuredSize; }

private:
    std::string m_text;
    std::string m_fontName = "default";
    int m_fontSize = 20;
    float m_fontSpacing = 2;
    Color m_color = RAYWHITE;

    Vector2 m_lastBorder{0, 0};
    Vector2 m_lastMeasuredSize{0, 0};
    Vector2 m_desiredFullSize{0, 0};

    std::vector<Line> m_lines;
    std::vector<Line> m_linesConstrained;
    bool m_dirtyText = false;
    bool m_dirtyFull = false;
};
