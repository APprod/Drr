/* rendering functionality */
#pragma once
#include "core/recManager.hpp"
#include "ui/ui.hpp"
#include "scenes/layer.hpp"



class Renderer
{
public:

    void drawCoords(Button &button);
    void drawLayersButtons(Layers &layers);
    void drawButtons(const BNames &names);
    void drawButtonsBase(const BNames &names);
    void drawButtonsDarken(const BNames &names);
    void drawButtonsLighten(const BNames &names);

    void drawGrid(float spacing, float amount, Vector2 start);

    Renderer();
    void init(MyUI *ui, RecourceManager *recManager, GData *gData);
    
    Vector2 getSize(Resolution size); //FIX THIS
    Vector2 getScreenSize();
    Vector2 getScreenRelativeRes();

    void setScreenResolution(Resolution size);
    void setScreenSize();
    
    RenderTexture2D& getTarget() { return m_renderTarget;}
    RenderTexture2D& getWorldRTV() { return m_worldRTV;}
    Rectangle getWorldRTVBoundaries();

    void setWorldRTV();

    ~Renderer();
private:
    int m_screenWidth = 1920/2;
    int m_screenHeight = 1080/2;

    MyUI *m_ui = nullptr;
    RecourceManager *m_recManager = nullptr;
    GData *m_data = nullptr;


    RenderTexture2D m_renderTarget;
    RenderTexture2D m_worldRTV;
};
