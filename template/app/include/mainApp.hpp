#pragma once
#include "app.hpp"
#include "userSettings.hpp"
#include "scene/baseScene.hpp"
#include "ui/label.hpp"
#include "ui/layout.hpp"
#include "ui/overlay.hpp"

class HelloScene : public BaseScene {
public:
    HelloScene() { m_transitionTime = 0.2f; }
    void OnEnter() override {
        auto center = std::make_unique<VerticalLayout>(
            UICSpec().FillMaxSize(),
            LayoutSpec{}.AlignCenter().CrossCenter());
        center->AddChild(std::make_unique<Label>(Text("Hello, ApEngine!", "default")));
        root.AddChild(std::move(center));
        root.AddChild(std::make_unique<Overlay>());
        OnResize();
    }
    void OnDrawContent() override {
        root.OnDraw();
        DrawFadeTransition();
    }
    void OnUpdateState() override {}
};

class App : public IApp {
    virtual void initPreOpenGl() override {}
    virtual UserSettings getStartConfig() override {
        return UserSettings{
            .targetFPS = 60,
            .showFPS = true,
            .userBrightness = 1,
            .windowMode = WindowMode::Windowed,
            .windowSize{800, 450},
            .windowPos{10, 10},
        };
    };
    virtual void initPostOpenGl() override {
    };
    virtual std::unique_ptr<IScene> createScene(){
        return std::make_unique<HelloScene>();
    }
    virtual void close() override {};
};

std::unique_ptr<IApp> createApp();
