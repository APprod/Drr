#pragma once

#include <vector>
#include <memory>

#include "raylib.h"
#include "core/util.hpp" //temp

struct Padding
{
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;
};

class UIComponent{
public:
    UIComponent() = default;
    virtual ~UIComponent() = default;

    virtual void OnUpdate(){} //potentionally OnEvent/or checking it's own status
    virtual void OnDraw(){}

    virtual void OnMeasure(Vector2 available){
        desiredSize = {std::min(available.x, targetSize.x),
                        std::min(available.y, targetSize.y)};
    }
    virtual void OnArrange(Rectangle actualRect){
        actual = actualRect;
    }
    Vector2 DesiredSize(){return desiredSize;}
    Rectangle FinalRect(){return actual;}
    
protected:
    Rectangle actual; 
    Vector2 desiredSize;
    Vector2 targetSize; //temporary
    Padding padding; // Each component should draw itself according to padding
    bool active; //for those wich should be skipped as Layer

    // Vector2 GetDrawDimensions() helpful function that offest padding
    // Vector2 GetDrawPos() helpful function that offests by padding
};

class Layout: public UIComponent{ 
public:
    virtual ~Layout() = default;
    void AddChild(std::unique_ptr<UIComponent>&& child);
    void OnDraw() override;
    void OnUpdate() override;
protected:
    std::vector<std::unique_ptr<UIComponent>> childs; //calls their Update/Draw Function
    int spacing = 5; //places chids with specified spacing
};

//TODO
class VerticalLayout: public Layout{
    virtual void OnMeasure(Vector2 available) override;
    virtual void OnArrange(Rectangle actualRect) override;
};

class HorizontalLayout: public Layout{
    virtual void OnMeasure(Vector2 available) override;
    virtual void OnArrange(Rectangle actualRect) override;
};

class Root: public Layout{ // Just to be explicit, actually just Stack layout
public:
    virtual void OnMeasure(Vector2 available) override{
        desiredSize = available;
        for (auto&child: childs){
            child->OnMeasure(available);
        }
    }
    virtual void OnArrange(Rectangle rect) override{
        for (auto&child: childs){
            child->OnArrange(rect);
        }
    }
};
