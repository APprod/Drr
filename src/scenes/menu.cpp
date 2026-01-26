
#include "debug.hpp"
#include "scenes/menu.hpp"
#include "scenes/sceneManager.hpp"
#include "core/logic.hpp"
#include "core/util.hpp"

MenuScene::MenuScene(RecourceManager &manager, Renderer &renderer, MyInput &input, LogicController &controller, GData &gData, MyUI &ui, SceneManager &sceneManager):
    IScene(manager, renderer, input, controller,  gData, ui, sceneManager)
{
}

MenuScene::~MenuScene()
{
    
}

void createButton(MyUI &ui, RecourceManager &manager, Renderer &renderer,  Button &button, ILayer *layer, std::string name);

void MenuScene::load()
{

    layers.push_back(&m_menuLayer);
    layers.push_back(&m_optionsLayer);
    m_menuLayer.isActive = true;

    m_manager.loadTexture("button_default");
    m_manager.loadTexture("button_wide");
    m_manager.loadTexture("menu");

    auto buttons = m_manager.loader.getButtons("assets/layoutMedium.json");
    for (auto &[key, val]: buttons)
    {
        if (key == "start") createButton(m_ui, m_manager, m_renderer, val, &m_menuLayer, "start");
        else if (key == "options") createButton(m_ui, m_manager, m_renderer, val, &m_menuLayer, "options");
        else if (key == "return") createButton(m_ui, m_manager, m_renderer, val, &m_optionsLayer, "return");
        else if (key == "test") createButton(m_ui, m_manager, m_renderer, val, &m_optionsLayer, "test");
    }
}

void MenuScene::reload(Resolution size)
{
}
void MenuScene::unload()
{
    layers.clear();
}

void MenuScene::updateMenu(float dt)
{
    if (!m_menuLayer.isActive) return;
    if (m_ui.getButton("options").release)
    {
        m_menuLayer.isActive = false;
        m_optionsLayer.isActive = true;
    }
    if (m_ui.getButton("start").release)
    {
        gData.changeScene = true;
        gData.scene = "world";
    }
}
void MenuScene::updateOptions(float dt)
{
    if (!m_optionsLayer.isActive) return;
    if (m_ui.getButton("return").release)
    {
        m_menuLayer.isActive = true;
        m_optionsLayer.isActive = false;
    }
    if (m_ui.getButton("test").release)
    {
        m_renderer.setScreenResolution(Resolution::S_MEDIUM);
    }
}

void MenuScene::debugMove(Button &button, MyInput input)
{
    if (IsKeyDown(KEY_R)) button.release = false;
    if (button.hold && IsKeyDown(KEY_R))
    {
        button.pos += input.getMoveDelta();
    }
}

void MenuScene::update(float dt)
{
    for (auto &layer : layers)
    {
        if (!layer->isActive) continue;
        m_controller.updateButtons(m_input, m_ui, layer);
        for (auto &name: layer->getButtonNames())
        {
            auto &button = m_ui.getButton(name);
            debugMove(button, m_input);
        }
    }
    updateMenu(dt);
    updateOptions(dt);
}

void MenuScene::render()
{
    BeginDrawing();
    BeginTextureMode(m_renderer.getTarget());
        ClearBackground(BLACK); 
        
        DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
        DrawFPS(20, 20);
        // for (auto &layer : layers)
        // {       
        //     if (!layer->isActive) continue;
        //     m_renderer.drawButtons(layer->getButtonNames());
        // }
        m_renderer.drawLayersButtons(layers);
    EndTextureMode();
    auto &target = m_renderer.getTarget().texture;
    DrawTexturePro(target,
                { 0, 0, static_cast<float>(target.width), static_cast<float>(-target.height) },
                { 0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())},
                    {0,0}, 0, RAYWHITE);
    EndDrawing();  

}


//------------------
//      Layers
//------------------
