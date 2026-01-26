#pragma once

#include "core/timer.hpp"
#include "scenes/scene.hpp"
#include "scenes/layer.hpp"
#include "objects/world.hpp"

class HudLayer : public ILayer
{
public:
    HudLayer() = default;
    ~HudLayer()= default;
    
private:   
};

class MapLayer : public ILayer
{
public:
    MapLayer() = default;
    ~MapLayer()= default;
};


class WorldScene : public IScene
{
public:
    WorldScene(
            RecourceManager &manager,
            Renderer &m_renderer,
            MyInput &input,
            LogicController &controller,
            GData &gData,
            MyUI &ui,
            SceneManager &sceneManager);

    ~WorldScene() override;
    virtual void load() override;
    void reload(Resolution size);
    virtual void unload() override;

    void debugMove(Button &button, MyInput input);
    void updateHud(float dt);

    void updatePos(float dt);
    void updateClick(float dt);
    void updateWorld(float dt);
    void update(float dt) override;

    void updateEditor();
    void renderToWorldRTV();
    void renderFromRTV();
    void renderWorld();

    void render() override;

private:
    bool m_gotButtonClick = false;

    World m_world;
    TimersSystem m_timers;

    HudLayer m_hudLayer;
    MapLayer m_mapLayer;
    
};
