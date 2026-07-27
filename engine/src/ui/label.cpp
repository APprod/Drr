#include "ui/label.hpp"
#include "utils/util.hpp"

#include <cmath>

Label::Label(
    Text text,
    UIComponentSpec spec,
    TextAlign align
): UIComponent{spec}, m_text{std::move(text)}, m_textAlign{align}
{
    m_scroll.atBottom = true;
}

void Label::SetText(std::string text){
    m_text.SetText(std::move(text));
}

bool Label::OnUpdate(float)
{
    auto r = GetVisualRect();
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

    auto clampToMin = [&](Vector2& v) {
        if (m_compSpec.minSize.x > 0 && v.x < m_compSpec.minSize.x) v.x = m_compSpec.minSize.x;
        if (m_compSpec.minSize.y > 0 && v.y < m_compSpec.minSize.y) v.y = m_compSpec.minSize.y;
    };
    clampToMin(effectiveOld);
    clampToMin(effectiveNew);

    if (effectiveOld != effectiveNew) {
        return true;
    }
    return false;
}

void Label::ArrangeContent(Rectangle rect){
    m_scroll.OnUpdate(GetVisualRect(), m_contentDesiredSize);
    UIComponent::ArrangeContent(rect);
}

void Label::MeasureContent(Vector2 available){
    auto textSize = m_text.ReMeasure(available);
    float desired = std::ceil(textSize.x);
    m_contentDesiredSize = {desired, std::ceil(textSize.y)};

    float pad = static_cast<float>(m_text.GetFontSize()) * 0.25f;
    if (desired > m_accumulatedWidth) {
        m_accumulatedWidth = desired + pad;
    } else {
        float luft = static_cast<float>(m_text.GetFontSize()) * 2.0f;
        if (m_accumulatedWidth - desired > luft)
            m_accumulatedWidth = desired + pad;
    }
    m_contentDesiredSize.x = m_accumulatedWidth;
}

void Label::OnDrawContent(){
    auto rect = GetVisualRect();
    auto textSize = m_text.RealSize();

    auto drawCall = [this, rect, textSize](){
        Vector2 pos;
        pos.y = rect.y - m_scroll.scrollOffset;
        switch (m_text.GetVAlign()) {
            case TextVAlign::Center: pos.y += (rect.height - textSize.y) * 0.5f; break;
            case TextVAlign::Bottom: pos.y += rect.height - textSize.y; break;
            default: break;
        }
        switch (m_textAlign) {
            case TextAlign::Left:   pos.x = rect.x; break;
            case TextAlign::Center: pos.x = rect.x + (rect.width - textSize.x) / 2; break;
            case TextAlign::Right:  pos.x = rect.x + rect.width - textSize.x; break;
        }
        m_text.Draw(pos);
    };
    m_scroll.DrawInside(rect, drawCall);
    m_scroll.DrawScrollbar(rect);
}

bool Label::OnEvent(const MyEvent& event){
    if (m_scroll.OnEvent(event)) {
        if (m_scroll.dragging)
            GetUIContext().SetCapture(this);
        else
            GetUIContext().ReleaseCapture();
        return true;
    }
    return false;
}

bool FPSDraw::OnUpdate(float dt){
    if (GetServices().runtimeCfg.user.showFPS)
        SetText(std::to_string(GetFPS()) + " FPS");
    else
        SetText("");
    return Label::OnUpdate(dt);
}