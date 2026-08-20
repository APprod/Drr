#include <memory>

#include "scene/scene.hpp"
#include "scene/sceneManager.hpp"
#include "utils/log.hpp"

Pending::~Pending() {};
SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::QuePop(){
    //Ques to pop Scenes Stack aka return to previous Scene
    if (m_state != TransitState::Idle || m_pending) {
        mylog::GetLogger().Warn("Transition already queued");
        return;
    }
    if (m_scenes.size()<=1){
        mylog::GetLogger().Warn("Can't pop when <=1 scene left");
        return;
    }
    m_pending.emplace();
    m_pending->type = TransitType::Pop;
    m_pending->transitingScene.reset();
    m_state = TransitState::Exiting; 
}
void SceneManager::ResolveTransitions(){
    if (!m_pending) return;
    switch (m_pending->type)
    {
    case TransitType::TransitSus:{
        PerformSuspendAndTransit(std::move(m_pending->transitingScene));
        break;
    }
    case TransitType::Transit:{
        PerformTransit(std::move(m_pending->transitingScene));
        break;
    }
    case TransitType::Pop:{
        PopScene();
        break;
    }
    }
    m_pending.reset();
}
void SceneManager::PopScene(){
    m_scenes.back()->OnExit();
    m_scenes.pop_back();
    m_scenes.back()->OnRestore();
    m_state = TransitState::Entering;
}
void SceneManager::PerformTransit(std::unique_ptr<IScene> scene){
    if (m_scenes.size() != 0) {
        m_scenes.back()->OnExit();
        m_scenes.pop_back();
    }
    AddScene(std::move(scene));
}
void SceneManager::PerformSuspendAndTransit(std::unique_ptr<IScene> scene){
    m_scenes.back()->OnSuspend();
    AddScene(std::move(scene));
}
void SceneManager::AddScene(std::unique_ptr<IScene> scene){
    m_scenes.push_back(std::move(scene));
    m_scenes.back()->OnEnter();
    m_state = TransitState::Entering;
}
void SceneManager::Update(){
    float dt = GetFrameTime();
    
    if (m_scenes.empty()) {
        if (m_pending) ResolveTransitions();
        if (m_scenes.empty()) {
            mylog::GetLogger().Error("Scene Stack is Empty");
            return;
        }
    }
    switch (m_state) {
    case TransitState::Idle:
        m_scenes.back()->OnUpdate(dt);
        break;
    case TransitState::Entering:
        if (m_scenes.back()->AnimateEnter(dt))
            m_state = TransitState::Idle;
        m_scenes.back()->OnUpdate(dt);
        break;
    case TransitState::Exiting: {
        bool finished = m_scenes.back()->AnimateExit(dt);
        if (!finished) {
            m_scenes.back()->OnUpdate(dt);
        } else {
            ResolveTransitions();
            if (m_state == TransitState::Entering) {
                if (m_scenes.back()->AnimateEnter(dt))
                    m_state = TransitState::Idle;
                m_scenes.back()->OnUpdate(dt);
            }
        }
        break;
    }
    }
}
void SceneManager::Draw(){
    if (m_scenes.empty()){
        mylog::GetLogger()
            .Error("Scene Stack is Empty");
        return;
    }
    m_scenes.back()->OnDraw();
}