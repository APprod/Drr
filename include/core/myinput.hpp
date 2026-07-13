/* used only for different types of input devices potentialy. just abstraction above inputs */
#pragma once
#include <vector>
#include "raylib.h"
#include "events.hpp"

class MyInput
{
public:
    MyInput();
    ~MyInput();
    std::vector<MyEvent> pollEvents();
    //Can be called after call. Retrieves all events between frames
    std::vector<MyEvent> getEvents(){return m_events;}

    bool IsKeyDown(InputKey key) const {
        return m_keyDown[key];
    }

private:
    std::vector<MyEvent> m_events;
    std::array<bool, InputKey::KEY_KB_MENU + 1> m_keyDown{};
    bool cursorOnScreen = true;
};

