#include "app/overlay.hpp"
#include "core/structs.hpp"
#include "core/debugOverlay.hpp"

Overlay::Overlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {

    interactive = false;

    auto listener = std::make_unique<HotkeysListener>();
    listener->Bind(InputKeyEvent{InputKey::KEY_F1}, []{
        GetServices().runtimeCfg.showDebugOverlay ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F2}, []{
        GetServices().runtimeCfg.showLayoutBounds ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F3}, []{
        GetServices().runtimeCfg.showLayoutContentBounds ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F4}, []{
        GetServices().runtimeCfg.showFPS ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F5}, []{
        GetServices().runtimeCfg.showCursorPos ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F6}, []{
        GetServices().runtimeCfg.showPerformance ^= true;
    });
    listener->Bind(InputKeyEvent{InputKey::KEY_F7}, []{
        GetServices().runtimeCfg.showDebugLog ^= true;
    });
    AddChild(std::move(listener));
}
