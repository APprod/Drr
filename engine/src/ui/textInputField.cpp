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
    m_viewShift = 0;
    syncText();
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
                    focus();
                }
                cursorToPos(e->pos.x);
                return true;
            }
            if (m_focused) {
                blur();
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
                if (e->repeat) return false;
                if (m_onSubmit) m_onSubmit(m_buffer);
                blur();
                break;
            case KEY_ESCAPE:
                if (e->repeat) return false;
                blur();
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

    auto textSize = m_text.RealSize();
    auto drawCall = [this, rect, textSize](){
        Vector2 pos;
        pos.x = rect.x - m_viewShift;
        pos.y = rect.y + (rect.height - textSize.y) * 0.5f;
        m_text.Draw(pos);
    };
    m_scroll.DrawInside(rect, drawCall);

    if (m_focused && fmodf(m_cursorTimer, 0.5f) < 0.25f) {
        auto& theme = GetServices().runtimeCfg.user.theme;
        auto font = theme.resolveFont(m_text.GetRole());
        int fontSize = theme.resolveSize(m_text.GetRole());
        float spacing = m_text.GetFontSpacing();

        float textH = textSize.y;
        if (textH <= 0) textH = static_cast<float>(fontSize);
        float cursorY = rect.y + (rect.height - textH) * 0.5f;

        std::string prefix = m_buffer.substr(0, m_cursorPos);
        float cursorX = rect.x - m_viewShift;
        if (!prefix.empty()) {
            cursorX += MeasureTextEx(font, prefix.c_str(), static_cast<float>(fontSize), spacing).x;
        }
        DrawRectangle(static_cast<int>(cursorX), static_cast<int>(cursorY),
                      2, static_cast<int>(textH), WHITE);
    }
}

void TextInputField::focus(){
    m_focused = true;
    m_cursorTimer = 0;
    GetUIContext().SetCapture(this);
    m_cursorPos = static_cast<int>(m_buffer.size());
    updateViewShift();
}

void TextInputField::blur(){
    m_focused = false;
    GetUIContext().ReleaseCapture();
}

void TextInputField::syncText(){
    m_text.SetText(m_buffer);
    m_cursorTimer = 0;
    updateViewShift();
}

float TextInputField::cursorX() const {
    auto& theme = GetServices().runtimeCfg.user.theme;
    auto font = theme.resolveFont(m_text.GetRole());
    int fontSize = theme.resolveSize(m_text.GetRole());
    float spacing = m_text.GetFontSpacing();
    std::string prefix = m_buffer.substr(0, m_cursorPos);
    if (prefix.empty()) return 0.0f;
    return MeasureTextEx(font, prefix.c_str(), static_cast<float>(fontSize), spacing).x;
}

void TextInputField::updateViewShift(){
    auto rect = GetVisualRect();
    float viewWidth = rect.width;
    float cx = cursorX();

    // keep cursor inside the view with a small margin
    const float margin = 2.0f;
    if (cx - m_viewShift < margin)
        m_viewShift = cx - margin;
    else if (cx - m_viewShift > viewWidth - margin)
        m_viewShift = cx - viewWidth + margin;

    // never show empty space on the left; release shift when text fits
    float textW = m_text.RealSize().x;
    if (textW <= viewWidth || m_viewShift < 0.0f)
        m_viewShift = std::min(m_viewShift, 0.0f);
    if (textW <= viewWidth)
        m_viewShift = 0.0f;
    float maxShift = std::max(0.0f, textW - viewWidth);
    m_viewShift = std::clamp(m_viewShift, 0.0f, maxShift);
}

void TextInputField::cursorToPos(float x){
    auto& theme = GetServices().runtimeCfg.user.theme;
    auto font = theme.resolveFont(m_text.GetRole());
    int fontSize = theme.resolveSize(m_text.GetRole());
    float spacing = m_text.GetFontSpacing();

    float localX = x - GetVisualRect().x + m_viewShift;
    int best = 0;
    float bestDist = std::abs(localX);
    int i = 0;
    while (i < static_cast<int>(m_buffer.size())) {
        int cpSize = 0;
        GetCodepointNext(m_buffer.c_str() + i, &cpSize);
        if (cpSize <= 0) break;
        i = std::min(i + cpSize, static_cast<int>(m_buffer.size()));
        float w = MeasureTextEx(font, m_buffer.substr(0, i).c_str(),
                                static_cast<float>(fontSize), spacing).x;
        float dist = std::abs(localX - w);
        if (dist < bestDist) { bestDist = dist; best = i; }
        else break;
    }
    m_cursorPos = best;
    m_cursorTimer = 0;
    updateViewShift();
}

void TextInputField::insertChar(int codepoint) {
    if (codepoint < 32) return;
    int len = 0;
    const char* encoded = CodepointToUTF8(codepoint, &len);
    if (!encoded || len <= 0) return;
    m_buffer.insert(static_cast<size_t>(m_cursorPos), encoded, static_cast<size_t>(len));
    m_cursorPos += len;
    syncText();
}

void TextInputField::deleteBefore() {
    if (m_cursorPos <= 0) return;
    int size = 0;
    GetCodepointPrevious(m_buffer.c_str() + m_cursorPos, &size);
    size = std::clamp(size, 1, m_cursorPos);
    m_cursorPos -= size;
    m_buffer.erase(static_cast<size_t>(m_cursorPos), static_cast<size_t>(size));
    syncText();
}

void TextInputField::deleteAfter() {
    if (m_cursorPos >= static_cast<int>(m_buffer.size())) return;
    int size = 0;
    GetCodepointNext(m_buffer.c_str() + m_cursorPos, &size);
    size = std::clamp(size, 1, static_cast<int>(m_buffer.size()) - m_cursorPos);
    m_buffer.erase(static_cast<size_t>(m_cursorPos), static_cast<size_t>(size));
    syncText();
}

void TextInputField::cursorLeft() {
    if (m_cursorPos > 0) {
        int size = 0;
        GetCodepointPrevious(m_buffer.c_str() + m_cursorPos, &size);
        m_cursorPos -= std::clamp(size, 1, m_cursorPos);
        m_cursorTimer = 0;
        updateViewShift();
    }
}

void TextInputField::cursorRight() {
    if (m_cursorPos < static_cast<int>(m_buffer.size())) {
        int size = 0;
        GetCodepointNext(m_buffer.c_str() + m_cursorPos, &size);
        m_cursorPos += std::clamp(size, 1, static_cast<int>(m_buffer.size()) - m_cursorPos);
        m_cursorTimer = 0;
        updateViewShift();
    }
}

void TextInputField::cursorHome() {
    m_cursorPos = 0;
    m_cursorTimer = 0;
    updateViewShift();
}

void TextInputField::cursorEnd() {
    m_cursorPos = static_cast<int>(m_buffer.size());
    m_cursorTimer = 0;
    updateViewShift();
}
