#include "raylib.h"
#include "core/app.hpp"
#include "core/debug.hpp"
#include "core/util.hpp"
#include "core/services.hpp"
#include "app/scenes/testScene.hpp"

#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


void setupDebugConfig() {
#ifndef NDEBUG
    GetServices().runtimeCfg = kDebugConfig;
    dbg::GetLogger().SetMinSeverity(dbg::Severity::INFO);
#else
    GetServices().runtimeCfg = kReleaseConfig;
#endif
}

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}


void App::init(){
    setupDebugConfig();
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::ConsoleSink>(dbg::ConsoleSink()));
    #ifndef __EMSCRIPTEN__
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::FileSink>(dbg::FileSink("log.txt")));
    #endif
    dbg::GetLogger().Info("app:run \n");
    dbg::GetLogger().Info("Running direcotry: ", GetWorkingDirectory());
    #ifndef __EMSCRIPTEN__
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
    #endif
    {//temp, initislization
        InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
                SetWindowPosition(5,20);
                SetWindowState(::FLAG_WINDOW_RESIZABLE);

                auto m = GetCurrentMonitor();  
                auto fps = GetMonitorRefreshRate(m);
                if (!fps)  fps = 60;
                SetTargetFPS(fps);
                SetWindowState(::FLAG_WINDOW_MAXIMIZED); 
    }
    auto& recManager = GetServices().recManager;
    recManager.init();
    recManager.load();
    SetExitKey(0);
    
    // float dt = 0.016f;
    m_sceneManager.QueTransit<TestScene>(recManager);
}

void App::frame(){
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

void App::run()
{
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void* arg)
        {
            static_cast<App*>(arg)->frame();
        },
        this,
        0,
        true
    );
    #else
    while (!WindowShouldClose())
    {
        frame();
    }
    #endif
}
void App::close()
{
    
}

App::~App()
{
    CloseWindow();
}
