#pragma once
#include "app.hpp"
#include "userSettings.hpp"
#include "scenes/testScene.hpp"

class App: public IApp{
    virtual void initPreOpenGl() override {}; 
    virtual UserSettings getStartConfig() override {
        UserSettings cfg{
            .targetFPS = 60,
            .showFPS = true,
            .userBrightness = 1,
            .windowMode = WindowMode::Windowed,
            .windowSize{1000,500},
            .windowPos{10,10},
            .theme{}
        };
        auto& th = cfg.theme;
        th.setSizes({12,14,16,18,20,22,24,26,28,32,36,40,44,48,52,56,60,64,72,80,88,96});
        th.registerSizeRole("caption", -1);
        th.registerSizeRole("button", 2);
        th.registerSizeRole("header", 2);
        th.registerSizeRole("big header", 4);
        th.registerFontRole("default", "Inter", 1);
        return cfg;
    };
    virtual void initPostOpenGl() override {
        GetServices().sceneManager.QueTransit<TestScene>(); 
    };
    virtual void close() override {};
};

std::unique_ptr<IApp> createApp();