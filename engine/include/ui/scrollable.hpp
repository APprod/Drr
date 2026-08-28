#pragma once

#include <functional>
#include "input/events.hpp"

enum class ScrollDirection{
    Vertical,
    Horizontal
};

struct ScrollThumbGeometry {
    Rectangle track{};      // full track rect (direction-aware)
    Rectangle thumb{};      // current thumb rect at scrollOffset
    float maxThumbPos{};    // trackMajor - thumbMajor (thumb travel range)
};

// use to compose scrollable object
// root component events to this class
struct Scrollable {
    bool OnEvent(const MyEvent& event);
    void OnUpdate(Rectangle drawRect, Vector2 contentSize);
    void OnHover();
    void OnHoverExit();
    // applies scissors and draws borders if needed
    void DrawInside(Rectangle actual, std::function<void()> drawCall);
    void DrawScrollbar(Rectangle viewport);
    ScrollThumbGeometry thumbGeometry(Rectangle viewport) const;

    /* options*/
    float scrollSpeed{20};
    bool drawBorders{true};
    ScrollDirection direction{ScrollDirection::Vertical};
    bool showScrollbar{true};
    float scrollbarWidth{8};
    Color trackColor{50, 50, 50, 200};
    Color thumbColor{150, 150, 150, 120};
    Color thumbHoverColor{200, 200, 200, 200};

    /*state*/
    bool atBottom{false};
    bool dragging{false};
    float scrollOffset{0};
    float maxOffset{0};
    bool hovered{false};
    Rectangle m_lastViewport{};
    Rectangle m_thumbRect{};
    float m_dragStartMouse{0};
    float m_dragStartOffset{0};
    bool m_thumbHovered{false};
};
