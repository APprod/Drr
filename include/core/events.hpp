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
struct InputKeyEvent{ //only for typying
    KeyboardKey key{};
    bool pressed{};
};
struct Action{}; // Shortcuts, etc
struct ActionEvent{
    Action key{};
    bool pressed{};
};

using MyEvent = std::variant<CursorActionEvent, CursorMoveEvent, InputKeyEvent,
    ActionEvent, ScreenInterEvent>;
using EventMask = uint32_t;

namespace EventType{
enum EventType : uint32_t {
    CursorMove   = BIT(0),
    CursorAction = BIT(1),
    InputKey     = BIT(2),
    Action       = BIT(3),
    ScreenInter  = BIT(4),
};
}

inline constexpr EventMask getEventType(const MyEvent& e){
    if (std::holds_alternative<CursorMoveEvent>(e))   return EventType::CursorMove;
    else if (std::holds_alternative<CursorActionEvent>(e)) return EventType::CursorAction;
    else if (std::holds_alternative<InputKeyEvent>(e))     return EventType::InputKey;
    else if (std::holds_alternative<ActionEvent>(e))       return EventType::Action;
    else if (std::holds_alternative<ScreenInterEvent>(e)) return EventType::ScreenInter;
    return 0;
}