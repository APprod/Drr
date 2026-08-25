#include "ui/component.hpp"
#include "ui/overlay.hpp"

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}

UIComponent::~UIComponent(){
    GetUIContext().ClearComponent(this);
}

EventMask  UIContext::GetCaptureTypes() const {
    if (m_captured) return m_captured->getCaptureTypes();
    return 0;
}


UICompId UIContext::PushPopup(std::unique_ptr<UIComponent> comp){
    if (m_overlayStack.empty()){
        mylog::GetLogger().Warn("Can't push a Popup, no overlays registered");
        return 0;
    }
    return m_overlayStack.back()->PushPopup(std::move(comp));
}
    
UICompId UIContext::PopPopup(){
    if (m_overlayStack.empty()){
        mylog::GetLogger().Warn("Can't pop a Popup, no overlays registered");
        return 0;
    }
    return m_overlayStack.back()->PopPopup();
}
void UIContext::RemovePopup(UICompId id){
    if (m_overlayStack.empty()){
        mylog::GetLogger().Warn("Can't Remove a Popup, no overlays registered");
        return;
    }
    m_overlayStack.back()->RemovePopup(id);
}
void UIContext::RemovePopupImmediate(UICompId id){
    if (m_overlayStack.empty()){
        mylog::GetLogger().Warn("Can't Remove a Popup, no overlays registered");
        return;
    }
    m_overlayStack.back()->RemovePopupImmediate(id);
}

UIComponent* UIContext::GetPopupById(UICompId id){
    if (m_overlayStack.empty()){
        return nullptr;
    }
    return m_overlayStack.back()->GetPopupById(id);
}