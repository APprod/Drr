#pragma once

#include <memory>
#include <vector>
#include "core/log.hpp"

class IScene;

enum class SceneAction{
    Idle,
    Transit,
    TransitSus,
    Pop,
};

enum class State{
    Idle,
    Exiting,
    Entering
};

class Status{
public:
    SceneAction action = SceneAction::Idle;
    std::unique_ptr<IScene> transitingScene;
    ~Status();
};

class SceneManager{ //will be a part of Services so acessible everywhere
public:

    SceneManager();
    ~SceneManager();
    //Queues transition
    template<typename T, typename... Args> 
    void QueTransit(Args&&... args){
        if (m_status.action != SceneAction::Idle) {
            mylog::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        m_status.transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_status.action = SceneAction::Transit;
        m_state = State::Exiting;
    }
    //Queues transition current scene will be
    //Suspended and restored on next Pop
    template<typename T, typename... Args> 
    void QueTransitSus(Args&&... args){
        if (m_status.action != SceneAction::Idle) {
            mylog::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        m_status.transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_status.action = SceneAction::TransitSus;
        m_state = State::Exiting;
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
    State m_state{State::Idle};
    std::vector<std::unique_ptr<IScene>> m_scenes;
};
