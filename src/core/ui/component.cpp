#include "core/ui/component.hpp"
#include "core/ui/overlay.hpp"

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}

EventMask  UIContext::GetCaptureTypes() const {
    if (m_captured) return m_captured->getCaptureTypes();
    return 0;
}


UICompId UIContext::PushPopup(std::unique_ptr<UIComponent> comp){
    if (!m_overlay){
        dbg::GetLogger().Warn("Can't push a Popup, overlay isn't set");
        return 0;
    }
    return m_overlay->PushPopup(std::move(comp));
}
    
UICompId UIContext::PopPopup(){
    if (!m_overlay){
        dbg::GetLogger().Warn("Can't Pop a Popup, overlay isn't set");
        return 0;
    }
    return m_overlay->PopPopup();
}
bool UIContext::RemovePopup(UICompId id){
    if (!m_overlay){
        dbg::GetLogger().Warn("Can't Remove a Popup, overlay isn't set");
        return 0;
    }
    return m_overlay->RemovePopup(id);
}