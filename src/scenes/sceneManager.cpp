
#include "debug.hpp"
#include "scenes/sceneManager.hpp"
#include "scenes/scene.hpp"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::changeScene(IScene *newScene, GData &data)
{
    if (!newScene)
    {
        fwk::D_Pr("no scene provided");
        return;
    }
    if (newScene == m_currentScene)
    {
        fwk::D_Pr("scene Is Current");
        return;
    }
    if (m_currentScene) 
    {
        fwk::D_Pr("Unloading Scene");
        m_currentScene->unload();
    }
    fwk::D_Pr("Loading Scene");
    newScene->load();
    m_currentScene = newScene;
    data.changeScene = false; 
}

IScene *SceneManager::getScene()
{
    return m_currentScene;
}
