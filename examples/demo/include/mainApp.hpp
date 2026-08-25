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
        };
    };
    virtual void initPostOpenGl() override {
        auto& th = GetServices().theme; // registration/binding is name-only here; fonts bake lazily after rm.load()
        th.registerSizeRole("caption", -1);
        th.registerSizeRole("button", 2);
        th.registerSizeRole("header", 2);
        th.registerSizeRole("big header", 4);
        th.registerFontRole("default", "Inter", 1);
        GetServices().sceneManager.QueTransit<TestScene>();
    };
    virtual void close() override {};
};

std::unique_ptr<IApp> createApp();