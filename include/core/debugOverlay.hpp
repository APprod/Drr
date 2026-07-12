#pragma once

#include "core/ui.hpp"

class DebugOverlay: public Stack{
    //Does not consume events
public:
    DebugOverlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
};

class FPSDraw: public Label{
public:
    using Label::Label;
    bool OnUpdate() override;
};

class CursorTrack: public Label{
public:
    using Label::Label;
    virtual EventResult OnEvent(const MyEvent& )override;
    bool OnUpdate() override;
    Vector2 m_pos{0,0};
};