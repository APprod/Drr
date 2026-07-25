#pragma once

#include "ui/layout.hpp"
#include "ui/label.hpp"
#include "ui/checkbox.hpp"
#include "ui/slider.hpp"
#include "ui/valueLabel.hpp"
#include "ui/hotkeysListener.hpp"

class DebugOverlay: public Stack{
    //Does not consume events
public:
    DebugOverlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
    bool OnUpdate(float dt) override;
};

class CursorTrack: public Label{
public:
    using Label::Label;
    virtual bool OnEvent(const MyEvent& )override;
    bool OnUpdate(float dt) override;
    Vector2 m_pos{0,0};
};

class PerformanceDisplay: public Label {
    using Label::Label;
    bool OnUpdate(float dt) override;
};

class DebugLogDisplay: public Label {
    using Label::Label;
    bool OnUpdate(float dt) override;
};

class CfgDisplay: public Label {
    using Label::Label;
    bool OnUpdate(float dt) override;
};

class DebugHorizontalLayout: public HorizontalLayout {
    using HorizontalLayout::HorizontalLayout;
    void OnDrawContent() override;
};

class DebugVerticalLayout: public VerticalLayout {
    using VerticalLayout::VerticalLayout;
    void OnDrawContent() override;
};
