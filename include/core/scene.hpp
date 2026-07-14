#pragma once

#include <memory>
#include <vector>

#include "core/services.hpp"
#include "core/debug.hpp"
#include "core/ui/root.hpp"
#include "core/util.hpp"

class IScene{ //Base class for Scene
public:
    virtual ~IScene() = default; 

    virtual void OnEnter() = 0; //Allocate everything
    virtual void OnRestore() = 0; //Called only if it was Suspended

    virtual void OnUpdate() = 0;
    virtual void OnDraw() = 0; 

    virtual void OnExit() = 0; //DeAllocate, called on normal transit
    virtual void OnSuspend() = 0;  // called on suspended transit
private:
};

enum class SceneAction{
    Idle,
    Transit,
    TransitSus,
    Pop
};

class Status{
public:
    SceneAction action = SceneAction::Idle;
    std::unique_ptr<IScene> transitingScene;
};

class SceneManager{ //will be a part of Services so acessible everywhere
public:

    SceneManager();
    //Queues transition
    template<typename T, typename... Args> 
    void QueTransit(Args&&... args){
        if (m_status.action != SceneAction::Idle) {
            dbg::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        m_status.transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_status.action = SceneAction::Transit;
    }
    //Queues transition current scene will be
    //Suspended and restored on next Pop
    template<typename T, typename... Args> 
    void QueTransitSus(Args&&... args){
        
        if (m_status.action != SceneAction::Idle) {
            dbg::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        m_status.transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_status.action = SceneAction::TransitSus;
    }
    void QuePop();
    void ResolveTransitions();

    void Update();
    void Draw();

private:
    void PopScene();
    void PerformTransit(std::unique_ptr<IScene> scene);
    void PerformSuspendAndTransit(std::unique_ptr<IScene> scene);
    void AddScene(std::unique_ptr<IScene> scene);
    Status m_status;
    std::vector<std::unique_ptr<IScene>> m_scenes;
};

class BaseScene: public IScene{
public:
    void OnRestore() override {};
    void OnSuspend() override {};
    void OnExit() override {};

    void OnDraw() override {
        BeginDrawing();
        ClearBackground(BLACK);
        OnDrawContent();
        EndDrawing();
    };
    void OnUpdate() override {
        PerfTester tester = GetServices().perfLog.log("Scene OnUpdate Full");
        auto& input = GetServices().input;
        {
            PerfTester innerTester = GetServices().perfLog.log("Scene input.pollEvents");
            input.pollEvents();
        }
        auto events = input.getEvents();
        for (auto& event: events){
            root.OnEvent(event); //updates states
        }
        bool needsResize = root.OnUpdate(); //updates other data related to UI states
        if (needsResize || ::IsWindowResized()) { //resize if needed
            OnResize();
        }
        OnUpdateState(); //updates scene data not UI
    };
protected:
    virtual void OnResize(){
        auto dims = Vector2{
                static_cast<float>(::GetScreenWidth()),
                static_cast<float>(::GetScreenHeight())
            };
            MyRectangle rect = {{0,0}, dims};
            root.OnMeasure(dims);
            root.OnArrange(rect);
    }
    virtual void OnDrawContent() = 0;
    virtual void OnUpdateState() = 0;
    Root root;
};
