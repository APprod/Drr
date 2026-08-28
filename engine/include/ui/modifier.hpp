#pragma once
#include <memory>
#include <optional>

#include "ui/component.hpp"
#include "rendering/textureSpec.hpp"
#include "utils/animated.hpp"

// Acts as a wrapper above single UIComponent, explicitly allows to draw additionally before and after draw calls of original UIComponent
// Can override more functions for example to catch its events if needed, etc. 
// Anything that needs to Enhance funtnionaliity of a single UiComponent but needs to work with any UIComponent child create custom Modifier by deriving from it
class Modifier: public UIComponent{
public:
    using UIComponent::UIComponent;

    template<typename T> struct is_unique_ptr : std::false_type {};
    template<typename T, typename D> struct is_unique_ptr<std::unique_ptr<T,D>> : std::true_type {};
    template<typename T>
    requires (!is_unique_ptr<std::decay_t<T>>::value)
    Modifier(T&& child, UIComponentSpec spec = {})
        : UIComponent{spec}, 
          m_child{std::make_unique<std::decay_t<T>>(std::forward<T>(child))} {}

    Modifier(std::unique_ptr<UIComponent> child, UIComponentSpec spec = {})
    : UIComponent{spec}, m_child{std::move(child)} {}
    
    
    virtual bool OnUpdate(float dt) override{
        return m_child->OnUpdate(dt);
    }
    virtual bool OnEvent(const MyEvent& event)override{
        return m_child->OnEvent(event);
    }
    virtual void MeasureContent(Vector2 available)override{
        m_child->OnMeasure(available);
        m_contentDesiredSize = m_child->DesiredSize();
    }
    virtual void ArrangeContent(Rectangle actualRect)override{
        m_child->OnArrange(actualRect);
    }

    void OnDrawContent() override{
        OnDrawBefore();
        m_child->OnDraw();
        OnDrawAfter();
    }

    virtual UIComponent* FindTarget(Vector2 point) override
    {
        return m_child->FindTarget(point);
    }

    UIComponent* FindById(UICompId searchId) override {
        if (this->id == searchId) return this;
        if (m_child) return m_child->FindById(searchId);
        return nullptr;
    }

    virtual void OnDrawBefore(){};
    virtual void OnDrawAfter(){};
protected:
    std::unique_ptr<UIComponent> m_child;
};

// puts texture as background begind child
class Background: public Modifier{
public:
    template<typename T>
    Background(
        UIComponentSpec compSpec,
        TextureSpec texSpec,
        T&& child
    ): Modifier(std::forward<T>(child), compSpec), m_spec{std::move(texSpec)} {}

    void OnDrawBefore() override {
        m_spec.Draw(GetActualRect());
    }
    void OnDrawAfter()override{}
protected:
    TextureSpec m_spec;
};