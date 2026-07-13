#include "raylib.h"
#include "core/app.hpp"
#include "core/debug.hpp"
#include "core/util.hpp"
#include "core/structs.hpp"
#include "app/scenes/testScene.hpp"

#include <memory>

void setupDebugConfig() {
#ifndef NDEBUG
    GetServices().runtimeCfg = kDebugConfig;
#else
    GetServices().runtimeCfg = kReleaseConfig;
    dbg::GetLogger().SetMinSeverity(dbg::Severity::INFO);
#endif
}

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}


void App::run()
{
    setupDebugConfig();
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::ConsoleSink>(dbg::ConsoleSink()));
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::FileSink>(dbg::FileSink("log.txt")));

    dbg::GetLogger().Info("app:run \n");
    dbg::GetLogger().Info("Running direcotry: ", GetWorkingDirectory());
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
    {//temp, initislization
        InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
                SetWindowPosition(5,20);
                SetWindowState(::FLAG_WINDOW_RESIZABLE);

                auto m = GetCurrentMonitor();  
                SetTargetFPS(GetMonitorRefreshRate(m));
                SetWindowState(::FLAG_WINDOW_MAXIMIZED); 
    }
    auto& recManager = GetServices().recManager;
    recManager.init();
    recManager.load();
    SetExitKey(0);
    
    // float dt = 0.016f;
    m_sceneManager.QueTransit<TestScene>(recManager);
    
    while(!WindowShouldClose())
    {
        {
            PerfTester tester = GetServices().perfLog.log("Scene Update");
            m_sceneManager.Update();
        }
        {
            PerfTester tester = GetServices().perfLog.log("Scene Render");
            m_sceneManager.Draw();
        }            
        GetServices().perfLog.update();
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
