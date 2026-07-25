#pragma once

#include "ui/component.hpp"

class Clickable : public UIComponent {
public:
    Clickable(UIComponentSpec spec = {});

    void OnHoverEnter() override { m_hover = true; }
    void OnHoverExit() override { m_hover = false; }
    EventMask getCaptureTypes() const override { return EventType::CursorAction; }
    bool OnEvent(const MyEvent& event) override;
    bool OnUpdate(float) override { return false; }

protected:
    bool m_hold = false;
    bool m_hover = false;

    virtual void OnPressed()  {}
    virtual void OnReleased() {}
    virtual void OnClick()    {}
};
