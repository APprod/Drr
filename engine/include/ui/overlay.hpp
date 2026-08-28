#pragma once

#include <functional>
#include <unordered_map>

#include "input/events.hpp"
#include "ui/debugHotkeysListener.hpp"
#include "ui/layout.hpp"
#include "utils/log.hpp"


// basic overlay implementation. derive for custom functionality 
// adds keybindings, popup handling
class Overlay : public Stack, public IOverlay {
public:
    Overlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = basicLayout); 
    virtual ~Overlay();

    void Bind(InputKey key, std::function<void()> cb, std::string label = "") {
        m_listener->Bind({key, true}, std::move(cb), std::move(label));
    }
    void Bind(InputKeyEvent event, std::function<void()> cb, std::string label = "") {
        m_listener->Bind(event, std::move(cb), std::move(label));
    }
    void Unbind(InputKey key) { m_listener->Unbind(key); }
    void UnbindAll()          { m_listener->UnbindAll(); }
    const auto& GetBindings() const { return m_listener->GetBindings(); }
    UICompId PushPopup(std::unique_ptr<UIComponent> comp) override;
    UICompId PopPopup() override;
    void RemovePopup(UICompId compId) override;
    void RemovePopupImmediate(UICompId compId) override;
    UIComponent* GetPopupById(UICompId id) override;

    bool OnEvent(const MyEvent& event) override;

private:
    void CloseTopPopup();
    HotkeysListener* m_listener{};
    std::vector<UICompId> ids;
};
