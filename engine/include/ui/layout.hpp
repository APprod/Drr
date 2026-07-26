#pragma once

#include <memory>
#include <vector>

#include <type_traits>
#include <utility>

#include "ui/component.hpp"
#include "ui/scrollable.hpp"
#include "input/events.hpp"

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
    void QueueRemoveChild(UICompId id);
    void OnDrawContent() override;
    bool OnUpdate(float dt) override;
    bool OnEvent(const MyEvent& event) override;
    const std::vector<std::unique_ptr<UIComponent>>& getChildren() const {return m_children;}
    virtual UIComponent* FindTarget(Vector2 point) override;
protected:
    LayoutSpec m_layoutSpec;
    std::vector<std::unique_ptr<UIComponent>> m_children;
    std::vector<UICompId> m_removalQueue;
    bool m_needsRemeasure = false;

    using Axis = float Vector2::*;
    void MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis);
    void ArrangeAxialLayout(Rectangle actualRect, Axis mainAxis, Axis crossAxis);

    std::vector<Vector2> CalculateFlex(Vector2 available, Axis mainAxis, Axis crossAxis, float& spare);
    void ResolveFlex(std::vector<Vector2>& sizes, Vector2 innerDim, Axis mainAxis, float& spare,
        float Flex::*flexField, Vector2 UIComponentSpec::*limitField, float tolerance);
    bool RemoveChild(UICompId id);
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

template<typename LayoutType>
class ScrollView: public LayoutType{
public:
    template<typename... Ts>
    ScrollView(UIComponentSpec uiSpec = {}, LayoutSpec layoutSpec = base, Ts&&... children)
        : LayoutType(std::move(uiSpec), std::move(layoutSpec), std::forward<Ts>(children)...)
    {
        m_scroll.direction = sDir;
    }

    bool OnUpdate(float dt) override{
        this->m_contentDesiredSize.*mainAxis = 0;
        for (auto& child : this->m_children) {
            this->m_contentDesiredSize.*mainAxis += child->DesiredSize().*mainAxis + this->m_layoutSpec.spacing;
        }
        if (!this->m_children.empty())
            this->m_contentDesiredSize.*mainAxis -= this->m_layoutSpec.spacing;
        m_scroll.OnUpdate(this->GetVisualRect(), this->m_contentDesiredSize);
        if (!m_initialScrollApplied && m_scroll.maxOffset > 0){
            if (this->m_layoutSpec.align == Alignment::End)
                m_scroll.scrollOffset = m_scroll.maxOffset;
            else if (this->m_layoutSpec.align == Alignment::Center)
                m_scroll.scrollOffset = m_scroll.maxOffset * 0.5f;
            m_initialScrollApplied = true;
        }
        float stored = this->positionOffset.*mainAxis;
        if (m_scroll.maxOffset > 0)
            this->positionOffset.*mainAxis = -m_scroll.scrollOffset;
        bool dirty = LayoutType::OnUpdate(dt);
        this->positionOffset.*mainAxis = stored;
        return dirty;
    }
    bool OnEvent(const MyEvent& event) override {
        m_scroll.OnUpdate(this->GetVisualRect(), this->m_contentDesiredSize);

        if (auto* e = std::get_if<ScrollEvent>(&event)) {
            m_scroll.hovered = CheckCollisionPointRec(e->pos, this->GetVisualRect());
            if (m_scroll.hovered) return m_scroll.OnEvent(event);
            return false;
        }

        if (m_scroll.OnEvent(event)) {
            if (m_scroll.dragging)
                GetUIContext().SetCapture(this);
            else
                GetUIContext().ReleaseCapture();
            return true;
        }

        return LayoutType::OnEvent(event);
    }
    void OnDrawContent() override{
        auto viewport = this->GetVisualRect();
        m_scroll.DrawInside(
            viewport,
            [this](){
                LayoutType::OnDrawContent();
            }
        );
        m_scroll.DrawScrollbar(viewport);
    }
    UIComponent* FindTarget(Vector2 point) override {
        if (!CheckCollisionPointRec(point, this->GetVisualRect()))
            return nullptr;
        return LayoutType::FindTarget(point);
    }
    EventMask getCaptureTypes() const override {
        return EventType::CursorAction | EventType::CursorMove;
    }
private:
    static constexpr auto sDir =
        std::is_same_v<LayoutType, VerticalLayout> ? ScrollDirection::Vertical : ScrollDirection::Horizontal;
    static constexpr auto mainAxis =
        std::is_same_v<LayoutType, VerticalLayout> ? &Vector2::y : &Vector2::x;

    Scrollable m_scroll;
    bool m_initialScrollApplied = false;
};

using VerticalScrollView = ScrollView<VerticalLayout>;
using HorizontalScrollView = ScrollView<HorizontalLayout>;

class Stack: public Layout{
public:
    using Layout::Layout;
    virtual void MeasureContent(Vector2 available) override;
    virtual void ArrangeContent(Rectangle rect) override;
};
