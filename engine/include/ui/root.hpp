#pragma once

#include <optional>

#include "ui/layout.hpp"

class Root: public Stack{
public:
    using Stack::Stack;
    virtual bool OnEvent(const MyEvent& event) override;
    void UpdateHover();
    std::optional<bool> CheckCaptured(const MyEvent& event);
    Vector2 getPos(const MyEvent& event);
private:
    Vector2 m_cursorPos{};
};
