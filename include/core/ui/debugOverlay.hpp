#pragma once

#include "core/ui/layout.hpp"
#include "core/ui/label.hpp"
#include "core/ui/checkbox.hpp"
#include "core/ui/slider.hpp"
#include "core/ui/valueLabel.hpp"
#include "core/ui/hotkeysListener.hpp"

class DebugOverlay: public Stack{
    //Does not consume events
public:
    DebugOverlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
    bool OnUpdate()override;
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

class PerformanceDisplay: public Label {
    using Label::Label;
    bool OnUpdate() override;
};

class DebugLogDisplay: public Label {
    using Label::Label;
    bool OnUpdate() override;
};

class CfgDisplay: public Label {
    using Label::Label;
    bool OnUpdate() override;
};

class DebugHorizontalLayout: public HorizontalLayout {
    using HorizontalLayout::HorizontalLayout;
    void OnDrawContent() override;
};

class DebugVerticalLayout: public VerticalLayout {
    using VerticalLayout::VerticalLayout;
    void OnDrawContent() override;
};
