/* used only for different types of input devices potentialy. just abstraction above inputs */
#pragma once
#include <array>
#include <vector>
#include "raylib.h"
#include "events.hpp"

class MyInput
{
public:
    MyInput();
    ~MyInput();
    // resets old events and polls new events after last frame
    std::vector<MyEvent> pollEvents();
    // Can be called multiple times, doesnt reset local copy.
    // Returns all events after last frame
    const std::vector<MyEvent>& getEvents() const {return m_events;}

    bool IsKeyDown(InputKey key) const {
        return m_keyDown[key];
    }

    float GetAxis(InputKey positive, InputKey negative) const {
        if (m_keyDown[positive] == m_keyDown[negative]) return 0.0f;
        return m_keyDown[positive] ? 1.0f : -1.0f;
    }

private:
    std::vector<MyEvent> m_events;
    std::array<bool, InputKey::KEY_KB_MENU + 1> m_keyDown{};
    bool cursorOnScreen = true;
};

