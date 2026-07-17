#include "core/ui/label.hpp"
#include "core/util.hpp"

Label::Label(
    Text text,
    UIComponentSpec spec
): UIComponent{spec}, m_text{std::move(text)}
{}

void Label::SetText(std::string text){
    m_text.SetText(std::move(text));
}

bool Label::OnUpdate()
{
    auto r = GetDrawRect();
    m_text.ReMeasure({r.width,r.height});
    if (m_text.IsDirty()) {
        m_text.ClearDirty();
        return true;
    }
    return false;
}

void Label::MeasureContent(Vector2 available){
    auto textSize = m_text.ReMeasure(available);
    m_contentDesiredSize = {
        std::min(available.x, textSize.x),
        textSize.y
    };
}

void Label::OnDrawContent(){
    auto rect = GetDrawRect();
    auto maxOffset = getMaxOffset();

    if (maxOffset > 0.0f && m_atBottom) {
        m_scrollOffset = maxOffset;
    }
    myClamp(m_scrollOffset, 0.0f, maxOffset);

    auto ir = irect(rect);
    BeginScissorMode(ir.x, ir.y, ir.width, ir.height);
    m_text.Draw({rect.x, rect.y - m_scrollOffset});
    if (maxOffset > 0.0f){
        ::DrawRectangleLinesEx(rect,1,WHITE);
    }
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



bool FPSDraw::OnUpdate(){
    if (GetServices().runtimeCfg.debug.showFPS)
        SetText(std::to_string(GetFPS()) + " FPS");
    else
        SetText("");
    return Label::OnUpdate();
}