#include <memory>

#include "core/scene.hpp"
#include "core/sceneManager.hpp"
#include "core/debug.hpp"

Status::~Status() = default;
SceneManager::~SceneManager() = default;

SceneManager::SceneManager() {
}
void SceneManager::QuePop(){
    //Ques to pop Scenes Stack aka return to previous Scene
    if (m_status.action != SceneAction::Idle) {
        dbg::GetLogger().Warn("Transition already queued");
        return;
    }
    m_status.action = SceneAction::Pop;
    m_status.transitingScene.reset();
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
        dbg::GetLogger()
            .Error("No scene left when trying to pop scene");
        return;
    }
    m_scenes.back()->OnExit();
    m_scenes.pop_back();
    m_scenes.back()->OnRestore();
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
        dbg::GetLogger()
            .Error("No scene left when trying to transit");
        return;
    }
    m_scenes.back()->OnSuspend();
    AddScene(std::move(scene));
}
void SceneManager::AddScene(std::unique_ptr<IScene> scene){
    m_scenes.push_back(std::move(scene));
    m_scenes.back()->OnEnter();
}

void SceneManager::Update(){
    ResolveTransitions();
    if (m_scenes.empty()){
        dbg::GetLogger()
            .Error("Scene Stack is Empty");
        return;
    }
    m_scenes.back()->OnUpdate();
}
void SceneManager::Draw(){
    if (m_scenes.empty()){
        dbg::GetLogger()
            .Error("Scene Stack is Empty");
        return;
    }
    m_scenes.back()->OnDraw();
}