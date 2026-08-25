#include "rendering/text.hpp"
#include "services.hpp"

#include <cmath>

namespace {
// A line is blank when it has no words or only space tokens
bool isBlankLine(const std::vector<Word>& words){
    for (const auto& w : words)
        if (w.word.empty() || w.word[0] != ' ') return false;
    return true;
}
}

Text::Text(std::string text, std::string role, int sizeDelta, Color color)
    : m_text{std::move(text)}, m_role{std::move(role)}, m_sizeRole{0},
      m_sizeDelta{sizeDelta}, m_color{color}
{
    auto& theme = GetServices().theme;
    m_sizeRole = theme.getSizeRole(m_role);
    m_dirtyText = true;
}

FontData Text::fontData() const {
    auto& theme = GetServices().theme;
    return theme.getFont({m_sizeRole, 0}, m_sizeDelta);
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
        char c = text[i];
        auto isSep = [](char ch) { return ch == '/' || ch == '\\'; };

        if (c == ' ')
        {
            while (i < text.size() && text[i] == ' ')
                ++i;
        }
        else if (isSep(c))
        {
            while (i < text.size() && isSep(text[i]))
                ++i;
        }
        else
        {
            while (i < text.size() && text[i] != '\n' && text[i] != ' ' && !isSep(text[i]))
                ++i;
        }

        line.words.push_back({text.substr(start, i - start)});
    }

    if (!line.words.empty() || (!text.empty() && text.back() == '\n'))
        lines.push_back(std::move(line));

    return lines;
}

//Measures the m_lines words sizes
void Text::measureLines(){
    auto fd = fontData();
    float spaceSize = MeasureTextEx(fd.font, " ", fd.size, fd.spacing).x;
    for (auto& line: m_lines ){
        Vector2 linesize{0,0};
        bool first = true;
        for (auto& word: line.words){
            auto size = MeasureTextEx(fd.font, word.word.c_str(), fd.size, fd.spacing);
            word.size = size;
            linesize.x += size.x;
            if (!first) linesize.x += spaceSize;
            else first = false;
            linesize.y = std::max(size.y,linesize.y);
        }
        // blank or pure-space lines keep no height from words — use fontSize
        if (linesize.y == 0.0f && (line.words.empty() || isBlankLine(line.words)))
            linesize.y = fd.size;
        line.size = linesize;
    }
    m_lastFontSize = static_cast<int>(fd.size);
}
std::vector<Line> Text::constructConstrained(const std::vector<Line>& lines, Vector2 borders)
{
    std::vector<Line> result;
    auto fd = fontData();
    float blankHeight = fd.size;

    for (const auto& srcLine : lines){
        // blank or pure-space source line — emit single empty line with font height
        if (srcLine.words.empty() || isBlankLine(srcLine.words)) {
            Line blank;
            blank.size = {0.0f, blankHeight};
            blank.lineView = "";
            result.push_back(std::move(blank));
            continue;
        }
        Line current;
        float currentWidth = 0.0f;

        for (const auto& token : srcLine.words){
            bool isSpaces = !token.word.empty() && token.word[0] == ' ';

            if (isSpaces && current.words.empty())
                continue;

            if (!current.words.empty() &&
                currentWidth + token.size.x + current.words.size() * fd.spacing > borders.x &&
                !isSpaces){
                while (!current.words.empty() &&
                       !current.words.back().word.empty() &&
                       current.words.back().word[0] == ' '){
                    currentWidth -= current.words.back().size.x;
                    current.words.pop_back();
                }

                current.size.x = currentWidth;
                if (current.words.size() > 1)
                    current.size.x += (current.words.size() - 1) * fd.spacing;
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
            if (current.words.size() > 1)
                current.size.x += (current.words.size() - 1) * fd.spacing;
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
    m_dirtyFull = false;
    bool themeChanged = GetServices().theme.needsUpdate(m_themeVersion);

    if (m_dirtyText || themeChanged)
    {
        m_lines = splitLines(m_text);
        measureLines();
        m_desiredFullSize = {0,0};
        for (auto& line: m_lines){
            m_desiredFullSize.x = std::max(line.size.x, m_desiredFullSize.x);
            m_desiredFullSize.y += line.size.y;
        }
    }
    if (borders != m_lastBorder || m_dirtyText || themeChanged){
        auto lastMeasured = m_lastMeasuredSize;
        m_linesConstrained = constructConstrained(m_lines, borders);
        m_lastMeasuredSize = {0,0};
        for (auto& line: m_linesConstrained){
            m_lastMeasuredSize.x = std::max(line.size.x, m_lastMeasuredSize.x);
            m_lastMeasuredSize.y += line.size.y;
        }
        if (lastMeasured != m_lastMeasuredSize){
            m_dirtyFull = true;
        }
        m_lastBorder = borders;
    }
    m_dirtyText = false;
    return m_lastMeasuredSize;
}


void Text::Draw(Vector2 position)
{
    position.x = std::round(position.x);
    position.y = std::round(position.y);
    auto fd = fontData();
    auto pos = position;
    for (auto& line: m_linesConstrained){
        
        ::DrawTextEx(fd.font, line.lineView.c_str(), pos, fd.size, fd.spacing, m_color);
        pos.y += line.size.y;
    }
    
}

void Text::DrawCentered(Rectangle bounds)
{
    float x = std::round(bounds.x + (bounds.width - m_lastMeasuredSize.x) / 2);
    float y = std::round(bounds.y + (bounds.height - m_lastMeasuredSize.y) / 2);
    Draw({x, y});
}
