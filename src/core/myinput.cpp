#include "core/myinput.hpp"
#include "core/util.hpp"

MyInput::MyInput()
{
}

MyInput::~MyInput()
{
}

std::vector<MyEvent> MyInput::pollEvents(){
    m_events.clear();
    { //Mouse events
        Vector2 delta = ::GetMouseDelta();
        Vector2 pos = ::GetMousePosition();
        if (delta != Vector2{0,0}){ //moved
            m_events.push_back(CursorMoveEvent{.pos = pos, .delta =  delta});
        }
        for (MouseButton btn : {MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE}) {
            if (IsMouseButtonPressed(btn))
                m_events.push_back(CursorActionEvent{pos, btn, true});
            if (IsMouseButtonReleased(btn))
                m_events.push_back(CursorActionEvent{pos, btn, false});
        }   
        if (cursorOnScreen){
            if (!IsCursorOnScreen()){
                cursorOnScreen = false;
                m_events.push_back(ScreenInterEvent{.action = ScreenInteraction::EXIT});
            };
        }else{
            if (IsCursorOnScreen()){
                cursorOnScreen = true;
                m_events.push_back(ScreenInterEvent{.pos = pos, .action = ScreenInteraction::ENTER});
            };
        }
        
    }
    return m_events;
}
