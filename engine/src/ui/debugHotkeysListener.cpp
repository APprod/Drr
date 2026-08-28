#include "ui/debugHotkeysListener.hpp"
#include "services.hpp"
bool DebugHotkeysListener::OnEvent(const MyEvent& event)  {
    if (auto* e = std::get_if<InputKeyEvent>(&event)) {
        if (e->pressed && !e->repeat && m_bindings.contains(e->key)) {
            if (GetServices().debugFlags.debugFeaturesAllowed)
                m_bindings[e->key].callback();
            return true;
        }
    }
    return false;
}