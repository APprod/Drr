#pragma once
#include "core/ui/component.hpp"
#include <memory>
#include <optional>

class Modifier: public UIComponent{
public:
    using UIComponent::UIComponent;
    template<typename T>
    Modifier(T&& child,
        UIComponentSpec spec = {}): UIComponent{spec}, m_child{std::make_unique<std::decay_t<T>>(std::forward<T>(child))}{}

    bool OnUpdate(){
        return m_child->OnUpdate();
    }
    EventResult OnEvent(const MyEvent& event){
        return m_child->OnEvent(event);
    }
    virtual void MeasureContent(Vector2 available){
        m_child->OnMeasure(available);
        m_contentDesiredSize = m_child->DesiredSize();
    }
    virtual void ArrangeContent(Rectangle actualRect){
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
    std::string texture;
    Color tint{WHITE};
    std::optional<ProcessingValues> processing;
};

class Background: public Modifier{
public:
    template<typename T>
    Background(
        T&& child,
        UIComponentSpec spec = {}, 
        BackgroundStyle style = {"default", WHITE, std::nullopt}
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