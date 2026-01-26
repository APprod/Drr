#include "core/app.hpp"
#include "debug.hpp"
#include "raylib.h"
#include "core/util.hpp"
#include "scenes/menu.hpp"
#include "scenes/scene.hpp"
#include "scenes/mainScene.hpp"

static bool DEB_PR = true;

App::App(int screenWidth, int screenHeight): m_screenHeight(screenHeight), m_screenWidth(screenWidth)
{ 
}

void App::run()
{
    fwk::D_Pr("app:run \n", DEB_PR);
    fwk::D_Pr("Running direcotry: ", GetWorkingDirectory());
    auto path = GetApplicationDirectory();
    ChangeDirectory(path);
    m_manager.init();
    m_renderer.init(&m_ui, &m_manager, &gData);
    m_controller.init(gData);
    m_manager.load();
    SetExitKey(0);
    
    float dt = 0.016f;
    MenuScene menuScene(m_manager, m_renderer, m_input, m_controller, gData, m_ui, m_sceneManager);
    m_sceneManager.changeScene(&menuScene, gData);
    gData.scene = "menu";

    WorldScene worldScene(m_manager, m_renderer, m_input, m_controller, gData, m_ui, m_sceneManager);

    
    while(!WindowShouldClose())
    {
        if (gData.changeScene)
        {
            if (gData.scene == "world") m_sceneManager.changeScene(&worldScene, gData);
            if (gData.scene == "menu") m_sceneManager.changeScene(&menuScene, gData);
        }
        {
            Tester test("Input", 500.0f, DEB_PR); 
            m_input.getInput(m_renderer.getScreenRelativeRes());
        }
        {
            Tester test("Update", 500.0f, DEB_PR);
            m_sceneManager.getScene()->update(dt);  
        }
        {
            Tester test("Render", 17000.0f, DEB_PR);
            
                m_sceneManager.getScene()->render();  
                        
        }            
        dt = 1000.0f/GetFPS();
    }
    
    
}
void App::close()
{
}

App::~App()
{
   CloseWindow();
}
