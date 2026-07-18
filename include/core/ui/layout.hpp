#pragma once

#include <memory>
#include <vector>

#include "core/ui/component.hpp"

enum class Alignment{
    Beginning,
    Center,
    End
};

enum class JustifyContent{
    SpaceEvenly,
    None,
};

struct LayoutSpec{
    Alignment align;
    Alignment crossAlign{Alignment::Center};
    JustifyContent justifyContent{JustifyContent::None};
    int spacing = 0;
    bool crossShrink = true;
    bool uniformCross = false;
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
    LayoutSpec& UniformCross() { uniformCross = true; return *this; }
    LayoutSpec& SetJustifyContent(JustifyContent j) { justifyContent = j; return *this; }
    LayoutSpec& JustifyEvenly() { justifyContent = JustifyContent::SpaceEvenly; return *this; }
    LayoutSpec& JustifyNone() { justifyContent = JustifyContent::None; return *this; }
    LayoutSpec& Spacing(int s) { spacing = s; return *this; }
};

inline LayoutSpec base{Alignment::Beginning};

class Layout: public UIComponent{
public:
    template<typename... Ts>
    Layout(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base, Ts&&... children)
    : UIComponent(uiSpec), m_layoutSpec(layoutSpec) {
        (AddChild(std::make_unique<std::decay_t<Ts>>(std::forward<Ts>(children))), ...);
    }
    Layout(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base)
    : UIComponent(uiSpec), m_layoutSpec(layoutSpec){
    }
    
    Layout(Layout&&) = default;
    Layout& operator=(Layout&&) = default;
    virtual ~Layout() = default;

    template<typename... Ts>
    Layout& Add(Ts&&... iChildren) {
        (AddChild(std::make_unique<std::decay_t<Ts>>(std::forward<Ts>(iChildren))), ...);
        return *this;
    }
    UICompId AddChild(std::unique_ptr<UIComponent>&& child);
    bool RemoveChild(UICompId id);
    void OnDrawContent() override;
    bool OnUpdate(float dt) override;
    bool OnEvent(const MyEvent& event) override;
    const std::vector<std::unique_ptr<UIComponent>>& getChildren() const {return m_children;}
    virtual UIComponent* FindTarget(Vector2 point) override;
protected:
    LayoutSpec m_layoutSpec;
    std::vector<std::unique_ptr<UIComponent>> m_children;
    bool m_needsRemeasure = false;

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

class Stack: public Layout{
public:
    using Layout::Layout;
    virtual void MeasureContent(Vector2 available) override;
    virtual void ArrangeContent(Rectangle rect) override;
};
