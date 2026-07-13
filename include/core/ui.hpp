#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <numeric>
#include "raylib.h"
#include "core/events.hpp"
#include "core/structs.hpp"
#include "core/text.hpp"


struct Padding
{
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
};

struct PaddingPct
{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

enum class FillMode {
    FillMaxWidth,
    FillMaxHeight,
    FillMaxSize,
    ByTargetSize
};

struct Flex{
    float growth{0};
    float shrink{0};
};

struct UIComponentSpec {
    FillMode fillMode = FillMode::ByTargetSize;
    Padding padding{};
    PaddingPct paddingPct{};
    Flex flex{0,0};
    Vector2 minSize{0,0};
    Vector2 maxSize{std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};

    UIComponentSpec& SetFlex(Flex iflex){flex = iflex; return *this;}
    UIComponentSpec& MinSize(Vector2 size){minSize = size; return *this;}
    UIComponentSpec& MaxSize(Vector2 size){maxSize = size; return *this;}
    UIComponentSpec& FillMode(FillMode mode){fillMode = mode; return *this;}
    UIComponentSpec& FillMaxWidth(){fillMode = FillMode::FillMaxWidth; return *this;}
    UIComponentSpec& FillMaxHeight(){fillMode = FillMode::FillMaxHeight; return *this;}
    UIComponentSpec& FillMaxSize(){fillMode = FillMode::FillMaxSize; return *this;}
    UIComponentSpec& FillTargetSize(){fillMode = FillMode::ByTargetSize; return *this;}
    UIComponentSpec& SetPadding(Padding p){padding = p; return *this;}
    UIComponentSpec& SetPaddingPct(PaddingPct p){paddingPct = p; return *this;}

    Padding ResolvePadding(Vector2 dims) const;
};
using UICSpec = UIComponentSpec;

class UIComponent;
enum class EventResult { NotHandled, Handled, RequireCapture, ReleaseCapture};


class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): compSpec{spec}{}
    virtual ~UIComponent() = default;

    const UIComponentSpec& Spec() const {
        return compSpec;
    }

    virtual bool OnUpdate(){ return false; } //potentionally if we want to update status based on dynamic value
    virtual EventResult OnEvent(const MyEvent& ){ return EventResult::NotHandled;} 
    virtual void OnDraw() final{
        OnDrawContent();
        if (GetServices().runtimeCfg.showLayoutContentBounds){
            auto rec = GetDrawRect();
            DrawRectangleLinesEx(rec,2,RED);
        }
        if (GetServices().runtimeCfg.showLayoutBounds){
            DrawRectangleLinesEx(actual,2,RAYWHITE);
        }
    }
    virtual void OnDrawContent(){}

    virtual void OnMeasure(Vector2 available) final{
        auto pad = compSpec.ResolvePadding(available);
        auto vertPad = pad.top + pad.bottom;
        auto horPad = pad.left + pad.right;
        Vector2 innerAvailable{
            std::max(0.0f, available.x - horPad),
            std::max(0.0f, available.y - vertPad)
        };
        MeasureContent(innerAvailable);
        desiredSize = {
            contentDesiredSize.x + horPad,
            contentDesiredSize.y + vertPad
        };
        switch (compSpec.fillMode) {
            case FillMode::FillMaxWidth: desiredSize.x = available.x; break;
            case FillMode::FillMaxHeight: desiredSize.y = available.y; break;
            case FillMode::FillMaxSize: desiredSize = available; break;
            default: break;
        }
        myClamp(desiredSize.x, compSpec.minSize.x, compSpec.maxSize.x);
        myClamp(desiredSize.y, compSpec.minSize.y, compSpec.maxSize.y);
    }
    virtual void MeasureContent(Vector2 available) {
        contentDesiredSize = {
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
    virtual bool HitTest(Vector2 point) const {return CheckCollisionPointRec(point, actual) && CheckCollisionPointRec(point, {0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});}
    virtual void OnHoverEnter(){}
    virtual void OnHoverExit(){}
    virtual UIComponent* FindTarget(Vector2 point);
    virtual EventMask getCaptureTypes() const {return 0;}

    Vector2 DesiredSize(){return desiredSize;}
    Rectangle FinalRect(){return actual;}
    bool interactive = true; //for those who want to recieve events
    bool visible = true; //for those wich should be skipped as Layer
protected:
    UIComponentSpec compSpec;
    Rectangle actual; //Result after arrange - all available, actual location
    Vector2 desiredSize{0,0}; //What gets after its measured
    Vector2 contentDesiredSize{0,0}; //Result of OnMeasure Size of all the contents, excluding padding
    Vector2 targetSize{10,10}; //Target size in case Ui element uses it, may replaced by min/max size


    Rectangle GetDrawRect() const{
        auto pad = compSpec.ResolvePadding({actual.width, actual.height});
        return {
            actual.x + pad.left, actual.y + pad.top,
            actual.width - pad.left - pad.right,
            actual.height - pad.top - pad.bottom,
        };
    }

};


enum class Alignment{
    Beginning,
    Center,
    End
};
//Defines how components will be spaced out if they don't acquire all the available place
enum class JustifyContent{
    SpaceEvenly,
    None,
};


struct LayoutSpec{
    Alignment align;
    Alignment crossAlign{Alignment::Center};
    JustifyContent justifyContent{JustifyContent::None};
    int spacing = 5; //places chids with specified spacing
    bool crossShrink = true;
    LayoutSpec& SetAlign(Alignment a) { align = a; return *this; }
    LayoutSpec& AlignBegin() { align = Alignment::Beginning; return *this; }
    LayoutSpec& AlignCenter() { align = Alignment::Center; return *this; }
    LayoutSpec& AlignEnd() { align = Alignment::End; return *this; }
    LayoutSpec& CrossAlign(Alignment a) { crossAlign = a; return *this; }
    LayoutSpec& CrossBegin() { crossAlign = Alignment::Beginning; return *this; }
    LayoutSpec& CrossCenter() { crossAlign = Alignment::Center; return *this; }
    LayoutSpec& CrossEnd() { crossAlign = Alignment::End; return *this; }
    LayoutSpec& CrossShrink(bool a) { crossShrink = a; return *this; }
    LayoutSpec& ShrinkOn() { crossShrink = true; return *this; }
    LayoutSpec& ShrinkOff() { crossShrink = false; return *this; }
    LayoutSpec& JustifyContent(JustifyContent j) { justifyContent = j; return *this; }
    LayoutSpec& JustifyEvenly() { justifyContent = JustifyContent::SpaceEvenly; return *this; }
    LayoutSpec& JustifyNone() { justifyContent = JustifyContent::None; return *this; }
    LayoutSpec& Spacing(int s) { spacing = s; return *this; }
};

inline LayoutSpec base{Alignment::Beginning};

class Layout: public UIComponent{ 
public:
    Layout(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base);
    virtual ~Layout() = default;
    
    template<typename... Ts>
    Layout& Add(Ts&&... children) {
        (AddChild(std::make_unique<std::decay_t<Ts>>(std::forward<Ts>(children))), ...);
        return *this;
    }
    void AddChild(std::unique_ptr<UIComponent>&& child);
    void OnDrawContent() override;
    bool OnUpdate() override;
    EventResult OnEvent(const MyEvent& event) override;
    const std::vector<std::unique_ptr<UIComponent>>& getChildren() const {return children;}
    virtual UIComponent* FindTarget(Vector2 point) override;
protected:
    LayoutSpec layoutSpec;
    std::vector<std::unique_ptr<UIComponent>> children; //calls their Update/Draw Function

    using Axis = float Vector2::*;
    void MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis);
    void ArrangeAxialLayout(Rectangle actualRect, Axis mainAxis, Axis crossAxis);

    std::vector<Vector2> CalculateFlex(Vector2 available, Axis mainAxis, Axis crossAxis, float& spare);
    void ResolveFlex(std::vector<Vector2>& sizes, Vector2 innerDim, Axis mainAxis, float& spare,
        float Flex::*flexField, Vector2 UIComponentSpec::*limitField, float tolerance);
};

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
    void UpdateHover();
    std::optional<EventResult> CheckCaptured(const MyEvent& event);
    Vector2 getPos(const MyEvent& event);
private:
    Vector2 m_cursorPos{};
    UIComponent* m_captured = nullptr;
    UIComponent* m_hovered = nullptr;
};

class Button: public UIComponent{
public:
    Button(
        Text text,
        std::function<void()> onClick,
        std::string textureName,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void OnDrawContent() override;
    EventResult OnEvent(const MyEvent& event) override;
    void OnHoverEnter() override;
    void OnHoverExit() override;
    virtual EventMask getCaptureTypes() const override {return EventType::CursorAction;}
    bool OnUpdate() override {auto r = GetDrawRect();
        m_text.ReMeasure({r.width,r.height});
        if (m_text.IsDirty()) {
            m_text.ClearDirty();
            return true;
        }
        return false;
    }
protected: 
    bool m_hold = false;
    bool m_hover = false;
    Text m_text;
    std::function<void()> m_onClick;
    std::string m_textureName;
};

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