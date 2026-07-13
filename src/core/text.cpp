#include "core/text.hpp"
#include "core/structs.hpp"
#include "core/debug.hpp"

Text::Text(std::string text, std::string fontName, float fontSize, float fontSpacing, Color color)
    : m_text{std::move(text)}, m_fontName{std::move(fontName)}, m_fontSize{fontSize}, m_fontSpacing{fontSpacing}, m_color{color}
{
    m_dirtyText = true;
}

std::vector<Line> splitLines(std::string& text){
    std::vector<Line> lines;
    Line line;
    size_t start = 0;
    for (size_t i = 0; i <= text.size(); i++){
        if (i == text.size() || text[i] == ' ' || text[i] == '\n'){
            if (i > start)
                line.words.push_back({text.substr(start, i - start)});
            start = i + 1;
            if (i < text.size() && text[i] == '\n')
            {
                lines.push_back(std::move(line));
                line = {};
            }
        }
        
    }
    if (!line.words.empty())
        lines.push_back(std::move(line));
    return lines;
}

//Measures the m_lines words sizes
void Text::measureLines(){
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    float spaceSize = MeasureTextEx(font, " ", m_fontSize, m_fontSpacing).x;
    for (auto& line: m_lines ){
        Vector2 linesize{0,0};
        bool first = true;
        for (auto& word: line.words){
            auto size = MeasureTextEx(font, word.word.c_str(),m_fontSize,m_fontSpacing);
            word.size = size;
            linesize.x += size.x;
            if (!first) linesize.x += spaceSize;
            else first = false;
            linesize.y = std::max(size.y,linesize.y);
        }
        line.size = linesize;
    }
}

std::vector<Line> Text::constructConstrained(const std::vector<Line>& lines, Vector2 borders){
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    float spaceSize = MeasureTextEx(font, " ", m_fontSize, m_fontSpacing).x;
    std::vector<Line> resLines;
    for (auto& line: lines){
        Line newLine;
        float currentWidth = 0;
        for (auto& word : line.words)
        {
            float newWidth = currentWidth + word.size.x;
            if (!newLine.words.empty()) newWidth += spaceSize;
            if (!newLine.words.empty() && newWidth > borders.x){
                newLine.size = {currentWidth, newLine.size.y};
                resLines.push_back(std::move(newLine));

                newLine = {};
                currentWidth = 0;

                newWidth = word.size.x;
            }

            newLine.words.push_back(word);
            currentWidth = newWidth;
            newLine.size.y = std::max(newLine.size.y, word.size.y);
        }
        if (!newLine.words.empty())
        {
            newLine.size.x = currentWidth;
            resLines.push_back(std::move(newLine));
        }
    }
    for (auto& line: resLines){
        for (auto& word : line.words){
            if (!line.lineView.empty())
                line.lineView += " ";

            line.lineView += word.word;
        }
    }
    
    return resLines;
}


// After ReMeasure: if text was changed - all m_lines are recalculated
// If text wasnt changed, only constrained are recalculated
// Returns desired size;
Vector2 Text::ReMeasure(Vector2 borders)
{
    auto lastDesired = m_desiredFullSize;
    if (m_dirtyText)
    {
        m_lines = splitLines(m_text);
        measureLines();
        m_desiredFullSize = {0,0};
        for (auto& line: m_lines){
            m_desiredFullSize.x = std::max(line.size.x, m_desiredFullSize.x);
            m_desiredFullSize.y += line.size.y;
        }
        if (lastDesired != m_desiredFullSize){
            m_dirtyFull = true;
        }
    }
    if (borders != m_lastBorder || m_dirtyText){
        m_linesConstrained = constructConstrained(m_lines, borders);
        m_lastMeasuredSize = {0,0};
        for (auto& line: m_linesConstrained){
            m_lastMeasuredSize.x = std::max(line.size.x, m_lastMeasuredSize.x);
            m_lastMeasuredSize.y += line.size.y;
        }
        m_lastBorder = borders;
    }
    m_dirtyText = false;
    return m_lastMeasuredSize;
}


void Text::Draw(Vector2 position)
{

    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    auto pos = position;
    for (auto& line: m_linesConstrained){
        
        ::DrawTextEx(font, line.lineView.c_str(), pos, m_fontSize, m_fontSpacing, m_color);
        pos.y += line.size.y;
    }
    
}

void Text::DrawCentered(Rectangle bounds)
{
    float x = bounds.x + (bounds.width - m_lastMeasuredSize.x) / 2;
    float y = bounds.y + (bounds.height - m_lastMeasuredSize.y) / 2;
    Draw({x, y});
}
