#include "debug.hpp"
#include "core/renderer.hpp"
#include "raylib.h"
#include "core/util.hpp"
#include "scenes/layer.hpp"


static bool RDBG = true;

static bool DEBUG_POS = false;

static Color BLUE1 = {32,64,80,255};
static Color BLUE2 = {56,80,80,255};

Renderer::Renderer()
{
    
}

void Renderer::init(MyUI *ui, RecourceManager *recManager, GData *gData)
{
    fwk::D_Pr("Renderer Initialized");
    m_ui = ui;
    m_recManager = recManager; 
    m_data = gData;

    ::SetConfigFlags(::FLAG_MSAA_4X_HINT | ::FLAG_VSYNC_HINT);
    
    InitWindow(m_screenWidth, m_screenHeight, "Raylib app");
    
    setScreenResolution(Resolution::S_MEDIUM);
    setScreenSize();
    
    SetWindowPosition(5,20);
    SetWindowState(::FLAG_WINDOW_RESIZABLE);
    
    
    // SetTargetFPS(4000);
    SetTargetFPS(60);
    auto m = GetCurrentMonitor();    // Get current monitor where window is placed
    SetTargetFPS(GetMonitorRefreshRate(m));
    SetWindowState(::FLAG_WINDOW_MAXIMIZED);
    
}

Renderer::~Renderer()
{
}

Rectangle Renderer::getWorldRTVBoundaries()
{
    return Rectangle{0.0f, static_cast<float>(m_screenHeight * 0.052), static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight * (0.79))};
}

void Renderer::setWorldRTV()
{
    float width = m_screenWidth;
    float height = m_screenHeight * 0.79f;
    if(IsRenderTextureValid(m_worldRTV)) ::UnloadRenderTexture(m_worldRTV);
    m_worldRTV = ::LoadRenderTexture(width, height);
}

void Renderer::drawCoords(Button &button)
{
    std::string pos;
    pos = "x: " + std::to_string(button.pos.x) + ", y: " + std::to_string(button.pos.y);
    DrawTextEx(m_recManager->getFont("TNR"), pos.c_str(), button.pos - 5, button.textSize, button.textSpacing, RAYWHITE);
}

Vector2 Renderer::getSize(Resolution size)
{
    return Vector2();
}

void Renderer::setScreenResolution(Resolution size)
{
    m_screenWidth = 0;
    m_screenHeight = 0;

    if (size == Resolution::S_MEDIUM) 
    {
        m_screenWidth = 1920;
        m_screenHeight = 1080;
    }
    else if (size == Resolution::S_SMALL)
    {
        m_screenWidth = 1920/2;
        m_screenHeight = 1080/2;
    }

    m_data->res = size;
    
    if (m_renderTarget.texture.width == m_screenWidth) return;
    UnloadRenderTexture(m_renderTarget);
    
    m_renderTarget = LoadRenderTexture(m_screenWidth, m_screenHeight);
}


void Renderer::setScreenSize()
{
    SetWindowSize(m_screenWidth, m_screenHeight);
    setWorldRTV();
}

Vector2 Renderer::getScreenSize()
{
    return {m_screenWidth, m_screenHeight};
}

Vector2 Renderer::getScreenRelativeRes()
{
    return {GetScreenWidth()/static_cast<float>(m_screenWidth), GetScreenHeight()/static_cast<float>(m_screenHeight)};
}

void Renderer::drawButtonsBase(const BNames &names)
{
    for (auto buttonName: names)
    {
        Button &button = m_ui->getButton(buttonName);
        Texture2D texture = m_recManager->getTexture(button.textureName);
        Vector2 origin{0.0f, 0.0f}; float rotation = 0.0f;
        ::DrawTexturePro(texture, source(texture), rect(button), origin, rotation, WHITE);

        if (DEBUG_POS) drawCoords(button);
        
        auto textX = button.pos.x + (button.size.x - button.textDimensions.x) / 2.0f;
        auto textY = button.pos.y + (button.size.y - button.textDimensions.y) / 2.0f;
        Vector2 textPos = {.x = textX, .y = textY};
        ::DrawTextEx(m_recManager->getFont("TNR"), button.message.c_str(), textPos, button.textSize, 2, button.textTint);
    }
}
void Renderer::drawButtonsLighten(const BNames &names)
{
    for (auto buttonName: names)
    {
        Button &button = m_ui->getButton(buttonName);
        Texture2D texture = m_recManager->getTexture(button.textureName);
        if (!button.hold && button.hover) ::DrawRectangleRec(rect(button), {255,255,255,30}); 
    }
}
void Renderer::drawButtonsDarken(const BNames &names)
{
    for (auto buttonName: names)
    {
        Button &button = m_ui->getButton(buttonName);
        Texture2D texture = m_recManager->getTexture(button.textureName);
        if (button.hold && button.hover) ::DrawRectangleRec(rect(button), {200,200,200,255}); 
    }
}



void Renderer::drawLayersButtons(Layers &layers)
{
    for (auto &layer : layers)
    {       
        if (!layer->isActive) continue;
        drawButtons(layer->getButtonNames());
    }
    ::DrawFPS(20, 20);   
}

void Renderer::drawButtons(const BNames &names)
{  
        drawButtonsBase(names);
    ::BeginBlendMode(BlendMode::BLEND_MULTIPLIED);
        drawButtonsDarken(names);
    ::EndBlendMode();
    ::BeginBlendMode(BlendMode::BLEND_ADDITIVE);
        drawButtonsLighten(names);
    ::EndBlendMode();
}
void Renderer::drawGrid(float spacing, float amount, Vector2 start)
{
    for (int i = 0; i <  amount; ++i)
    {
        for (int j = 0; j < amount; ++j)
        {
            DrawRectangleLinesEx({start.x + spacing * i, start.y + spacing * j, spacing + 0.1f, spacing + 0.1f}, 0.2, RAYWHITE);
        }
    }
}
