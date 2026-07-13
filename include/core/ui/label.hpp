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
    virtual void OnHoverEnter()override{hovered = true;}
    virtual void OnHoverExit()override{hovered = false;}
protected:
    bool hovered{false};
    Text m_text;
    float m_scrollSpeed{10.f};
    float m_scrollOffset{0.0f};
};
