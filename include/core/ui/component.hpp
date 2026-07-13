#pragma once

#include "raylib.h"
#include "core/ui/spec.hpp"
#include "core/events.hpp"
#include "core/services.hpp"

class UIComponent;
enum class EventResult { NotHandled, Handled, RequireCapture, ReleaseCapture};

class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): compSpec{spec}{}
    virtual ~UIComponent() = default;

    const UIComponentSpec& Spec() const {
        return compSpec;
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
            DrawRectangleLinesEx(actual,2,RAYWHITE);
        }
    }
    virtual void OnDrawContent(){}

    virtual void OnMeasure(Vector2 available) final{
        auto pad = compSpec.ResolvePadding(available);
        auto vertPad = pad.top + pad.bottom;
        auto horPad = pad.left + pad.right;
        Vector2 innerAvailable{
            std::max(0.0f, available.x - horPad),
            std::max(0.0f, available.y - vertPad)
        };
        MeasureContent(innerAvailable);
        desiredSize = {
            contentDesiredSize.x + horPad,
            contentDesiredSize.y + vertPad
        };
        switch (compSpec.fillMode) {
            case FillMode::FillMaxWidth: desiredSize.x = available.x; break;
            case FillMode::FillMaxHeight: desiredSize.y = available.y; break;
            case FillMode::FillMaxSize: desiredSize = available; break;
            default: break;
        }
        myClamp(desiredSize.x, compSpec.minSize.x, compSpec.maxSize.x);
        myClamp(desiredSize.y, compSpec.minSize.y, compSpec.maxSize.y);
    }
    virtual void MeasureContent(Vector2 available) {
        contentDesiredSize = {
            std::min(available.x, targetSize.x),
            std::min(available.y, targetSize.y)
        };
    }
    virtual void OnArrange(Rectangle actualRect) final {
        actual = actualRect;
        Rectangle inner = GetDrawRect();
        ArrangeContent(inner);
    }
    virtual void ArrangeContent(Rectangle ) {}
    virtual bool HitTest(Vector2 point) const {return CheckCollisionPointRec(point, actual) && CheckCollisionPointRec(point, {0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});}
    virtual void OnHoverEnter(){}
    virtual void OnHoverExit(){}
    virtual UIComponent* FindTarget(Vector2 point);
    virtual EventMask getCaptureTypes() const {return 0;}

    Vector2 DesiredSize(){return desiredSize;}
    Rectangle FinalRect(){return actual;}
    bool interactive = true;
    bool visible = true;
protected:
    UIComponentSpec compSpec;
    Rectangle actual;
    Vector2 desiredSize{0,0};
    Vector2 contentDesiredSize{0,0};
    Vector2 targetSize{10,10};

    Rectangle GetDrawRect() const{
        auto pad = compSpec.ResolvePadding({actual.width, actual.height});
        return {
            actual.x + pad.left, actual.y + pad.top,
            actual.width - pad.left - pad.right,
            actual.height - pad.top - pad.bottom,
        };
    }
};
