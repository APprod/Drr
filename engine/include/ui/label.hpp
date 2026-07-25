#pragma once

#include <string>

#include "ui/component.hpp"
#include "rendering/text.hpp"

enum class TextAlign { Left, Center, Right };

class Label: public UIComponent{
public:
    Label(
        Text text,
        UIComponentSpec spec = {},
        TextAlign align = TextAlign::Left
    );
    void SetText(std::string text);
    bool OnEvent(const MyEvent& event) override;
    bool OnUpdate(float dt) override;
    void MeasureContent(Vector2 available) override;
    void OnDrawContent() override;
    void CalculateOffset(float delta);
    float getMaxOffset();
    virtual void OnHoverEnter()override{m_hovered = true;}
    virtual void OnHoverExit()override{m_hovered = false;}
protected:
    Text m_text;
    TextAlign m_textAlign = TextAlign::Left;
    float m_scrollOffset{0.0f};
    bool m_hovered{false};
    bool m_atBottom{false};
public:
    float m_scrollSpeed{20.f};

};

class FPSDraw: public Label{
public:
    FPSDraw(
        Text text,
        UIComponentSpec spec = {}
    ): Label{text, spec}{hitTesting = recievesEvents = false;}
    bool OnUpdate(float dt) override;
};