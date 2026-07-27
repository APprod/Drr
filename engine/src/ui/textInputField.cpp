#include "ui/textInputField.hpp"
#include "services.hpp"

#include <cmath>

TextInputField::TextInputField(
    std::string role,
    UIComponentSpec spec,
    std::function<void(std::string)> onSubmit
)
    : Label(Text("", std::move(role)), std::move(spec), TextAlign::Left)
    , m_onSubmit(std::move(onSubmit))
{
    m_text.SetVAlign(TextVAlign::Center);
}

void TextInputField::SetText(std::string text) {
    m_buffer = std::move(text);
    m_cursorPos = static_cast<int>(m_buffer.size());
    m_text.SetText(m_buffer);
}

bool TextInputField::OnUpdate(float dt) {
    if (m_focused) {
        m_cursorTimer += dt;
    }
    return Label::OnUpdate(dt);
}

bool TextInputField::OnEvent(const MyEvent& event) {
    if (auto* e = std::get_if<CursorActionEvent>(&event)) {
        if (e->pressed) {
            if (HitTest(e->pos)) {
                if (!m_focused) {
                    m_focused = true;
                    m_cursorTimer = 0;
                    GetUIContext().SetCapture(this);
                    m_cursorPos = static_cast<int>(m_buffer.size());
                }
                return true;
            }
            if (m_focused) {
                m_focused = false;
                GetUIContext().ReleaseCapture();
                return false;
            }
        }
        return false;
    }

    if (auto* e = std::get_if<InputKeyEvent>(&event)) {
        if (!e->pressed || !m_focused) return false;

        switch (e->key) {
            case KEY_BACKSPACE: deleteBefore(); break;
            case KEY_DELETE:    deleteAfter();  break;
            case KEY_LEFT:      cursorLeft();   break;
            case KEY_RIGHT:     cursorRight();  break;
            case KEY_HOME:      cursorHome();   break;
            case KEY_END:       cursorEnd();    break;
            case KEY_ENTER:
                if (m_onSubmit) m_onSubmit(m_buffer);
                [[fallthrough]];
            case KEY_ESCAPE:
                m_focused = false;
                GetUIContext().ReleaseCapture();
                break;
            default: return false;
        }
        return true;
    }

    if (auto* e = std::get_if<CharEvent>(&event)) {
        if (!m_focused) return false;
        insertChar(e->codepoint);
        return true;
    }

    return false;
}

void TextInputField::MeasureContent(Vector2 available) {
    Label::MeasureContent(available);
    int fontSize = m_text.GetFontSize();
    if (fontSize > 0) {
        float lineH = static_cast<float>(fontSize);
        m_contentDesiredSize.y = std::max(m_contentDesiredSize.y, lineH);
    }
}

void TextInputField::OnDrawContent() {
    auto rect = GetVisualRect();

    Color borderColor = m_focused ? RAYWHITE : GRAY;
    DrawRectangleLines(static_cast<int>(rect.x), static_cast<int>(rect.y),
                       static_cast<int>(rect.width), static_cast<int>(rect.height), borderColor);

    Label::OnDrawContent();

    if (m_focused && fmodf(m_cursorTimer, 0.5f) < 0.25f) {
        auto& theme = GetServices().runtimeCfg.user.theme;
        auto font = theme.resolveFont(m_text.GetRole());
        int fontSize = theme.resolveSize(m_text.GetRole());
        float spacing = m_text.GetFontSpacing();

        float textH = m_text.RealSize().y;
        if (textH <= 0) textH = static_cast<float>(fontSize);
        float cursorY = rect.y + (rect.height - textH) * 0.5f;

        std::string prefix = m_buffer.substr(0, m_cursorPos);
        float cursorX = rect.x;
        if (!prefix.empty()) {
            cursorX += MeasureTextEx(font, prefix.c_str(), static_cast<float>(fontSize), spacing).x;
        }
        DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY),
                      2, static_cast<int>(textH), WHITE);
    }
}

void TextInputField::insertChar(int codepoint) {
    m_buffer.insert(m_buffer.begin() + m_cursorPos, static_cast<char>(codepoint));
    m_cursorPos++;
    m_text.SetText(m_buffer);
}

void TextInputField::deleteBefore() {
    if (m_cursorPos <= 0) return;
    m_buffer.erase(m_cursorPos - 1, 1);
    m_cursorPos--;
    m_text.SetText(m_buffer);
}

void TextInputField::deleteAfter() {
    if (m_cursorPos >= static_cast<int>(m_buffer.size())) return;
    m_buffer.erase(m_cursorPos, 1);
    m_text.SetText(m_buffer);
}

void TextInputField::cursorLeft() {
    if (m_cursorPos > 0) m_cursorPos--;
}

void TextInputField::cursorRight() {
    if (m_cursorPos < static_cast<int>(m_buffer.size())) m_cursorPos++;
}

void TextInputField::cursorHome() {
    m_cursorPos = 0;
}

void TextInputField::cursorEnd() {
    m_cursorPos = static_cast<int>(m_buffer.size());
}
