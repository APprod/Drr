#include "core/text.hpp"
#include "core/services.hpp"

Text::Text(std::string text, std::string fontName, int fontSize, float fontSpacing, Color color)
    : m_text{std::move(text)}, m_fontName{std::move(fontName)}, m_fontSize{fontSize}, m_fontSpacing{fontSpacing}, m_color{color}
{
    m_dirtyText = true;
}

std::vector<Line> splitLines(std::string& text)
{
    std::vector<Line> lines;
    Line line;

    size_t i = 0;
    while (i < text.size())
    {
        if (text[i] == '\n')
        {
            lines.push_back(std::move(line));
            line = {};
            ++i;
            continue;
        }

        size_t start = i;
        bool isSpace = text[i] == ' ';

        while (i < text.size() &&
               text[i] != '\n' &&
               (text[i] == ' ') == isSpace)
        {
            ++i;
        }

        line.words.push_back({text.substr(start, i - start)});
    }

    if (!line.words.empty())
        lines.push_back(std::move(line));

    return lines;
}

//Measures the m_lines words sizes
void Text::measureLines(){
    auto font = GetServices().recManager.getFont(m_fontName, static_cast<int>(m_fontSize));
    float spaceSize = MeasureTextEx(font, " ", static_cast<float>(m_fontSize), m_fontSpacing).x;
    for (auto& line: m_lines ){
        Vector2 linesize{0,0};
        bool first = true;
        for (auto& word: line.words){
            auto size = MeasureTextEx(font, word.word.c_str(), static_cast<float>(m_fontSize), m_fontSpacing);
            word.size = size;
            linesize.x += size.x;
            if (!first) linesize.x += spaceSize;
            else first = false;
            linesize.y = std::max(size.y,linesize.y);
        }
        line.size = linesize;
    }
}
std::vector<Line> Text::constructConstrained(const std::vector<Line>& lines, Vector2 borders)
{
    std::vector<Line> result;

    for (const auto& srcLine : lines){
        Line current;
        float currentWidth = 0.0f;

        for (const auto& token : srcLine.words){
            bool isSpaces = !token.word.empty() && token.word[0] == ' ';

            if (isSpaces && current.words.empty())
                continue;

            if (!current.words.empty() &&
                currentWidth + token.size.x > borders.x &&
                !isSpaces){
                while (!current.words.empty() &&
                       !current.words.back().word.empty() &&
                       current.words.back().word[0] == ' '){
                    currentWidth -= current.words.back().size.x;
                    current.words.pop_back();
                }

                current.size.x = currentWidth;
                result.push_back(std::move(current));

                current = {};
                currentWidth = 0.0f;
            }

            current.words.push_back(token);
            currentWidth += token.size.x;
            current.size.y = std::max(current.size.y, token.size.y);
        }

        while (!current.words.empty() &&
               !current.words.back().word.empty() &&
               current.words.back().word[0] == ' '){
            currentWidth -= current.words.back().size.x;
            current.words.pop_back();
        }

        if (!current.words.empty()){
            current.size.x = currentWidth;
            result.push_back(std::move(current));
        }
    }

    for (auto& line : result){
        line.lineView.clear();
        for (auto& token : line.words)
            line.lineView += token.word;
    }

    return result;
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
    auto font = GetServices().recManager.getFont(m_fontName, static_cast<int>(m_fontSize));
    auto pos = position;
    for (auto& line: m_linesConstrained){
        
        ::DrawTextEx(font, line.lineView.c_str(), pos, static_cast<float>(m_fontSize), m_fontSpacing, m_color);
        pos.y += line.size.y;
    }
    
}

void Text::DrawCentered(Rectangle bounds)
{
    float x = bounds.x + (bounds.width - m_lastMeasuredSize.x) / 2;
    float y = bounds.y + (bounds.height - m_lastMeasuredSize.y) / 2;
    Draw({x, y});
}
