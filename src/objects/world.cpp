#include <algorithm>
#include <array>
#include <cmath>
#include "core/util.hpp"
#include "objects/world.hpp"



World::World(TimersSystem &timers): m_timers(timers)
{
    camera.zoom = 1;
    camera.target = {0,0};
}

World::~World()
{
}

void World::init(Renderer &renderer, RecourceManager &manager)
{
    m_renderer = &renderer;
    m_manager = &manager;
    player = Player::createPlayer({450, 450}, 0, 100, 50, "default");
    loadTiles();
    
}

void  World::loadTiles()
{
    for (int32_t x = 0; x < Tile::tileXCount; ++x)
    {
        for (int32_t y = 0; y < Tile::tileYCount; ++y)
        {
            tiles.push_back(Tile{.pos = {x, y}});
        }
    }   
}

void World::clampCameraPos() //TODO: start timer and move in like 1 second towards edgePos for more smooth movement
{
    Rectangle bounds = getInWorldViewBounds();
    Ivec2 pos = getPlayerTile();

    myClamp(pos.x, 1, static_cast<int32_t>(Tile::tileXCount - 1));
    myClamp(pos.y, 1, static_cast<int32_t>(Tile::tileYCount - 1));

    Ivec2 edgePos = {(pos.x - 1) * Tile::tileSize,
                    (pos.y - 1) * Tile::tileSize};       
    Ivec2 rEdgePos = {(pos.x + loadSize + 1) * Tile::tileSize,
            (pos.y + loadSize + 1) * Tile::tileSize};
                
    myClamp(camera.target.x, edgePos.x + bounds.width / 2,  rEdgePos.x - bounds.width / 2);
    myClamp(camera.target.y, edgePos.y + bounds.height / 2,  rEdgePos.y - bounds.height / 2);
}
void World::clampCameraZoom()
{
    auto bounds = getInWorldViewBounds();
    float limitSize = static_cast<float>(Tile::tileSize * (loadSize * 2 +1));
    if (bounds.width > limitSize)
    {
        float ratio = bounds.width / limitSize;
        int difference = 0;
        while (ratio > 1)
        {
            ratio = ratio / m_zoomCoeff;
            difference += 1;
        }
        m_zoomMultiplier += difference;
        camera.zoom = std::pow(m_zoomCoeff, m_zoomMultiplier);
    }
}

void World::updateCamera(Vector2 RTVSize, float deltaZoom)
{
    m_zoomMultiplier += deltaZoom;
    camera.zoom = std::pow(m_zoomCoeff, m_zoomMultiplier);
    clampCameraZoom();

    camera.offset = RTVSize / Vector2{2.0, 2.0};
    clampCameraPos();
}

void World::moveCamera(Vector2 deltaMove)
{

    camera.target +=  deltaMove / ( -camera.zoom);
    clampCameraPos();
    
}

void World::centerPlayer()
{
    camera.target = player.worldPos;
}

// void World::updateTiles()
// {
//     Ivec2 playerTile = ivec(player.worldPos / Tile::tileSize);
// }

Ivec2 World::getPlayerTile()
{
    return ivec(player.worldPos / Tile::tileSize);
}

Tilemap World::getActiveTiles()
{
    Ivec2 playerTile = ivec(player.worldPos / Tile::tileSize);
    fwk::D_Pr("Tile: ", playerTile, true);
    Tilemap activeTiles;

    for (auto &tile : tiles)
    {
        if (playerTile.x - loadSize <= tile.pos.x &&
            playerTile.x + loadSize >= tile.pos.x &&
            playerTile.y - loadSize <= tile.pos.y &&
            playerTile.y + loadSize >= tile.pos.y) 
        {
            // activeTiles[tile.pos] = tile;
            activeTiles.insert({tile.pos, tile});
        }  //active Tiles  
    }
    return activeTiles;
}

static float randomAlpha = 150.0f;
Color randomColor()
{
    return {rand() / 255.0f, rand() / 255.0f, rand() / 255.0f, randomAlpha};
}

static std::array<Color, 12> colors;
static bool filled = false;


void World::drawTiles()
{
    Tilemap tilemap = getActiveTiles();

    //test
    if (!filled) {filled = true; std::generate(colors.begin(), colors.end(), randomColor);}
   
    int i = 0;
    for (auto &[key, tile]: tilemap)
    {
        // Recta
        auto pos = Vector2{tile.pos.x * Tile::tileSize, tile.pos.y * Tile::tileSize};
        
        float size = static_cast<float>(Tile::tileSize);
        DrawRectangleV(pos, {size, size}, colors.at(i++));
    }
}

void World::render()
{
    Color color{255,255,255,50};
    ::BeginMode2D(camera);
        ::ClearBackground(color);
        drawTiles();
        m_renderer->drawGrid(10, 10, {600.0f, 600.0f});
        player.render(*m_manager, getInWorldViewBounds(), camera.zoom);
    ::EndMode2D();
    
}

Rectangle World::getInWorldViewBounds()
{
    auto centerCords = camera.target;
    auto [x, y, width, height] = m_renderer->getWorldRTVBoundaries();
    auto LUp = centerCords  - Vector2{width / 2.0f, height / 2.0f} / camera.zoom;
    auto Size = Vector2{width , height } / camera.zoom;
    return rect(LUp, Size);
}

Vector2 World::posToWorld(Vector2 pos)
{
    auto bounds = getInWorldViewBounds();
    auto rtvBounds = m_renderer->getWorldRTVBoundaries();
    auto res = Vector2{(pos.x - rtvBounds.x), (pos.y - rtvBounds.y)} / camera.zoom;
    return Vector2{bounds.x, bounds.y} + res;
}

