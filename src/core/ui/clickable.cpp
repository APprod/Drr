#include "core/ui/clickable.hpp"

Clickable::Clickable(UIComponentSpec spec)
    : UIComponent(spec)
{}

EventResult Clickable::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<CursorActionEvent>(&event)){
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT && e->pressed){
            if (HitTest(e->pos)){
                m_hold = true;
                OnPressed();
                return EventResult::RequireCapture;
            }
        }
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT && !e->pressed){
            if (m_hold){
                OnReleased();
                if (HitTest(e->pos)){
                    OnClick();
                }
                m_hold = false;
                return EventResult::ReleaseCapture;
            }
        }
    }
    return EventResult::NotHandled;
}
