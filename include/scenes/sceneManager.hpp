#pragma once

class IScene;
class GData;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void changeScene(IScene *newScene, GData &data);
    IScene* getScene();
private:
    IScene *m_currentScene = nullptr;
};