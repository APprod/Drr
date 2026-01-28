#include <memory>
#include <vector>
#include <stdexcept>

#include "core/debug.hpp"
#include "core/scene.hpp"


SceneManager::SceneManager() {
}
void SceneManager::QuePop(){
    //Ques to pop Scenes Stack aka return to previous Scene
    if (status.action != SceneAction::Idle) {
        dbg::GetLogger().Warn("Transition already queued");
        return;
    }
    status.action = SceneAction::Pop;
    status.transitingScene.reset();
}
void SceneManager::ResolveTransitions(){
    //called once per frame
    switch (status.action)
    {
    case SceneAction::Idle:{
        return;
    }
    case SceneAction::TransitSus:{
        PerformSuspendAndTransit(std::move(status.transitingScene));
        break;
    }
    case SceneAction::Transit:{
        PerformTransit(std::move(status.transitingScene));
        break;
    }
    case SceneAction::Pop:{
        PopScene();
        break;
    }
    }
    status.action = SceneAction::Idle;
}
void SceneManager::PopScene(){
    if (scenes.size() == 1) {
        dbg::GetLogger()
            .Error("No scene left when trying to pop scene", typeid(*scenes.back()));
        return;
    }
    scenes.back()->OnExit();
    scenes.pop_back();
    scenes.back()->OnRestore();
}
void SceneManager::PerformTransit(std::unique_ptr<IScene> scene){
    if (scenes.size() != 0) {
        scenes.back()->OnExit();
        scenes.pop_back();
    }
    AddScene(std::move(scene));
}
void SceneManager::PerformSuspendAndTransit(std::unique_ptr<IScene> scene){
    if (scenes.size() == 0) {
        dbg::GetLogger()
            .Error("No scene left when trying to transit");
        return;
    }
    scenes.back()->OnSuspend();
    AddScene(std::move(scene));
}
void SceneManager::AddScene(std::unique_ptr<IScene> scene){
    scenes.push_back(std::move(scene));
    scenes.back()->OnEnter();
}
Status status;
std::vector<std::unique_ptr<IScene>> scenes;



