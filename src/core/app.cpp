#include "core/app.hpp"
#include "core/debug.hpp"
#include "raylib.h"
#include "core/util.hpp"
#include "app/scenes/testScene.hpp"
// #include "app/scenes/menu.hpp"
// #include "app/scenes/scene.hpp"
// #include "app/scenes/mainScene.hpp"
#include <memory>

// static bool DEB_PR = true;
static bool DEB_PR = false;

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}

void App::run()
{
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::ConsoleSink>(dbg::ConsoleSink()));

    dbg::GetLogger().DebugInfo("app:run \n");
    dbg::GetLogger().DebugInfo("Running direcotry: ", GetWorkingDirectory());
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
    {//temp
        InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
    
                // setScreenResolution(Resolution::S_MEDIUM);
                // setScreenSize();
                
                SetWindowPosition(5,20);
                SetWindowState(::FLAG_WINDOW_RESIZABLE);
                
                
                // SetTargetFPS(4000);
                SetTargetFPS(60);
                auto m = GetCurrentMonitor();    // Get current monitor where window is placed
                SetTargetFPS(GetMonitorRefreshRate(m));
                SetWindowState(::FLAG_WINDOW_MAXIMIZED); 
    }
    m_manager.init();
    // m_renderer.init(&m_ui, &m_manager, &gData);
    // m_controller.init(gData);
    
    m_manager.load();
    SetExitKey(0);
    
    // float dt = 0.016f;
    // MenuScene menuScene(m_manager, m_renderer, m_input, m_controller, gData, m_ui, m_sceneManager);
    // m_sceneManager.changeScene(&menuScene, gData);
    // gData.scene = "menu";

    // WorldScene worldScene(m_manager, m_renderer, m_input, m_controller, gData, m_ui, m_sceneManager);

    m_sceneManager.QueTransit<TestScene>(m_manager);
    
    while(!WindowShouldClose())
    {
        // if (gData.changeScene)
        // {
        //     // if (gData.scene == "world") m_sceneManager.changeScene(&worldScene, gData);
        //     // if (gData.scene == "menu") m_sceneManager.changeScene(&menuScene, gData);
        // }
        {
            Tester test("Input", 500.0f, DEB_PR); 
            // m_input.getInput(m_renderer.getScreenRelativeRes());
        }
        {
            Tester test("Update", 500.0f, DEB_PR);
            // m_sceneManager.getScene()->update(dt);  
            m_sceneManager.Update();
        }
        {
            Tester test("Render", 17000.0f, DEB_PR);
            m_sceneManager.Draw();
        }            
        // dt = 1000.0f/GetFPS();
    }
}
void App::close()
{
}

App::~App()
{
   CloseWindow();
}
