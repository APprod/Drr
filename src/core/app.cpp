#include "raylib.h"
#include "core/app.hpp"
#include "core/utils/log.hpp"
#include "core/utils/util.hpp"
#include "core/services.hpp"


#include <memory>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#ifdef TRACY_PROFILE
#include <tracy/Tracy.hpp>
#endif

void setupDebugConfig() {
#ifndef NDEBUG
    GetServices().runtimeCfg.debug = defaultDebugFlags;
    #else
    GetServices().runtimeCfg.debug = defaultReleaseFlags;
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

Engine::Engine(std::unique_ptr<IApp> app): m_app{std::move(app)}
{ 
}

void Engine::init(){
    //Logging
    mylog::GetLogger();// Will be initialized before services, destroyed after
    mylog::GetLogger().AddSink(
        std::make_unique<mylog::ConsoleSink>(mylog::ConsoleSink())
    );
    //App + config
    m_app->initPreOpenGl();
    auto startConf = m_app->getStartConfig();
    auto& usr = GetServices().runtimeCfg.user = startConf;
    setupDebugConfig();
    initPlatform();
    mylog::GetLogger().Info("App init \n");
    mylog::GetLogger().Info("Running directory: ", GetWorkingDirectory());
   
    //OPengl+window
    InitWindow(usr.windowSize.x, usr.windowSize.y, "Raylib app");
    SetWindowPosition(usr.windowPos.x, usr.windowPos.y);
    m_app->initPostOpenGl();
    if (usr.vsync) SetWindowState(::FLAG_VSYNC_HINT);
    SetWindowState(::FLAG_WINDOW_RESIZABLE);
    if (usr.targetFPS){
        SetTargetFPS(usr.targetFPS);
    }else{
        auto m = GetCurrentMonitor();  
        auto fps = GetMonitorRefreshRate(m);
        if (!fps)  fps = 60;
        SetTargetFPS(fps);
    }
    SwitchWindowMode(usr.windowMode);

    //All Services that need initialising
    auto& recManager = GetServices().recManager;
    recManager.init();
    recManager.load();
    SetExitKey(0); //TODO: remove at some point
}

void Engine::frame(){
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

static void runPlatform(Engine* app)
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        [](void* arg)
        {
            static_cast<Engine*>(arg)->frame();
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

void Engine::run()
{
    runPlatform(this);
}
void Engine::close()
{
    m_app->close();
}

Engine::~Engine()
{
    CloseWindow();
}
