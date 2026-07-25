#include <memory>

#include "core/scene/scene.hpp"
#include "core/scene/sceneManager.hpp"
#include "core/utils/log.hpp"

Status::~Status() {};
SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::QuePop(){
    //Ques to pop Scenes Stack aka return to previous Scene
    if (m_status.action != SceneAction::Idle) {
        mylog::GetLogger().Warn("Transition already queued");
        return;
    }
    m_status.action = SceneAction::Pop;
    m_status.transitingScene.reset();
    m_state = State::Exiting;
}
void SceneManager::ResolveTransitions(){
    //called once per frame
    switch (m_status.action)
    {
    case SceneAction::Idle:{
        return;
    }
    case SceneAction::TransitSus:{
        PerformSuspendAndTransit(std::move(m_status.transitingScene));
        break;
    }
    case SceneAction::Transit:{
        PerformTransit(std::move(m_status.transitingScene));
        break;
    }
    case SceneAction::Pop:{
        PopScene();
        break;
    }
    }
    m_status.action = SceneAction::Idle;
}
void SceneManager::PopScene(){
    if (m_scenes.size() == 1) {
        mylog::GetLogger()
            .Error("No scene left when trying to pop scene");
        return;
    }
    m_scenes.back()->OnExit();
    m_scenes.pop_back();
    m_scenes.back()->OnRestore();
    m_state = State::Entering;
}
void SceneManager::PerformTransit(std::unique_ptr<IScene> scene){
    if (m_scenes.size() != 0) {
        m_scenes.back()->OnExit();
        m_scenes.pop_back();
    }
    AddScene(std::move(scene));
}
void SceneManager::PerformSuspendAndTransit(std::unique_ptr<IScene> scene){
    if (m_scenes.size() == 0) {
        mylog::GetLogger()
            .Error("No scene left when trying to transit");
        return;
    }
    m_scenes.back()->OnSuspend();
    AddScene(std::move(scene));
}
void SceneManager::AddScene(std::unique_ptr<IScene> scene){
    m_scenes.push_back(std::move(scene));
    m_scenes.back()->OnEnter();
    m_state = State::Entering;
}
void SceneManager::Update(){
    float dt = GetFrameTime();
    
    if (m_scenes.empty()) {
        ResolveTransitions();
        if (m_scenes.empty()) {
            mylog::GetLogger().Error("Scene Stack is Empty");
            return;
        }
    }
    switch (m_state) {
    case State::Idle:
        m_scenes.back()->OnUpdate(dt);
        break;

    case State::Entering:
        if (m_scenes.back()->AnimateEnter(dt))
            m_state = State::Idle;
        m_scenes.back()->OnUpdate(dt);
        break;

    case State::Exiting: {
        bool finished = m_scenes.back()->AnimateExit(dt);
        if (!finished) {
            m_scenes.back()->OnUpdate(dt);
        } else {
            ResolveTransitions();
            if (m_state == State::Entering) {
                if (m_scenes.back()->AnimateEnter(dt))
                    m_state = State::Idle;
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