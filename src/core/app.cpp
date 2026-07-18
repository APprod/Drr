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
#ifdef TRACY_PROFILE
#include <tracy/Tracy.hpp>
#endif

void setupDebugConfig() {
#ifndef NDEBUG
    GetServices().runtimeCfg.debug = kDebugFlags;
    #else
    GetServices().runtimeCfg.debug = kReleaseFlags;
    dbg::GetLogger().SetMinSeverity(dbg::Severity::INFO);
#endif
}

static void initPlatform()
{
#ifndef __EMSCRIPTEN__
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::FileSink>(dbg::FileSink("log.txt")));
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
#endif
}

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}

void App::init(){
    dbg::GetLogger();// Will be initialized before services, destroyed after
    setupDebugConfig();
    dbg::GetLogger().AddSink(
        std::make_unique<dbg::ConsoleSink>(dbg::ConsoleSink()));
    initPlatform();
    dbg::GetLogger().Info("app:run \n");
    dbg::GetLogger().Info("Running directory: ", GetWorkingDirectory());
    {
        InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
                SetWindowPosition(5,20);
                SetWindowState(::FLAG_WINDOW_RESIZABLE);
                SetWindowState(::FLAG_VSYNC_HINT);

                auto m = GetCurrentMonitor();  
                auto fps = GetMonitorRefreshRate(m);
                if (!fps)  fps = 60;
                SetTargetFPS(fps);
                // SetWindowState(::FLAG_WINDOW_MAXIMIZED); 
    }
    auto& recManager = GetServices().recManager;
    recManager.init();
    recManager.load();
    SetExitKey(0);
    auto& manager = GetServices().sceneManager;
    manager.QueTransit<TestScene>(recManager);
}

void App::frame(){
    #ifdef TRACY_PROFILE
        FrameMark;
        ZoneScopedN("Frame"); 
    #endif
    auto& manager = GetServices().sceneManager;
    {
        PerfTester tester = GetServices().perfLog.log("Scene Update");
        manager.Update();
    }
    {
        PerfTester tester = GetServices().perfLog.log("Scene Render");
        manager.Draw();
    }            
    GetServices().perfLog.update();
    // dt = 1000.0f/GetFPS();
}

static void runPlatform(App* app)
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void* arg)
        {
            static_cast<App*>(arg)->frame();
        },
        app,
        0,
        true
    );
#else
    while (!WindowShouldClose())
    {
        app->frame();
    }
#endif
} 

void App::run()
{
    runPlatform(this);
}
void App::close()
{
    
}

App::~App()
{
    CloseWindow();
}
