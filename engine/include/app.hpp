/* Main class that contains all functionality*/
#pragma once
#include "userSettings.hpp"
#include "scene/scene.hpp"

class IApp{
public:
    IApp() = default;
    //Methods will be called in the following order

    virtual void initPreOpenGl() = 0;
    // This config will be set at startup  
    virtual UserSettings getStartConfig() = 0;
    virtual void initPostOpenGl() = 0;
    //Return first scene
    virtual std::unique_ptr<IScene> createScene() = 0;
    virtual void close() = 0;
    virtual ~IApp() = default;    
};

// Standalone Forward declared function without implementation
// Implement to return your derived App class
std::unique_ptr<IApp> createApp();

//Main class handles setup and mainloop
class Engine
{
public:
    Engine(std::unique_ptr<IApp> app);
    void init();
    void run();
    void frame();
    void close();
    ~Engine();
private:
    std::unique_ptr<IApp> m_app;
};
