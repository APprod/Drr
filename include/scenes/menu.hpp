#pragma once

#include "scenes/scene.hpp"
#include "scenes/layer.hpp"



class MenuLayer : public ILayer
{
public:
    MenuLayer() = default;
    ~MenuLayer()= default;
    
private:   
};

class OptionsLayer : public ILayer
{
public:
    OptionsLayer() = default;
    ~OptionsLayer()= default;
};


class MenuScene : public IScene
{
public:
    MenuScene(
            RecourceManager &manager,
            Renderer &m_renderer,
            MyInput &input,
            LogicController &controller,
            GData &gData,
            MyUI &ui,
            SceneManager &sceneManager);

    ~MenuScene() override;
    virtual void load() override;
    void reload(Resolution size);
    virtual void unload() override;

    void debugMove(Button &button, MyInput input);

    void updateMenu(float dt);
    void updateOptions(float dt);
    void update(float dt) override;

    void render() override;
    MenuLayer m_menuLayer;
    OptionsLayer m_optionsLayer;
private:
    
};
