
#include "scenes/mainScene.hpp"
#include "core/logic.hpp"
#include "core/util.hpp"
#include <cmath>

static bool WDBG = true;

void createButton(MyUI &ui, RecourceManager &manager, Renderer &renderer,  Button &button, ILayer *layer, std::string name)
{
    Font font = manager.getFont(button.fontName);
    button.calculateLength(font, button.textSize, button.textSpacing);
    ui.addButton(name, button, layer);
}

WorldScene::WorldScene(RecourceManager &manager, Renderer &renderer, MyInput &input, LogicController &controller, GData &gData, MyUI &ui, SceneManager &sceneManager):
    IScene(manager, renderer, input, controller,  gData, ui, sceneManager), m_world(m_timers)
{
}

WorldScene::~WorldScene()
{
    
}


void WorldScene::load()
{


    layers.push_back(&m_hudLayer);
    layers.push_back(&m_mapLayer);
    m_hudLayer.isActive = true;
    m_mapLayer.isActive = true;

    m_manager.loadTexture("button_default");
    m_manager.loadTexture("button_wide");
    m_renderer.setWorldRTV();
    // m_manager.loadTexture("menu");

    m_world.init(m_renderer, m_manager);

    auto buttons = m_manager.loader.getButtons("assets/layoutMediumWorld.json");
    for (auto &[key, val]: buttons)
    {
        // if (key == "start") createButton(m_ui, m_manager, m_renderer, val, &m_, "start");
    }

    m_world.m_zoomMultiplier = 30;
    m_world.centerPlayer();

}

void WorldScene::reload(Resolution size)
{
}
void WorldScene::unload()
{
    layers.clear();
}

void WorldScene::updateHud(float dt)
{
    if (!m_hudLayer.isActive) return;
    
}

void WorldScene::updateClick(float dt)
{
    m_world.click = false;
    m_world.release = false;
    m_world.releaseClose = false;
    if (!m_gotButtonClick && m_input.getClick())    
    {
        if (CheckCollisionPointRec(m_input.getActivePos(), m_renderer.getWorldRTVBoundaries()))
        {
            m_world.click = true;
            m_world.hold = true;
            m_world.clickPosScreen = m_input.getActivePos();
        }
    } else if (m_world.hold && m_input.getRelease())
    {
        m_world.release = true;
        m_world.hold = false;
        auto moveDistanceSquared = distanceSquared(m_world.clickPosScreen, m_input.getActivePos());
        auto limit = m_renderer.getScreenSize().x * 0.025;
        if (moveDistanceSquared < limit * limit)
        {
            m_world.releaseClose = true;
        }
    }

    fwk::D_Pr("WDBG Map Click", m_world.click && WDBG);
    fwk::D_Pr("WDBG Map Hold", m_world.hold && WDBG);
    fwk::D_Pr("WDBG Map release", m_world.release && WDBG);
    fwk::D_Pr("WDBG Map releaseClose", m_world.releaseClose && WDBG);
}


void WorldScene::updatePos(float dt)
{
    if (m_world.hold)
    {
        auto &cam = m_world.camera;
        m_world.moveCamera(m_input.getMoveDelta());
    }
}

void WorldScene::updateWorld(float dt)
{
    if (!m_mapLayer.isActive) return;

    updateClick(dt);
    auto bounds = m_renderer.getWorldRTVBoundaries();
    m_world.updateCamera({bounds.width, bounds.height}, m_input.getZoomAction());
    updatePos(dt);

    //test
    if (m_world.releaseClose)
    {
        // m_world.player.target = GetScreenToWorld2D(m_input.getActivePos(), m_world.camera);
        m_world.player.target = m_world.posToWorld(m_input.getActivePos());
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        m_world.player.moving = !m_world.player.moving;
    }

    if (IsKeyPressed(KEY_A))
    {
        m_world.centerPlayer();
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        gData.scene = "menu";
        gData.changeScene = true;
    }

    m_world.player.move(dt); // basic moving
    
}
void WorldScene::debugMove(Button &button, MyInput input)
{
    if (IsKeyDown(KEY_R)) button.release = false;
    if (button.hold && IsKeyDown(KEY_R))
    {
        button.pos += input.getMoveDelta();
    }
}

void WorldScene::update(float dt)
{
    m_gotButtonClick = false;
    for (auto &layer : layers)
    {
        if (!layer->isActive) continue;
        m_gotButtonClick = m_controller.updateButtons(m_input, m_ui, layer);
        for (auto &name: layer->getButtonNames())
        {
            auto &button = m_ui.getButton(name);
            debugMove(button, m_input);
        }
    }

    updateHud(dt);
    updateWorld(dt);
  
}

// void WorldScene::drawPlayer()
// {
//     auto &player = m_world.player;
//     auto playerTextrue = m_manager.getTexture(player.texture);
//     auto texturePos = player.worldPos;
//     texturePos.x -= player.playerIconSize.x/2.0f;
//     texturePos.y -= player.playerIconSize.y/2.0f;
    
//     DrawTexturePro(playerTextrue, source(playerTextrue), rect(texturePos, player.playerIconSize), {0,0}, 0, RAYWHITE);

//     DrawCircleV(player.target, 2,RED);
// }

void WorldScene::renderToWorldRTV()
{
    ::BeginTextureMode(m_renderer.getWorldRTV());
        m_world.render();
    ::EndTextureMode();
}

void WorldScene::renderFromRTV()
{
    auto &target = m_renderer.getTarget();
    auto &texure = m_renderer.getWorldRTV().texture;
    
    ::BeginTextureMode(target);
    
        auto bounds = m_renderer.getWorldRTVBoundaries();
        ::DrawTexturePro(texure,
                    { 0, 0, static_cast<float>(texure.width), static_cast<float>(-texure.height) },
                    bounds,
                        {0,0}, 0, RAYWHITE);
    ::EndTextureMode();
}

void WorldScene::renderWorld()

{
    if (!m_mapLayer.isActive) return;
    
    renderToWorldRTV();
    renderFromRTV();
}


void WorldScene::render() 
{
    ::BeginDrawing();
        ::BeginTextureMode(m_renderer.getTarget());
            ::ClearBackground(BLACK);
        ::EndTextureMode();

        renderWorld();

        ::BeginTextureMode(m_renderer.getTarget());
            m_renderer.drawLayersButtons(layers);
        ::EndTextureMode();
        
        auto &target = m_renderer.getTarget().texture;
        ::DrawTexturePro(target,
                    { 0, 0, static_cast<float>(target.width), static_cast<float>(-target.height) },
                    { 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())},
                        {0,0}, 0, RAYWHITE);
    ::EndDrawing();  
}


