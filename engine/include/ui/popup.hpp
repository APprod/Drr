#pragma once
#include <functional>
#include <memory>

#include "ui/modifier.hpp"
#include "utils/animated.hpp"

enum class OpenDirection { Down, Up, Auto };

// base wrapper for all popups
class Popup: public Modifier{
public:
    Popup(std::unique_ptr<UIComponent> child, UIComponentSpec spec = {})
        : Modifier(std::move(child), spec) {}

    template<typename T>
    requires (!is_unique_ptr<std::decay_t<T>>::value)
    Popup(T&& child, UIComponentSpec spec = {})
        : Modifier(std::forward<T>(child), spec) {}
    // set parent' position or locaton positon
    Popup&& SetAnchor(std::function<Rectangle()> anchor){m_anchorGetter = anchor; return std::move(*this);}
    Popup&& ParentSize(bool use){m_useParentSize = use; return std::move(*this);}
    Popup&& Direction(OpenDirection d) { m_direction = d; return std::move(*this); }
    template<typename T>
    T* GetChildAs(){return dynamic_cast<T*>(m_child.get());}
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
        } else if (m_direction == OpenDirection::Auto) {
            desiredTarget.y = anchorBottom;
            if (desiredTarget.y + desiredTarget.height > availableBottom)
                desiredTarget.y = anchor.y - desiredTarget.height;
        } else {
            desiredTarget.y = anchorBottom;
        }
        myClamp(desiredTarget.y, available.y, available.y + available.height - desiredTarget.height);

        return desiredTarget;
    }
private:
    std::function<Rectangle()> m_anchorGetter;
    bool m_useParentSize{false};
    OpenDirection m_direction{OpenDirection::Auto};
};
// creates opening animation with scissoring of the drawing output
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
            0, static_cast<int>(clipY),
            GetScreenWidth(), static_cast<int>(h)
        );
        Modifier::OnDrawContent();
        EndScissorMode();
    }

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
    OpenDirection m_revealDirection{OpenDirection::Down};
};
