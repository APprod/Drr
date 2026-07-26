#pragma once

#include <string>

#include "ui/component.hpp"
#include "ui/scrollable.hpp"
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
    EventMask getCaptureTypes() const override { return EventType::CursorAction | EventType::CursorMove; }
    void MeasureContent(Vector2 available) override;
    void ArrangeContent(Rectangle inner) override;
    void OnDrawContent() override;
    virtual void OnHoverEnter() override {m_scroll.OnHover();}
    virtual void OnHoverExit() override {m_scroll.OnHoverExit();}
protected:
    Text m_text;
    TextAlign m_textAlign = TextAlign::Left;
    Scrollable m_scroll;
};

class FPSDraw: public Label{
public:
    FPSDraw(
        Text text,
        UIComponentSpec spec = {}
    ): Label{text, spec}{hitTesting = recievesEvents = false;}
    bool OnUpdate(float dt) override;
};