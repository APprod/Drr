#include "ui/overlay.hpp"
#include "services.hpp"
#include "ui/label.hpp"
#include "ui/popup.hpp"

Overlay::Overlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {

    auto listener = std::make_unique<DebugHotkeysListener>();
    listener->Bind(InputKey::KEY_F1, []{
        GetServices().debugFlags.showDebugOverlay ^= true;
    }, "overlay");
    listener->Bind(InputKey::KEY_F2, []{
        GetServices().debugFlags.showLayoutBounds ^= true;
    }, "layout");
    listener->Bind(InputKey::KEY_F3, []{
        GetServices().debugFlags.showLayoutContentBounds ^= true;
    }, "content");
    listener->Bind(InputKey::KEY_F4, []{
        GetServices().userSettings.showFPS ^= true;
    }, "fps");
    listener->Bind(InputKey::KEY_F5, []{
        GetServices().debugFlags.showCursorPos ^= true;
    }, "cursor");
    listener->Bind(InputKey::KEY_F6, []{
        GetServices().debugFlags.showPerformance ^= true;
    }, "perf");
    listener->Bind(InputKey::KEY_F7, []{
        GetServices().debugFlags.showDebugLog ^= true;
    }, "log");
    listener->Bind(InputKey::KEY_F9, []{
        GetServices().debugFlags.showOverlayGradient ^= true;
    }, "gradient");
    AddChild(std::move(listener));
    auto listener2 = std::make_unique<HotkeysListener>();
    m_listener = listener2.get();
    AddChild(std::move(listener2));
    Add(FPSDraw(Text("FPS: ", "default", "default", 4)));
    GetUIContext().SetOverlay(this);
}

Overlay::~Overlay(){
    GetUIContext().ResetOverlay();
}

UICompId Overlay::PushPopup(std::unique_ptr<UIComponent> comp){
    auto newId = AddChild(std::move(comp));
    ids.push_back(newId);
    return newId;
}

UICompId Overlay::PopPopup(){
    if (ids.empty()){
        mylog::GetLogger().Warn("Trying to pop Popup in empty list");
        return 0;
    }
    UICompId last = ids.back();
    ids.pop_back();
    QueueRemoveChild(last);
    return last;
}

void Overlay::RemovePopup(UICompId compId){
    auto it = std::find_if(ids.begin(), ids.end(),
        [compId](const auto& c){ return c == compId; });
    if (it == ids.end()){
        mylog::GetLogger().Warn("RemovePopup: id not found: ", compId);
        return;
    }
    ids.erase(it);
    QueueRemoveChild(compId);
}

void Overlay::RemovePopupImmediate(UICompId compId){
    auto it = std::find_if(ids.begin(), ids.end(),
        [compId](const auto& c){ return c == compId; });
    if (it == ids.end()){
        mylog::GetLogger().Warn("RemovePopupImmediate: id not found: ", compId);
        return;
    }
    ids.erase(it);
    RemoveChild(compId);
}

UIComponent* Overlay::GetPopupById(UICompId popupId){
    for (auto& child : m_children) {
        if (child->id == popupId) return child.get();
    }
    return nullptr;
}

bool Overlay::OnEvent(const MyEvent& event){
    bool handled = Stack::OnEvent(event);
    if (handled) return true;
    if (auto* e = std::get_if<CursorActionEvent>(&event)){
        if (e->pressed && !ids.empty()){
            CloseTopPopup();
            return true;
        }
    }
    return false;
}

void Overlay::CloseTopPopup(){
    if (ids.empty()) return;
    UICompId top = ids.back();
    if (auto* comp = GetPopupById(top)){
        if (auto* popup = comp->GetAs<Popup>()){
            if (auto* reveal = popup->GetChildAs<AnimatedReveal>()){
                reveal->setOpen(false);
                return;
            }
        }
    }
    RemovePopup(top);
}