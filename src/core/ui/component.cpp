#include "core/ui/component.hpp"
#include "core/ui/overlay.hpp"

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}

UIComponent::~UIComponent(){
    GetUIContext().InvalidateComponent(this);
}

EventMask  UIContext::GetCaptureTypes() const {
    if (m_captured) return m_captured->getCaptureTypes();
    return 0;
}


UICompId UIContext::PushPopup(std::unique_ptr<UIComponent> comp){
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't push a Popup, no overlays registered");
        return 0;
    }
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't push a Popup, current overlay isn't set");
        return 0;
    }
    return m_overlayStack.back()->PushPopup(std::move(comp));
}
    
UICompId UIContext::PopPopup(){
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't pop a Popup, no overlays registered");
        return 0;
    }
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't pop a Popup, current overlay isn't set");
        return 0;
    }
    return m_overlayStack.back()->PopPopup();
}
bool UIContext::RemovePopup(UICompId id){
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't Remove a Popup, no overlays registered");
        return 0;
    }
    if (m_overlayStack.empty()){
        dbg::GetLogger().Warn("Can't Remove a Popup, current overlay isn't set");
        return 0;
    }
    return m_overlayStack.back()->RemovePopup(id);
}