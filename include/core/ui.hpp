#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "raylib.h"
#include "core/events.hpp"


struct Padding
{
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
};

enum class FillMode {
    FillMaxWidth,
    FillMaxHeight,
    FillMaxSize,
    ByTargetSize
};

struct UIComponentSpec {
    FillMode fillMode = FillMode::ByTargetSize;
    Padding padding{};

    UIComponentSpec& FillMode(FillMode mode){fillMode = mode; return *this;}
    UIComponentSpec& Padding(Padding p){padding = p; return *this;}
};

class UIComponent;
enum class EventResult { NotHandled, Handled, RequireCapture, ReleaseCapture};


class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): compSpec{spec}{}
    virtual ~UIComponent() = default;

    virtual void OnUpdate(){} //potentionally if we want to update status based on dynamic value
    virtual EventResult OnEvent(const MyEvent& ){ return EventResult::NotHandled;} 
    virtual void OnDraw(){}

    virtual void OnMeasure(Vector2 available) final{
        auto vertPad = compSpec.padding.top + compSpec.padding.bottom;
        auto horPad = compSpec.padding.left + compSpec.padding.right;
        Vector2 innerAvailable{
            std::max(0.0f, available.x - horPad),
            std::max(0.0f, available.y - vertPad)
        };
        MeasureContent(innerAvailable);
        desiredSize = {
            contentSize.x + horPad,
            contentSize.y + vertPad
        };
        switch (compSpec.fillMode) {
            case FillMode::FillMaxWidth: desiredSize.x = available.x; break;
            case FillMode::FillMaxHeight: desiredSize.y = available.y; break;
            case FillMode::FillMaxSize: desiredSize = available; break;
            default: break;
        }
    }
    virtual void MeasureContent(Vector2 available) {
        contentSize = {
            std::min(available.x, targetSize.x),
            std::min(available.y, targetSize.y)
        };
    }
    virtual void OnArrange(Rectangle actualRect) final {
        actual = actualRect;
        Rectangle inner = GetDrawRect();
        ArrangeContent(inner);
    }
    virtual void ArrangeContent(Rectangle ) {}
    virtual bool HitTest(Vector2 point) const {return CheckCollisionPointRec(point, actual);}
    virtual UIComponent* FindTarget(Vector2 point);
    virtual EventMask getCaptureTypes() const {return 0;}

    Vector2 DesiredSize(){return desiredSize;}
    Rectangle FinalRect(){return actual;}
    
protected:
    UIComponentSpec compSpec;
    Rectangle actual; 
    Vector2 desiredSize{0,0};
    Vector2 contentSize{0,0};
    Vector2 targetSize{10,10}; //temporary
    bool active = true; //for those wich should be skipped as Layer

    Rectangle GetDrawRect() const{
        return {
            actual.x + compSpec.padding.left, actual.y + compSpec.padding.top,
            actual.width - compSpec.padding.left - compSpec.padding.right,
            actual.height - compSpec.padding.top - compSpec.padding.bottom,
        };
    }

};


enum class Alignment{
    Beginning,
    Center,
    End
};

struct LayoutSpec{
    Alignment align;
    int spacing = 5; //places chids with specified spacing
    LayoutSpec& Alignment(Alignment a) { align = a; return *this; }
    LayoutSpec& Spacing(int s) { spacing = s; return *this; }
};

inline LayoutSpec base{Alignment::Beginning};

class Layout: public UIComponent{ 
public:
    Layout(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
    virtual ~Layout() = default;
    
    template<typename T>
    Layout& Add(T&& child) {
        AddChild(std::make_unique<std::decay_t<T>>(std::forward<T>(child)));
        return *this;
    }
    void AddChild(std::unique_ptr<UIComponent>&& child);
    void OnDraw() override;
    void OnUpdate() override;
    EventResult OnEvent(const MyEvent& event) override;
    const std::vector<std::unique_ptr<UIComponent>>& getChildren() const {return children;}
    virtual UIComponent* FindTarget(Vector2 point) override;
protected:
    LayoutSpec layoutSpec;
    std::vector<std::unique_ptr<UIComponent>> children; //calls their Update/Draw Function

    using Axis = float Vector2::*;
    void MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis);
    void ArrangeAxialLayout(Rectangle actualRect, Axis mainAxis);
};

//TODO (I think no longer todo)
class VerticalLayout: public Layout{
    using Layout::Layout;
    virtual void MeasureContent(Vector2 available) override;
    virtual void ArrangeContent(Rectangle actualRect) override;
};

class HorizontalLayout: public Layout{
    using Layout::Layout;
    virtual void MeasureContent(Vector2 available) override;
    virtual void ArrangeContent(Rectangle actualRect) override;
};


class Stack: public Layout{ // Just to be explicit, actually just Stack layout
public:
    using Layout::Layout;
    virtual void MeasureContent(Vector2 available) override;
    virtual void ArrangeContent(Rectangle rect) override;
};

class Root: public Stack{
public:
    using Stack::Stack;
    virtual EventResult OnEvent(const MyEvent& event) override;
    Vector2 getPos(const MyEvent& event);
private:
    UIComponent* m_captured = nullptr;
};

class Button: public UIComponent{
public:
    Button(
        std::string text,
        std::function<void()> onClick,
        std::string textureName,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void OnUpdate() override;
    void OnDraw() override;
    EventResult OnEvent(const MyEvent& event) override;
    virtual EventMask getCaptureTypes() const override {return EventType::CursorAction | EventType::CursorMove;}
protected: 
    bool m_hold = false;
    bool m_hover = false;
    std::string m_text;
    std::function<void()> m_onClick;
    std::string m_textureName;
};