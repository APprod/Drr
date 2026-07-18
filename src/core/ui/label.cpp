#include "core/ui/label.hpp"
#include "core/utils/util.hpp"

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
    m_text.ReMeasure({r.width, r.height});

    if (m_compSpec.fillMode == FillMode::FillMaxSize)
        return false;

    auto oldContent = m_contentDesiredSize;
    MeasureContent({r.width, r.height});

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
    auto maxOffset = getMaxOffset();

    if (maxOffset > 0.0f && m_atBottom)
        m_scrollOffset = maxOffset;
    myClamp(m_scrollOffset, 0.0f, maxOffset);

    auto ir = irect(rect);
    BeginScissorMode(ir.x, ir.y, ir.width, ir.height);

    Vector2 pos;
    pos.y = rect.y - m_scrollOffset;
    switch (m_textAlign) {
        case TextAlign::Left:   pos.x = rect.x; break;
        case TextAlign::Center: pos.x = rect.x + (rect.width - textSize.x) / 2; break;
        case TextAlign::Right:  pos.x = rect.x + rect.width - textSize.x; break;
    }
    m_text.Draw(pos);
    if (maxOffset > 0.0f)
        ::DrawRectangleLinesEx(rect, 1, WHITE);
    EndScissorMode();
}

float Label::getMaxOffset(){
    auto drawRect = GetDrawRect();
    auto textSize = m_text.RealSize();
    auto maxOffset = textSize.y - drawRect.height;
    maxOffset = std::max(0.0f,maxOffset);
    return maxOffset;
}

void Label::CalculateOffset(float delta){
    auto maxOffset = getMaxOffset();
    m_scrollOffset -= m_scrollSpeed * delta;
    myClamp(m_scrollOffset, 0.0f, maxOffset);
    m_atBottom = (m_scrollOffset == maxOffset);
}

bool Label::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<ScrollEvent>(&event)){
        if (!m_hovered){
            return false;
        }
        CalculateOffset(e->delta.y);
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