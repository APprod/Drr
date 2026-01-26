#pragma once

#include "core/recManager.hpp"
#include "core/myinput.hpp"
#include "core/renderer.hpp"
#include "core/structs.hpp"
#include "scenes/layer.hpp"
#include "ui/ui.hpp"
#include <vector>

class LogicController;
class SceneManager;

using Layers=std::vector<ILayer *>;

class IScene {
public:
    IScene( RecourceManager &manager,
            Renderer &m_renderer,
            MyInput &input,
            LogicController &controller,
            GData &gData,
            MyUI &ui,
            SceneManager &sceneManager);

    virtual ~IScene() = default;
    // virtual void () = 0;   // Initialize everything needed for scene // if shares data needs to check or lazy load
    // virtual void OnExit() = 0;    // DeInitialize everything needed for scene // or don't unload on exit +
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;
    
    
    Layers layers;
protected:

    RecourceManager &m_manager;
    LogicController &m_controller;
    Renderer &m_renderer;
    MyInput &m_input;
    GData &gData;
    MyUI &m_ui;
    SceneManager &m_sceneManager;
    
};

