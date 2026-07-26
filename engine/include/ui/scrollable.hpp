#pragma once

#include <functional>
#include "input/events.hpp"

struct Scrollable {
    bool OnEvent(const MyEvent& event);
    void OnUpdate(Rectangle drawRect, Vector2 contentSize);
    void OnHover();
    void OnHoverExit();
    void DrawInside(Rectangle actual, std::function<void()> drawCall);

    float scrollSpeed{20};
    float scrollOffset{0};
    float maxOffset{0};
    bool hovered{false};
    bool atBottom{true};
    bool drawBorders{true};
};
