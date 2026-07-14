#pragma once

#include "raylib.h"
#include "core/ui/spec.hpp"
#include "core/events.hpp"
#include "core/services.hpp"

class UIComponent;
enum class EventResult { NotHandled, Handled, RequireCapture, ReleaseCapture};

class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): m_compSpec{spec}{}
    virtual ~UIComponent() = default;

    const UIComponentSpec& Spec() const {
        return m_compSpec;
    }

    virtual bool OnUpdate(){ return false; }
    virtual EventResult OnEvent(const MyEvent& ){ return EventResult::NotHandled;}
    virtual void OnDraw() final{
        OnDrawContent();
        if (GetServices().runtimeCfg.showLayoutContentBounds){
            auto rec = GetDrawRect();
            DrawRectangleLinesEx(rec,2,RED);
        }
        if (GetServices().runtimeCfg.showLayoutBounds){
            DrawRectangleLinesEx(m_actual,2,RAYWHITE);
        }
    }
    virtual void OnDrawContent(){}

    virtual void OnMeasure(Vector2 available) final{
        auto pad = m_compSpec.ResolvePadding(available);
        auto vertPad = pad.top + pad.bottom;
        auto horPad = pad.left + pad.right;
        Vector2 innerAvailable{
            std::max(0.0f, available.x - horPad),
            std::max(0.0f, available.y - vertPad)
        };
        MeasureContent(innerAvailable);
        m_desiredSize = {
            m_contentDesiredSize.x + horPad,
            m_contentDesiredSize.y + vertPad
        };
        switch (m_compSpec.fillMode) {
            case FillMode::FillMaxWidth: m_desiredSize.x = available.x; break;
            case FillMode::FillMaxHeight: m_desiredSize.y = available.y; break;
            case FillMode::FillMaxSize: m_desiredSize = available; break;
            default: break;
        }
        myClamp(m_desiredSize.x, m_compSpec.minSize.x, m_compSpec.maxSize.x);
        myClamp(m_desiredSize.y, m_compSpec.minSize.y, m_compSpec.maxSize.y);
    }
    virtual void MeasureContent(Vector2 available) {
        m_contentDesiredSize = {
            std::min(available.x, m_targetSize.x),
            std::min(available.y, m_targetSize.y)
        };
    }
    virtual void OnArrange(Rectangle actualRect) final {
        m_actual = actualRect;
        Rectangle inner = GetDrawRect();
        ArrangeContent(inner);
    }
    virtual void ArrangeContent(Rectangle ) {}
    virtual bool HitTest(Vector2 point) const {return CheckCollisionPointRec(point, m_actual) && CheckCollisionPointRec(point, {0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});}
    virtual void OnHoverEnter(){}
    virtual void OnHoverExit(){}
    virtual UIComponent* FindTarget(Vector2 point);
    virtual EventMask getCaptureTypes() const {return 0;}

    Vector2 DesiredSize(){return m_desiredSize;}
    Rectangle FinalRect(){return m_actual;}
    bool interactive = true;
    bool visible = true;
protected:
    UIComponentSpec m_compSpec;
    Rectangle m_actual;
    Vector2 m_desiredSize{0,0};
    Vector2 m_contentDesiredSize{0,0};
    Vector2 m_targetSize{10,10};

    Rectangle GetDrawRect() const{
        auto pad = m_compSpec.ResolvePadding({m_actual.width, m_actual.height});
        return {
            m_actual.x + pad.left, m_actual.y + pad.top,
            m_actual.width - pad.left - pad.right,
            m_actual.height - pad.top - pad.bottom,
        };
    }
};
