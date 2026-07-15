/* all functionality in this class */
#pragma once

#include "core/scene.hpp"


class App
{
private:
    int m_screenHeight;
    int m_screenWidth;

public:
    App(int screenWidth = 800, int screenHeight = 450);
    void init();
    void run();
    void frame();
    void close();
    ~App();
};
