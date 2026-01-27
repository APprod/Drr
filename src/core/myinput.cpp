#include "core/myinput.hpp"
#include "core/util.hpp"

MyInput::MyInput()
{
}

MyInput::~MyInput()
{
}

void MyInput::getInput(Vector2 screenRel)
{
    m_click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT); 
    m_hold = IsMouseButtonDown(MOUSE_BUTTON_LEFT); 
    m_release = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    m_pressMovement = GetMouseDelta()/screenRel;
    m_activePos = GetMousePosition()/screenRel;
    m_zoomAction = GetMouseWheelMove();
}