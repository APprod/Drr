#include "core/ui/clickable.hpp"

Clickable::Clickable(UIComponentSpec spec)
    : UIComponent(spec)
{}

bool Clickable::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<CursorActionEvent>(&event)){
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT && e->pressed){
            if (HitTest(e->pos)){
                m_hold = true;
                OnPressed();
                GetUIContext().SetCapture(this);
                return true;
            }
        }
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT && !e->pressed){
            if (m_hold){
                OnReleased();
                if (HitTest(e->pos)){
                    OnClick();
                }
                m_hold = false;
                GetUIContext().ReleaseCapture();
                return true;
            }
        }
    }
    return false;
}
