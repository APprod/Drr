#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "utils/log.hpp"

class IScene;

enum class TransitType{
    Transit,
    TransitSus,
    Pop,
};

enum class TransitState{
    Idle,
    Exiting,
    Entering
};

class Pending{
public:
    TransitType type;
    std::unique_ptr<IScene> transitingScene;
    ~Pending();
};

class SceneManager{ //will be a part of Services so acessible everywhere
public:

    SceneManager();
    ~SceneManager();
    //Queues transition
    template<typename T, typename... Args> 
    void QueTransit(Args&&... args){
        if (m_state != TransitState::Idle || m_pending) {
            mylog::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        m_pending.emplace();
        m_pending->transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_pending->type = TransitType::Transit;
        m_state = TransitState::Exiting;
    }
    //Queues transition current scene will be
    //Suspended and restored on next Pop
    template<typename T, typename... Args> 
    void QueTransitSus(Args&&... args){
        if (m_state != TransitState::Idle || m_pending) {
            mylog::GetLogger().Warn("Transition already queued, ignoring");
            return;
        }
        if (m_scenes.empty()){
            mylog::GetLogger().Warn("Can't transitSus with empty stack");
            return;
        }
        m_pending.emplace();
        m_pending->transitingScene = std::make_unique<T>(std::forward<Args>(args)...);
        m_pending->type = TransitType::TransitSus;
        m_state = TransitState::Exiting;
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
    std::optional<Pending> m_pending;
    TransitState m_state{TransitState::Idle};
    std::vector<std::unique_ptr<IScene>> m_scenes;
};