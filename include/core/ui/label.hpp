#pragma once

#include <string>

#include "core/ui/component.hpp"
#include "core/text.hpp"

class Label: public UIComponent{
public:
    Label(
        Text text,
        UIComponentSpec spec = {}
    );
    void SetText(std::string text);
    EventResult OnEvent(const MyEvent& event) override;
    bool OnUpdate() override;
    void MeasureContent(Vector2 available) override;
    void OnDrawContent() override;
    void CalculateOffset(float delta);
    float getMaxOffset();
    virtual void OnHoverEnter()override{m_hovered = true;}
    virtual void OnHoverExit()override{m_hovered = false;}
protected:
    Text m_text;
    float m_scrollSpeed{10.f};
    float m_scrollOffset{0.0f};
    bool m_hovered{false};
    bool m_atBottom{false};

};

class FPSDraw: public Label{
public:
    using Label::Label;
    bool OnUpdate() override;
};