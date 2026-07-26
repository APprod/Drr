#pragma once
#include "ui/component.hpp"
#include <memory>
#include <optional>

// Acts as a wrapper above single UIComponent, explicitly allows to draw additionally before and after draw calls of original UIComponent
// Can override more functions for example to catch its events if needed, etc. 
// Anything that needs to Enhance funtnionaliity of a single UiComponent but needs to work with any UIComponent child create custom Modifier by deriving from it
class Modifier: public UIComponent{
public:
    using UIComponent::UIComponent;
    template<typename T>
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

enum class OpenDirection { Down, Up, Auto };

class Background: public Modifier{
public:
    template<typename T>
    Background(
        UIComponentSpec spec, 
        BackgroundStyle style,
        T&& child
    ): Modifier(std::forward<T&&>(child), spec),m_style{style}{}

    void OnDrawBefore() override {
        auto& manager = GetServices().resManager;
        auto texture = manager.getTexture(m_style.texture);
        auto drawCall = [&texture, this](){::DrawTexturePro(
            texture,
            rect(texture),
            GetActualRect(),
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
    Popup(std::unique_ptr<UIComponent> child, UIComponentSpec spec = {})
        : Modifier(std::move(child), spec) {}
    Popup&& SetAnchor(std::function<Rectangle()> anchor){m_anchorGetter = anchor; return std::move(*this);}
    Popup&& ParentSize(bool use){m_useParentSize = use; return std::move(*this);}
    Popup&& Direction(OpenDirection d) { m_direction = d; return std::move(*this); }
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
    Rectangle CalculateRect(Rectangle available) {
        Rectangle anchor = m_anchorGetter();
        Rectangle desiredTarget = {
            anchor.x, 0,
            m_desiredSize.x, m_desiredSize.y
        };
        if (m_useParentSize) desiredTarget.width = anchor.width;

        myClamp(desiredTarget.width, 0.0f, available.width);
        myClamp(desiredTarget.height, 0.0f, available.height);

        auto halfDiff = (anchor.width - desiredTarget.width) / 2;
        desiredTarget.x += halfDiff;
        myClamp(desiredTarget.x, available.x, available.x + available.width - desiredTarget.width);

        float availableBottom = available.y + available.height;
        float anchorBottom = anchor.y + anchor.height;

        if (m_direction == OpenDirection::Up) {
            desiredTarget.y = anchor.y - desiredTarget.height;
            float topEdge = desiredTarget.y;
            if (topEdge < available.y)
                desiredTarget.y = available.y;
        } else if (m_direction == OpenDirection::Auto) {
            desiredTarget.y = anchorBottom;
            float bottomEdge = desiredTarget.y + desiredTarget.height;
            if (bottomEdge > availableBottom) {
                desiredTarget.y = anchor.y - desiredTarget.height;
                myClamp(desiredTarget.y, available.y, desiredTarget.y);
            }
        } else {
            desiredTarget.y = anchorBottom;
        }

        return desiredTarget;
    }
private:
    std::function<Rectangle()> m_anchorGetter;
    bool m_useParentSize{false};
    OpenDirection m_direction{OpenDirection::Auto};
};

class AnimatedReveal : public Modifier {
public:
    template<typename T>
    AnimatedReveal(T&& child, UIComponentSpec spec = {},
                   Animated<float> anim = {0.0f, 0.20f, Easing::easeOutCubic},
                   OpenDirection dir = OpenDirection::Down)
        : Modifier(std::forward<T>(child), spec)
        , m_clipHeight(anim)
        , m_revealDirection(dir) {}

    void setOpen(bool open) {
        m_open = open;
        m_clipHeight.setTarget(open ? 1.0f : 0.0f);
    }

    bool isClosingDone() const { return !m_open && m_clipHeight.isDone(); }

    bool OnUpdate(float dt) override {
        m_clipHeight.update(dt);
        return Modifier::OnUpdate(dt);
    }

    bool OnEvent(const MyEvent& event) override {
        auto inClip = [&](Vector2 pos) -> bool {
            auto actual = GetActualRect();
            float h = actual.height * m_clipHeight.current;
            if (h <= 0.0f) return false;
            float clipY = (m_revealDirection == OpenDirection::Up)
                ? actual.y + actual.height - h
                : actual.y;
            return CheckCollisionPointRec(pos, {actual.x, clipY, actual.width, h});
        };

        if (auto* e = std::get_if<CursorActionEvent>(&event)) {
            if (!inClip(e->pos)) return false;
        }
        if (auto* e = std::get_if<CursorMoveEvent>(&event)) {
            if (!inClip(e->pos)) return false;
        }
        if (auto* e = std::get_if<ScreenInterEvent>(&event)) {
            if (!inClip(e->pos)) return false;
        }

        return Modifier::OnEvent(event);
    }

    void OnDrawContent() override {
        auto actual = GetActualRect();
        float h = actual.height * m_clipHeight.current;
        float clipY = (m_revealDirection == OpenDirection::Up)
            ? actual.y + actual.height - h
            : actual.y;
        BeginScissorMode(
            static_cast<int>(actual.x), static_cast<int>(clipY),
            static_cast<int>(actual.width), static_cast<int>(h)
        );
        Modifier::OnDrawContent();
        EndScissorMode();
    }

    void setPopupId(UICompId popupId) { m_popupId = popupId; }

    UIComponent* FindTarget(Vector2 point) override {
        auto actual = GetActualRect();
        float h = actual.height * m_clipHeight.current;
        if (h <= 0.0f) return nullptr;
        float clipY = (m_revealDirection == OpenDirection::Up)
            ? actual.y + actual.height - h
            : actual.y;
        Rectangle clipRect = {actual.x, clipY, actual.width, h};
        if (!CheckCollisionPointRec(point, clipRect))
            return nullptr;
        return Modifier::FindTarget(point);
    }

private:
    Animated<float> m_clipHeight;
    bool m_open = false;
    UICompId m_popupId = 0;
    OpenDirection m_revealDirection{OpenDirection::Down};
};