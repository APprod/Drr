#pragma once

#include "core/ui/layout.hpp"
#include "core/ui/label.hpp"

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

class HotkeysListener : public UIComponent {
public:
    HotkeysListener(UIComponentSpec spec = {}) : UIComponent(spec) {
        visible = false;     // не рисуется
        interactive = true; // не участвует в hit-test
    }
    void Bind(InputKeyEvent event, std::function<void()> cb) {
        m_bindings[event.key] = std::move(cb);
    }
    EventResult OnEvent(const MyEvent& event) override {
        if (auto* e = std::get_if<InputKeyEvent>(&event)) {
            if (e->pressed && m_bindings.contains(e->key)) {
                if (GetServices().runtimeCfg.debug.debugFeaturesAllowed)
                    m_bindings[e->key]();
                return EventResult::Handled;
            }
        }
        return EventResult::NotHandled;
    }
private:
    std::unordered_map<InputKey, std::function<void()>> m_bindings;
};