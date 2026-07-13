#include "raylib.h"
#include "core/app.hpp"
#include "core/debug.hpp"
#include "core/util.hpp"
#include "app/scenes/testScene.hpp"

#include <memory>

static void setupDebugConfig() {
    auto& cfg = GetServices().runtimeCfg;
#ifndef NDEBUG  // Debug build
    cfg.debugFeaturesAllowed = true;
    cfg.showFPS = true;
    cfg.showLayoutBounds = true;
    cfg.showLayoutContentBounds = true;
    cfg.showCursorPos = true;
    cfg.showPerformance = true;
    cfg.showDebugLog = true;
    cfg.showDebugOverlay = true;
#else           // Release build
    cfg.debugFeaturesAllowed = false;
    cfg.showFPS = false;
    cfg.showLayoutBounds = false;
    cfg.showLayoutContentBounds = false;
    cfg.showCursorPos = false;
    cfg.showPerformance = false;
    cfg.showDebugLog = false;
    cfg.showDebugOverlay = false;
    dbg::GetLogger().SetMinSeverity(dbg::Severity::INFO); // фильтрует DebugInfo
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

    dbg::GetLogger().DebugInfo("app:run \n");
    dbg::GetLogger().DebugInfo("Running direcotry: ", GetWorkingDirectory());
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
