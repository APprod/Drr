#pragma once
#include <variant>
#include "raylib.h"
#include "util.hpp"


struct CursorMoveEvent{
    Vector2 pos{};
    Vector2 delta{};
};
enum class ScreenInteraction{
    ENTER,
    EXIT,
};
struct ScreenInterEvent{
    Vector2 pos{0,0};
    ScreenInteraction action;
};

using CursorAction = MouseButton; // temporary
struct CursorActionEvent{
    Vector2 pos{};
    CursorAction button{};
    bool pressed{};
};

using InputKey = KeyboardKey;
struct InputKeyEvent{ //only for typying
    InputKey key{};
    bool pressed{};
};

struct Action{}; // Shortcuts, etc
struct ActionEvent{
    Action key{};
    bool pressed{};
};

struct ScrollEvent{
    Vector2 delta{};
};

using MyEvent = std::variant<CursorActionEvent, CursorMoveEvent, InputKeyEvent,
    ActionEvent, ScreenInterEvent, ScrollEvent>;
using EventMask = uint32_t;

namespace EventType{
enum EventType : uint32_t {
    CursorMove   = BIT(0),
    CursorAction = BIT(1),
    InputKey     = BIT(2),
    Action       = BIT(3),
    ScreenInter  = BIT(4),
    Scroll       = BIT(5),
};
}

inline constexpr EventMask getEventType(const MyEvent& e){
    if (std::holds_alternative<CursorMoveEvent>(e))   return EventType::CursorMove;
    else if (std::holds_alternative<CursorActionEvent>(e)) return EventType::CursorAction;
    else if (std::holds_alternative<InputKeyEvent>(e))     return EventType::InputKey;
    else if (std::holds_alternative<ActionEvent>(e))       return EventType::Action;
    else if (std::holds_alternative<ScreenInterEvent>(e)) return EventType::ScreenInter;
    else if (std::holds_alternative<ScrollEvent>(e))       return EventType::Scroll;
    return 0;
}