#pragma once

#include <functional>
#include <unordered_map>

#include "core/events.hpp"
#include "core/ui/hotkeysListener.hpp"
#include "core/ui/layout.hpp"
#include "core/debug.hpp"



class Overlay : public Stack, public IOverlay {
public:
    Overlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base); 
    virtual ~Overlay();

    void Bind(InputKey key, std::function<void()> cb) {
        m_listener->Bind({key, true}, std::move(cb));
    }
    void Bind(InputKeyEvent event, std::function<void()> cb) {
        m_listener->Bind(event, std::move(cb));
    }
    UICompId PushPopup(std::unique_ptr<UIComponent> comp) override;
    UICompId PopPopup() override;
    bool RemovePopup(UICompId id) override;

private:
    HotkeysListener* m_listener{};
    std::vector<UICompId> ids;
};
