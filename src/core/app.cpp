#include "raylib.h"
#include "core/app.hpp"
#include "core/debug.hpp"
#include "core/util.hpp"
#include "app/scenes/testScene.hpp"

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
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::FileSink>(dbg::FileSink("log.txt")));

    dbg::GetLogger().DebugInfo("app:run \n");
    dbg::GetLogger().DebugInfo("Running direcotry: ", GetWorkingDirectory());
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
    {//temp, initislization
        InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
                SetWindowPosition(5,20);
                SetWindowState(::FLAG_WINDOW_RESIZABLE);

                SetTargetFPS(60);
                auto m = GetCurrentMonitor();  
                SetTargetFPS(GetMonitorRefreshRate(m));
                SetWindowState(::FLAG_WINDOW_MAXIMIZED); 
    }
    m_manager.init();
    m_manager.load();
    SetExitKey(0);
    
    // float dt = 0.016f;
    m_sceneManager.QueTransit<TestScene>(m_manager);
    
    while(!WindowShouldClose())
    {
        {
            Tester test("Input", 500.0f, DEB_PR); 
            // m_input.getInput(m_renderer.getScreenRelativeRes());
            m_input.getInput({123, 123});
        }
        {
            Tester test("Update", 500.0f, DEB_PR);
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
