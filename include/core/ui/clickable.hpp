#pragma once

#include "core/ui/component.hpp"

class Clickable : public UIComponent {
public:
    Clickable(UIComponentSpec spec = {});

    void OnHoverEnter() override { m_hover = true; }
    void OnHoverExit() override { m_hover = false; }
    EventMask getCaptureTypes() const override { return EventType::CursorAction; }
    EventResult OnEvent(const MyEvent& event) override;
    bool OnUpdate() override { return false; }

protected:
    bool m_hold = false;
    bool m_hover = false;

    virtual void OnPressed()  {}
    virtual void OnReleased() {}
    virtual void OnClick()    {}
};
