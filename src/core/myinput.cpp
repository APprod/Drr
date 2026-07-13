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
        auto wheel = ::GetMouseWheelMoveV();
        if (wheel.x != 0 || wheel.y != 0){
            m_events.push_back(ScrollEvent{ wheel });
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
    {//Keyboard events
        while (int key = GetKeyPressed()) {
            m_keyDown[key] = true;
            m_events.push_back(InputKeyEvent{static_cast<InputKey>(key), true});
        }
        for (int key = KEY_NULL; key <= KEY_KB_MENU; ++key) {
            if (IsKeyReleased((KeyboardKey)key)) {
                m_keyDown[key] = false;
                m_events.push_back(InputKeyEvent{(InputKey)key, false});
            }
        }
    }
    return m_events;
}
