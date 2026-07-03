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
private:
    std::vector<MyEvent> m_events;
};

