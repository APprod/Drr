#pragma once

#include "raylib.h"
#include "ui/spec.hpp"
#include "input/events.hpp"


using UICompId = size_t;
// base for any UIComponent in the layout/UITree. Derive to create custom component
class UIComponent{
public:
    UIComponent(UIComponentSpec spec = {}): m_compSpec{spec}{}
    UIComponent(UIComponent&&) = default;
    UIComponent& operator=(UIComponent&&) = default;
    virtual ~UIComponent();

    const UIComponentSpec& Spec() const {
        return m_compSpec;
    }
    // return true to mark that the relayout is needed
    virtual bool OnUpdate(float){ return false; }
    // returns true if evens is handled to stop propagation
    virtual bool OnEvent(const MyEvent& ){ return false;}
    virtual void OnDraw() final;
    // Override to draw custom content
    virtual void OnDrawContent(){}

    // Measure pass - sets desired size, handles padding
    virtual void OnMeasure(Vector2 available) final{
        auto pad = ResolvePadding(available);
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
    // Measure pass - sets size neede purely for content (padding excluded)
    virtual void MeasureContent(Vector2 available) {
        m_contentDesiredSize = {
            std::min(available.x, m_targetSize.x),
            std::min(available.y, m_targetSize.y)
        };
    }
    // Arrange pass
    virtual void OnArrange(Rectangle actualRect) final {
        m_actual = actualRect;
        Rectangle inner = GetDrawRect();
        ArrangeContent(inner);
    }
    // Arrange all the inner content if needed
    virtual void ArrangeContent(Rectangle ) {}
    template<typename T>
    T* GetAs(){return dynamic_cast<T*>(this);}
    // defines whether event with certain point overlaps with component pos.
    // Should include offset from position offset  
    virtual bool HitTest(Vector2 point) const {Rectangle offsetRect{m_actual.x + positionOffset.x, m_actual.y + positionOffset.y, m_actual.width, m_actual.height}; return CheckCollisionPointRec(point, offsetRect) && CheckCollisionPointRec(point, {0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});}
    // Every component revieves when mouse enters componens content
    virtual void OnHoverEnter(){}
    // Every component revieves when mouse exits componens content
    virtual void OnHoverExit(){}
    // expected: Returns UIComponent that is under the point, or nullptr if none are
    // override for custom behaviour
    virtual UIComponent* FindTarget(Vector2 point);
    // allows full search of specific component
    virtual UIComponent* FindById(UICompId searchId) {
        return (this->id == searchId) ? this : nullptr;
    }
    // return whatever Events should be captured by your component after RequireCapture
    // all events of specified types will be first routed to this component after it cals RequireCapture
    virtual EventMask getCaptureTypes() const {return 0;}

    // defines how padding is calculated
    // override if you want custom padding calculation (dynamically updated for example)
    virtual Padding ResolvePadding(Vector2 dims) const {
        return m_compSpec.ResolvePadding(dims);
    }

    Vector2 DesiredSize(){return m_desiredSize;}
    Rectangle FinalRect(){return m_actual;}
    UICompId id{0}; 
    Vector2 positionOffset{0,0}; //Used for scrolling and non-only-visual changes that shouldn cause rearrange 
    bool recievesEvents = true;
    bool hitTesting = true;
    bool visible = true;
protected:
    UIComponentSpec m_compSpec;
    Rectangle m_actual;
    Vector2 m_desiredSize{0,0};
    Vector2 m_contentDesiredSize{0,0};
    Vector2 m_targetSize{10,10};

    // Get actual rect with padding substracted
    Rectangle GetDrawRect() const{
        auto pad = ResolvePadding({m_actual.width, m_actual.height});
        return {
            m_actual.x + pad.left, m_actual.y + pad.top,
            m_actual.width - pad.left - pad.right,
            m_actual.height - pad.top - pad.bottom,
        };
    }
    // Get actual arranged rect with the proper positionOffset
    Rectangle GetActualRect() const {
        return {m_actual.x + positionOffset.x, m_actual.y + positionOffset.y, m_actual.width, m_actual.height};
    }
    // Get rect where the content should be displayed. Includes offsets
    Rectangle GetVisualRect() const {
        auto base = GetDrawRect();
        return {base.x + positionOffset.x, base.y + positionOffset.y, base.width, base.height};
    }
};

// Used for popups and whatever custom behaviour needed
class IOverlay{
public:
    IOverlay() = default;
    virtual ~IOverlay() = default;
    virtual UICompId PushPopup(std::unique_ptr<UIComponent> comp) = 0;
    virtual UICompId PopPopup() = 0;
    virtual void RemovePopup(UICompId id) = 0;
    virtual void RemovePopupImmediate(UICompId id) = 0;
    virtual UIComponent* GetPopupById(UICompId id) = 0;
};
// Shared context for all UI elements
struct UIContext
{
    static UIContext& Get(){
        static UIContext m_ctx;
        return m_ctx;
    }
    // pushed popup to IOverlay
    template<typename T>
    requires std::is_base_of_v<UIComponent, std::decay_t<T>>
    UICompId PushPopup(T&& comp){
        return PushPopup(std::make_unique<std::decay_t<T>>(std::forward<T>(comp)));
    }
    // pushed popup to IOverlay
    UICompId PushPopup(std::unique_ptr<UIComponent> comp);
    UICompId PopPopup();
    // removes popup after all is updated, does not invalidate iterators 
    void RemovePopup(UICompId id);
    // removes popup from IOverlay. Invalidates layout tree iteration
    void RemovePopupImmediate(UICompId id);
    UIComponent* GetPopupById(UICompId id);

    void SetRoot(UIComponent* root){ m_root = root; }
    template<typename T>
    T* FindById(UICompId id) {
        if (!m_root) return nullptr;
        auto* found = m_root->FindById(id);
        return dynamic_cast<T*>(found);
    }

    void SetOverlay(IOverlay* over){m_overlayStack.push_back(over);}
    void ResetOverlay(){if (!m_overlayStack.empty()) m_overlayStack.pop_back(); }
    IOverlay* GetOverlay() const { return m_overlayStack.empty() ? nullptr : m_overlayStack.back(); }

    // allows root to set capturing component
    void SetCapture(UIComponent* comp){m_captured = comp;}
    EventMask  GetCaptureTypes() const;
    UIComponent*  GetCapturered() const{return m_captured;}
    // allows root to set hoverd component
    void  SetHovered(UIComponent* comp){m_hovered = comp;}
    UIComponent*  GetHovered() const{return m_hovered;}
    // clear captured/hovered state
    void ClearComponent(UIComponent* comp) {
        if (comp == m_captured) m_captured = nullptr;
        if (m_hovered == comp)  m_hovered = nullptr;
    }
    // automatically called in component destructor. Clears the states
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
    std::vector<IOverlay*> m_overlayStack; //Overlay layer contains popups
    UICompId maxId{0}; //unique id
    UIComponent* m_captured = nullptr;
    UIComponent* m_hovered = nullptr;
    UIComponent* m_root = nullptr;
    UIContext() = default;
    UIContext(const UIContext&) = delete;
    UIContext& operator=(const UIContext&) = delete;
};

inline UIContext& GetUIContext() {
    return UIContext::Get();
};

