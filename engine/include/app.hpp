/* all functionality in this class */
#pragma once
#include "userSettings.hpp"

class IApp{
public:
    IApp() = default; //Methods in their call order
    virtual void initPreOpenGl() = 0; 
    virtual UserSettings getStartConfig() = 0;
    virtual void initPostOpenGl() = 0; //Transition to the first scene
    virtual void close() = 0;
    virtual ~IApp() = default;    
};

//Implement to return your App class
std::unique_ptr<IApp> createApp();

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
