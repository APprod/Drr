/* all functionality in this class */
#pragma once

#include "core/recManager.hpp"
// #include "core/logic.hpp"
#include "core/myinput.hpp"
// #include "core/renderer.hpp"
#include "core/structs.hpp"
// #include "app/scenes/scene.hpp"
#include "core/scene.hpp"
// #include "ui/ui.hpp"


class App
{
private:
    int m_screenHeight;
    int m_screenWidth;

    // Renderer m_renderer;
    // LogicController m_controller;
    RecourceManager m_manager;
    MyInput m_input;
    SceneManager m_sceneManager;
    // GData gData;
    // MyUI m_ui;
    // SceneManager m_sceneManager;

public:
    App(int screenWidth = 800, int screenHeight = 450);
    void run();
    void close();
    ~App();
};
