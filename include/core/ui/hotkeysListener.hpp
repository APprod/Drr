#pragma once

#include <functional>
#include <unordered_map>

#include "core/events.hpp"
#include "core/ui/component.hpp"

class HotkeysListener : public UIComponent {
public:
    HotkeysListener(UIComponentSpec spec = {}) : UIComponent(spec) {
        visible = false;
        hitTesting = false;
    }

    void Bind(InputKey key, std::function<void()> cb) {
        m_bindings[key] = std::move(cb);
    }
    void Bind(InputKeyEvent event, std::function<void()> cb) {
        Bind(event.key, std::move(cb));
    }

    bool OnEvent(const MyEvent& event) override {
        if (auto* e = std::get_if<InputKeyEvent>(&event)) {
            if (e->pressed && m_bindings.contains(e->key)) {
                if (GetServices().runtimeCfg.debug.debugFeaturesAllowed)
                    m_bindings[e->key]();
                return true;
            }
        }
        return false;
    }

private:
    std::unordered_map<InputKey, std::function<void()>> m_bindings;
};
