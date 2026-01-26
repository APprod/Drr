/* used only for multi-platform potentialy. just abstraction above inputs */
#pragma once
#include "raylib.h"

class MyInput
{
public:
    MyInput();
    ~MyInput();

    void getInput(Vector2 screenRel);
    Vector2 getPressPos() {return m_pressPos;}
    Vector2 getMoveDelta() {return m_pressMovement;}
    Vector2 getReleasePos() {return m_releasePos;}
    Vector2 getActivePos() {return m_activePos;}
    float getZoomAction() {return m_zoomAction;}
    bool getClick() {return m_click;}
    bool getRelease() {return m_release;}
    bool getHold() {return m_hold;}
private:

    Vector2 m_pressPos;
    Vector2 m_pressMovement;
    Vector2 m_releasePos;
    Vector2 m_activePos;
    float m_zoomAction; // +/-
    bool m_click = false;
    bool m_release = false;
    bool m_hold = false;
};
