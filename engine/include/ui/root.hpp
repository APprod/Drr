#pragma once

#include "ui/layout.hpp"

// Root component for UI Tree, Handles event propagation, hovering, capturing
class Root: public Stack{
public:
    Root() { GetUIContext().SetRoot(this); }
    ~Root() { GetUIContext().SetRoot(nullptr); }
    virtual bool OnEvent(const MyEvent& event) override;
    void UpdateHover();
    std::optional<bool> CheckCaptured(const MyEvent& event);
private:
    Vector2 m_cursorPos{};
};
