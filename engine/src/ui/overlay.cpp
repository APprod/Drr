#include "ui/overlay.hpp"
#include "services.hpp"
#include "ui/label.hpp"

Overlay::Overlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {

    auto listener = std::make_unique<HotkeysListener>();
    m_listener = listener.get();
    listener->Bind(InputKey::KEY_F1, []{
        GetServices().runtimeCfg.debug.showDebugOverlay ^= true;
    });
    listener->Bind(InputKey::KEY_F2, []{
        GetServices().runtimeCfg.debug.showLayoutBounds ^= true;
    });
    listener->Bind(InputKey::KEY_F3, []{
        GetServices().runtimeCfg.debug.showLayoutContentBounds ^= true;
    });
    listener->Bind(InputKey::KEY_F4, []{
        GetServices().runtimeCfg.user.showFPS ^= true;
    });
    listener->Bind(InputKey::KEY_F5, []{
        GetServices().runtimeCfg.debug.showCursorPos ^= true;
    });
    listener->Bind(InputKey::KEY_F6, []{
        GetServices().runtimeCfg.debug.showPerformance ^= true;
    });
    listener->Bind(InputKey::KEY_F7, []{
        GetServices().runtimeCfg.debug.showDebugLog ^= true;
    });
    listener->Bind(InputKey::KEY_F8, []{
        GetServices().runtimeCfg.debug.useProcessingShader ^= true;
    });
    listener->Bind(InputKey::KEY_F9, []{
        GetServices().runtimeCfg.debug.showOverlayGradient ^= true;
    });
    AddChild(std::move(listener));
    Add(FPSDraw(Text("FPS: ", "big header")));
    GetUIContext().SetOverlay(this);
}

Overlay::~Overlay(){
    GetUIContext().ResetOverlay();
}

UICompId Overlay::PushPopup(std::unique_ptr<UIComponent> comp){
    id = AddChild(std::move(comp));
    ids.push_back(id);
    return id;
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

UIComponent* Overlay::GetPopup(UICompId popupId){
    for (auto& child : m_children) {
        if (child->id == popupId) return child.get();
    }
    return nullptr;
}