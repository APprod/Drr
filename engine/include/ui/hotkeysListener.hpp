#pragma once

#include <functional>
#include <unordered_map>

#include "input/events.hpp"
#include "ui/component.hpp"

struct Binding {
    std::function<void()> callback;
    std::string label;
};

class HotkeysListener : public UIComponent {
public:
    HotkeysListener(UIComponentSpec spec = {}) : UIComponent(spec) {
        visible = false;
        hitTesting = false;
    }

    void Bind(InputKey key, std::function<void()> cb, std::string label = "") {
        m_bindings[key] = {std::move(cb), std::move(label)};
    }
    void Bind(InputKeyEvent event, std::function<void()> cb, std::string label = "") {
        Bind(event.key, std::move(cb), std::move(label));
    }
    void Unbind(InputKey key) { m_bindings.erase(key); }
    void UnbindAll()          { m_bindings.clear(); }

    const std::unordered_map<InputKey, Binding>& GetBindings() const {
        return m_bindings;
    }

    bool OnEvent(const MyEvent& event) override {
        if (auto* e = std::get_if<InputKeyEvent>(&event)) {
            if (e->pressed && m_bindings.contains(e->key)) {
                if (GetServices().runtimeCfg.debug.debugFeaturesAllowed)
                    m_bindings[e->key].callback();
                return true;
            }
        }
        return false;
    }

private:
    std::unordered_map<InputKey, Binding> m_bindings;
};
