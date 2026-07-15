#pragma once

#include <functional>

#include "core/events.hpp"
#include "core/ui/hotkeysListener.hpp"
#include "core/ui/layout.hpp"

class Overlay : public Stack {
public:
    Overlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);

    void Bind(InputKey key, std::function<void()> cb) {
        m_listener->Bind({key, true}, std::move(cb));
    }
    void Bind(InputKeyEvent event, std::function<void()> cb) {
        m_listener->Bind(event, std::move(cb));
    }

private:
    HotkeysListener* m_listener{};
};
