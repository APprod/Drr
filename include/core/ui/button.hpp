#pragma once

#include <functional>
#include <string>

#include "core/ui/component.hpp"
#include "core/text.hpp"

class Button: public UIComponent{
public:
    Button(
        Text text,
        std::function<void()> onClick,
        std::string textureName,
        Vector2 targetSize,
        UIComponentSpec spec = {}
    );
    void OnDrawContent() override;
    EventResult OnEvent(const MyEvent& event) override;
    void OnHoverEnter() override;
    void OnHoverExit() override;
    virtual EventMask getCaptureTypes() const override {return EventType::CursorAction;}
    bool OnUpdate() override {auto r = GetDrawRect();
        m_text.ReMeasure({r.width,r.height});
        if (m_text.IsDirty()) {
            m_text.ClearDirty();
            return true;
        }
        return false;
    }
protected:
    bool m_hold = false;
    bool m_hover = false;
    Text m_text;
    std::function<void()> m_onClick;
    std::string m_textureName;
};
