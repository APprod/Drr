#pragma once
#include "core/ui/component.hpp"
#include <memory>
#include <optional>

// Acts as a wrapper above single UIComponent, explicitly allows to draw additionally before and after draw calls of original UIComponent
// Can override more functions for example to catch its events if needed, etc. 
// Anything that needs to Enhance funtnionaliity of a single UiComponent but needs to work with any UIComponent child create custom Modifier by deriving from it
class Modifier: public UIComponent{
public:
    using UIComponent::UIComponent;
    template<typename T>
    Modifier(T&& child,
        UIComponentSpec spec = {}): UIComponent{spec}, m_child{std::make_unique<std::decay_t<T>>(std::forward<T>(child))}{}
    
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

    virtual void OnDrawBefore(){};
    virtual void OnDrawAfter(){};
protected:
    std::unique_ptr<UIComponent> m_child;
};

struct BackgroundStyle
{
    std::string texture{"default"};
    Color tint{WHITE};
    std::optional<ProcessingValues> processing{std::nullopt};
};

class Background: public Modifier{
public:
    template<typename T>
    Background(
        UIComponentSpec spec, 
        BackgroundStyle style,
        T&& child
    ): Modifier(std::forward<T&&>(child), spec),m_style{style}{}

    void OnDrawBefore() override {
        auto& manager = GetServices().recManager;
        auto texture = manager.getTexture(m_style.texture);
        auto drawCall = [&texture, this](){::DrawTexturePro(
            texture,
            rect(texture),
            m_actual,
            {0,0}, 0.0f, m_style.tint
        );};
        if (m_style.processing) {
            auto& proc = m_style.processing.value();
        useShaderUnchecked(
            manager.getShaderProgram("processing"),
            {{"brightness", proc.brightness},
             {"contrast",   proc.contrast},
             {"saturation", proc.saturation},
             {"gamma",      proc.gamma},
             {"alpha",      proc.alpha}},
            drawCall);
        } else {
            drawCall();
        }
    }
    void OnDrawAfter()override{};
protected:
    BackgroundStyle m_style;
};

class Popup: public Modifier{
public:
    using Modifier::Modifier;
    Popup&& SetAnchor(std::function<Rectangle()> anchor){m_anchorGetter = anchor; return std::move(*this);}
    Popup&& ParentSize(bool use){m_useParentSize = use; return std::move(*this);}
    bool OnUpdate(float dt)override{
        auto popupRect = CalculateRect(m_actual);
        m_child->OnArrange(popupRect);
        return m_child->OnUpdate(dt);
    }  
    virtual void ArrangeContent(Rectangle actualRect)override{
        m_actual = actualRect;
        auto popupRect = CalculateRect(actualRect);
        m_child->OnArrange(popupRect);
    }  
    Rectangle CalculateRect(Rectangle available){
        Rectangle anchor = m_anchorGetter();
        Rectangle desiredTarget = {anchor.x, anchor.y + anchor.height, m_desiredSize.x, m_desiredSize.y};
        if (m_useParentSize) {desiredTarget.width = anchor.width;}
        myClamp(desiredTarget.width, 0.0f, available.width);
        myClamp(desiredTarget.height, 0.0f, available.height);

        auto halfDiff = (anchor.width - desiredTarget.width) / 2;
        desiredTarget.x += halfDiff;

        myClamp(desiredTarget.x, available.x, available.x + available.width - desiredTarget.width);

        auto diffY = (available.y + available.height) - (desiredTarget.y + desiredTarget.height);
        if (diffY < 0) desiredTarget.y += diffY;

        return desiredTarget;
    }
private:
    std::function<Rectangle()> m_anchorGetter;
    bool m_useParentSize{false};
};

// Popup(Button(...))
// 
// 
// 