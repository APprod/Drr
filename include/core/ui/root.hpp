#pragma once

#include <optional>

#include "core/ui/layout.hpp"

class Root: public Stack{
public:
    using Stack::Stack;
    virtual EventResult OnEvent(const MyEvent& event) override;
    void UpdateHover();
    std::optional<EventResult> CheckCaptured(const MyEvent& event);
    Vector2 getPos(const MyEvent& event);
private:
    Vector2 m_cursorPos{};
    UIComponent* m_captured = nullptr;
    UIComponent* m_hovered = nullptr;
};
