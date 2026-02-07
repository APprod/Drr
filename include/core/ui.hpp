#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "raylib.h"


struct Padding
{
    int up = 0;
    int down = 0;
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

class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): compSpec{spec}{}
    virtual ~UIComponent() = default;

    virtual void OnUpdate(){} //potentionally OnEvent/or checking it's own status
    virtual void OnDraw(){}

    virtual void OnMeasure(Vector2 available){ //default behaviour
        //left here for now in case I will want to override this This
        switch (compSpec.fillMode) {
            case FillMode::FillMaxWidth:
                desiredSize = {available.x, std::min(available.y, targetSize.y)};
                break;
            case FillMode::FillMaxHeight:
                desiredSize = {std::min(available.x, targetSize.x), available.y};
                break;
            case FillMode::FillMaxSize:
                desiredSize = available;
                break;
            case FillMode::ByTargetSize:
            default:
                desiredSize = {std::min(available.x, targetSize.x),
                               std::min(available.y, targetSize.y)};
                break;
        }
    }
    virtual void OnArrange(Rectangle actualRect){
        actual = actualRect;
    }
    Vector2 DesiredSize(){return desiredSize;}
    Rectangle FinalRect(){return actual;}
    
protected:
    UIComponentSpec compSpec;
    Rectangle actual; 
    Vector2 desiredSize;
    Vector2 targetSize; //temporary
    bool active; //for those wich should be skipped as Layer

    // Vector2 GetDrawDimensions() helpful function that offest padding
    // Vector2 GetDrawPos() helpful function that offests by padding
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
    void AddChild(std::unique_ptr<UIComponent>&& child);
    void OnDraw() override;
    void OnUpdate() override;
protected:
    LayoutSpec layoutSpec;
    std::vector<std::unique_ptr<UIComponent>> childs; //calls their Update/Draw Function
    Vector2 contentSize; //Measured Size of all children 
    
    using Axis = float Vector2::*;
    void MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis);
    void ArrangeAxialLayout(Rectangle actualRect, Axis mainAxis);
};

//TODO
class VerticalLayout: public Layout{
    using Layout::Layout;
    virtual void OnMeasure(Vector2 available) override;
    virtual void OnArrange(Rectangle actualRect) override;
};

class HorizontalLayout: public Layout{
    using Layout::Layout;
    virtual void OnMeasure(Vector2 available) override;
    virtual void OnArrange(Rectangle actualRect) override;
};

class Root: public Layout{ // Just to be explicit, actually just Stack layout
public:
    using Layout::Layout;
    virtual void OnMeasure(Vector2 available) override;
    virtual void OnArrange(Rectangle rect) override;
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
protected:
    std::string m_text;
    std::function<void()> m_onClick;
    std::string m_textureName;
};