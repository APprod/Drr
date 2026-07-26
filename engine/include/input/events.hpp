#pragma once
#include <optional>
#include <variant>
#include "raylib.h"
#include "utils/util.hpp"

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };


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
    Vector2 pos{};
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

inline std::string GetInputKeyName(InputKey key) {
    if (key >= KEY_F1 && key <= KEY_F12)
        return "F" + std::to_string(key - KEY_F1 + 1);
    switch (key) {
        case KEY_SPACE:       return "Space";
        case KEY_ESCAPE:      return "Escape";
        case KEY_ENTER:       return "Enter";
        case KEY_TAB:         return "Tab";
        case KEY_BACKSPACE:   return "Backspace";
        case KEY_INSERT:      return "Insert";
        case KEY_DELETE:      return "Delete";
        case KEY_RIGHT:       return "Right";
        case KEY_LEFT:        return "Left";
        case KEY_DOWN:        return "Down";
        case KEY_UP:          return "Up";
        case KEY_PAGE_UP:     return "PageUp";
        case KEY_PAGE_DOWN:   return "PageDown";
        case KEY_HOME:        return "Home";
        case KEY_END:         return "End";
        case KEY_CAPS_LOCK:   return "CapsLock";
        case KEY_SCROLL_LOCK: return "ScrollLock";
        case KEY_NUM_LOCK:    return "NumLock";
        case KEY_PRINT_SCREEN:return "PrintScreen";
        case KEY_PAUSE:       return "Pause";
        case KEY_LEFT_SHIFT:  return "LeftShift";
        case KEY_LEFT_CONTROL:return "LeftControl";
        case KEY_LEFT_ALT:    return "LeftAlt";
        case KEY_LEFT_SUPER:  return "LeftSuper";
        case KEY_RIGHT_SHIFT: return "RightShift";
        case KEY_RIGHT_CONTROL:return "RightControl";
        case KEY_RIGHT_ALT:   return "RightAlt";
        case KEY_RIGHT_SUPER: return "RightSuper";
        case KEY_KB_MENU:     return "Menu";
        default: break;
    }
    const char* raw = GetKeyName(key);
    return raw ? raw : std::to_string(static_cast<int>(key));
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

inline std::optional<Vector2> getEventPos(const MyEvent& event){
    return std::visit(overloaded{
        [](const CursorMoveEvent& e)    -> std::optional<Vector2> { return e.pos; },
        [](const CursorActionEvent& e)  -> std::optional<Vector2> { return e.pos; },
        [](const ScreenInterEvent& e)   -> std::optional<Vector2> { return e.pos; },
        [](const auto&)                 -> std::optional<Vector2> { return std::nullopt; }
    }, event);
}