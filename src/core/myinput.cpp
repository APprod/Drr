#include "core/myinput.hpp"
#include "core/util.hpp"

MyInput::MyInput()
{
}

MyInput::~MyInput()
{
}

std::vector<MyEvent> MyInput::pollEvents(){
    _events.clear();
    { //Mouse events
        Vector2 delta = ::GetMouseDelta();
        Vector2 pos = ::GetMousePosition();
        if (delta != Vector2{0,0}){ //moved
            _events.push_back(CursorMoveEvent{.pos = pos, .delta =  delta});
        }
        for (MouseButton btn : {MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE}) {
            if (IsMouseButtonPressed(btn))
                _events.push_back(CursorActionEvent{pos, btn, true});
            if (IsMouseButtonReleased(btn))
                _events.push_back(CursorActionEvent{pos, btn, false});
        }   
    }
    return _events;
}
