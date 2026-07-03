#pragma once
#include <variant>
#include "raylib.h"


struct CursorMoveEvent{
    Vector2 pos{};
    Vector2 delta{};
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

using MyEvent = std::variant<CursorActionEvent, CursorMoveEvent, InputKeyEvent, ActionEvent>;