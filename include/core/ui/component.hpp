#pragma once

#include "raylib.h"
#include "core/ui/spec.hpp"
#include "core/input/events.hpp"
#include "core/services.hpp"


using UICompId = size_t;

class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): m_compSpec{spec}{}
    UIComponent(UIComponent&&) = default;
    UIComponent& operator=(UIComponent&&) = default;
    virtual ~UIComponent();

    const UIComponentSpec& Spec() const {
        return m_compSpec;
    }

    virtual bool OnUpdate(float){ return false; }
    virtual bool OnEvent(const MyEvent& ){ return false;}
    virtual void OnDraw() final{
        OnDrawContent();
        if (GetServices().runtimeCfg.debug.showLayoutContentBounds){
            auto rec = GetDrawRect();
            DrawRectangleLinesEx(rec,2,RED);
        }
        if (GetServices().runtimeCfg.debug.showLayoutBounds){
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
    UICompId id{0}; 
    bool recievesEvents = true;
    bool hitTesting = true;
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

class IOverlay{
public:
    IOverlay() = default;
    virtual ~IOverlay() = default;
    virtual UICompId PushPopup(std::unique_ptr<UIComponent> comp) = 0;
    virtual UICompId PopPopup() = 0;
    virtual void RemovePopup(UICompId id) = 0;
};

struct UIContext
{
    static UIContext& Get(){
        static UIContext m_ctx;
        return m_ctx;
    }
    template<typename T>
    requires std::is_base_of_v<UIComponent, std::decay_t<T>>
    UICompId PushPopup(T&& comp){
        return PushPopup(std::make_unique<std::decay_t<T>>(std::forward<T>(comp)));
    }
    UICompId PushPopup(std::unique_ptr<UIComponent> comp);
    UICompId PopPopup();
    void RemovePopup(UICompId id);

    void SetOverlay(IOverlay* over){m_overlayStack.push_back(over);}
    void ResetOverlay(){if (!m_overlayStack.empty()) m_overlayStack.pop_back(); }

    void SetCapture(UIComponent* comp){m_captured = comp;}
    EventMask  GetCaptureTypes() const;
    UIComponent*  GetCapturered() const{return m_captured;}
    UIComponent*  GetHovered() const{return m_hovered;}
    void  SetHovered(UIComponent* comp){m_hovered = comp;}
    void ClearComponent(UIComponent* comp) {
        if (comp == m_captured) m_captured = nullptr;
        if (m_hovered == comp)  m_hovered = nullptr;
    }
    void InvalidateComponent(UIComponent* comp) {
        if (comp == m_captured) ReleaseCapture();
        if (m_hovered == comp) {
            m_hovered->OnHoverExit();
            m_hovered = nullptr;
        }
    }
    void ReleaseCapture(){m_captured = nullptr;}
    UICompId nextId(){return ++maxId;}
private:
    std::vector<IOverlay*> m_overlayStack;
    UICompId maxId{0}; 
    UIComponent* m_captured = nullptr;
    UIComponent* m_hovered = nullptr;
    UIContext() = default;
    UIContext(const UIContext&) = delete;
    UIContext& operator=(const UIContext&) = delete;
};

inline UIContext& GetUIContext() {
    return UIContext::Get();
};

