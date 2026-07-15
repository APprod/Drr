#include "core/ui/overlay.hpp"
#include "core/services.hpp"

Overlay::Overlay(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
: Stack(uiSpec, layoutSpec) {

    interactive = false;

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
        GetServices().runtimeCfg.debug.showFPS ^= true;
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
}
