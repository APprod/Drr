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
    auto ir = irect(rect);
    BeginScissorMode(ir.x, ir.y, ir.width, ir.height);
    m_text.Draw({rect.x, rect.y - m_scrollOffset});
    EndScissorMode();
}

EventResult Label::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<ScrollEvent>(&event)){
        if (!hovered){
            return EventResult::NotHandled;
        }
        auto drawRect = GetDrawRect();
        auto textSize = m_text.RealSize();
        auto maxOffset = textSize.y - drawRect.height;
        m_scrollOffset -= m_scrollSpeed * e->delta.y;
        myClamp(m_scrollOffset,0.0f,maxOffset);
        return EventResult::Handled;
    }
    return EventResult::NotHandled;
}
