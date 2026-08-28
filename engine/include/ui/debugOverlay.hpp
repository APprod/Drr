#pragma once

#include "ui/layout.hpp"
#include "ui/label.hpp"

// overlay with debug vidgets
class DebugOverlay: public Stack{
public:
    DebugOverlay(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = basicLayout);
    bool OnUpdate(float dt) override;
};
// tracks cursor pos
class CursorTrack: public Label{
public:
    using Label::Label;
    virtual bool OnEvent(const MyEvent& )override;
    bool OnUpdate(float dt) override;
    Vector2 m_pos{0,0};
};
// shows current performance data
class PerformanceDisplay: public Label {
    using Label::Label;
    bool OnUpdate(float dt) override;
};

// shows last logged messages
class DebugLogDisplay: public Label {
public:
    DebugLogDisplay(Text text, UIComponentSpec spec = {}) : Label(std::move(text), std::move(spec)) { setAtBottom(true); }
    bool OnUpdate(float dt) override;
};
// shows config
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
// displays bindings tooltip
class BindingsDisplay : public Label {
    using Label::Label;
    bool OnUpdate(float dt) override;
};
