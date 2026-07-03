/* all functionality in this class */
#pragma once

#include "core/recManager.hpp"
#include "core/myinput.hpp"
#include "core/structs.hpp"
#include "core/scene.hpp"


class App
{
private:
    int m_screenHeight;
    int m_screenWidth;
    SceneManager m_sceneManager;

public:
    App(int screenWidth = 800, int screenHeight = 450);
    void run();
    void close();
    ~App();
};
