#include "raylib.h"
#include "core/app.hpp"
#include "core/log.hpp"
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
    mylog::GetLogger().SetMinSeverity(mylog::Severity::INFO);
#endif
}

static void initPlatform()
{
#ifndef __EMSCRIPTEN__
    mylog::GetLogger().AddSink(
        std::make_unique<mylog::FileSink>(mylog::FileSink("mylog.txt")));
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
#endif
}

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}

void App::init(){
    mylog::GetLogger();// Will be initialized before services, destroyed after
    setupDebugConfig();
    mylog::GetLogger().AddSink(
        std::make_unique<mylog::ConsoleSink>(mylog::ConsoleSink()));
    initPlatform();
    mylog::GetLogger().Info("app:run \n");
    mylog::GetLogger().Info("Running directory: ", GetWorkingDirectory());
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
