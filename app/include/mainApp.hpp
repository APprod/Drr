#pragma once
#include "app.hpp"
#include "userSettings.hpp"
#include "scenes/testScene.hpp"

class App: public IApp{
    virtual void initPreOpenGl() override {}; 
    virtual UserSettings getStartConfig() override {
        return UserSettings{
            .targetFPS = 60,
            .showFPS = true,    
            .userBrightness = 1,
            .windowMode = WindowMode::Windowed,
            .windowSize{1000,500},
            .windowPos{10,10},
            .theme{}
        };
    };
    virtual void initPostOpenGl() override {
        GetServices().sceneManager.QueTransit<TestScene>(); 
    };
    virtual void close() override {};
};

std::unique_ptr<IApp> createApp();