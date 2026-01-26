#include "scenes/scene.hpp"
#include "scenes/layer.hpp"
#include <stdexcept>

class SceneManager;

IScene::IScene(RecourceManager &manager, Renderer &renderer, MyInput &input, LogicController &controller, GData &gData, MyUI &ui, SceneManager &sceneManager):
    m_manager(manager),m_renderer(renderer),  
    m_input(input), m_controller(controller), gData(gData), m_ui(ui), m_sceneManager(sceneManager)
{
}

//------------------
//     Scenes
//------------------
