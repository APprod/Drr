#include "ui/label.hpp"
#include "utils/util.hpp"

#include <cmath>

Label::Label(
    Text text,
    UIComponentSpec spec,
    TextAlign align
): UIComponent{spec}, m_text{std::move(text)}, m_textAlign{align}
{}

void Label::SetText(std::string text){
    m_text.SetText(std::move(text));
}

bool Label::OnUpdate(float)
{
    auto r = GetDrawRect();
    auto oldContent = m_contentDesiredSize;
    MeasureContent({r.width, r.height});
    m_scroll.OnUpdate(r,m_contentDesiredSize);
    if (m_compSpec.fillMode == FillMode::FillMaxSize)
        return false;

    auto effectiveOld = oldContent;
    auto effectiveNew = m_contentDesiredSize;
    switch (m_compSpec.fillMode) {
        case FillMode::FillMaxWidth:  effectiveOld.x = effectiveNew.x = 0; break;
        case FillMode::FillMaxHeight: effectiveOld.y = effectiveNew.y = 0; break;
        default: break;
    }

    if (effectiveOld != effectiveNew) {
        return true;
    }
    return false;
}

void Label::MeasureContent(Vector2 available){
    auto textSize = m_text.ReMeasure(available);
    m_contentDesiredSize = {
        std::min(available.x, std::ceil(textSize.x)),
        std::ceil(textSize.y)
    };
}

void Label::OnDrawContent(){
    auto rect = GetDrawRect();
    auto textSize = m_text.RealSize();

    auto drawCall = [this, rect, textSize](){
        Vector2 pos;
        pos.y = rect.y - m_scroll.scrollOffset;
        switch (m_textAlign) {
            case TextAlign::Left:   pos.x = rect.x; break;
            case TextAlign::Center: pos.x = rect.x + (rect.width - textSize.x) / 2; break;
            case TextAlign::Right:  pos.x = rect.x + rect.width - textSize.x; break;
        }
        m_text.Draw(pos);
    };
    m_scroll.DrawInside(rect, drawCall);
}

bool Label::OnEvent(const MyEvent& event){
    return m_scroll.OnEvent(event);
}

bool FPSDraw::OnUpdate(float dt){
    if (GetServices().runtimeCfg.user.showFPS)
        SetText(std::to_string(GetFPS()) + " FPS");
    else
        SetText("");
    return Label::OnUpdate(dt);
}