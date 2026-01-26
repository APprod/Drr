#pragma once
#include "raylib.h"
#include <string>
#include "core/structs.hpp"
#include "core/timer.hpp"
#include "core/util.hpp"
#include <vector>
#include "objects/player.hpp"
#include "objects/tiles.hpp"
#include "core/renderer.hpp"
#include <unordered_map>






// class Tile
// {
//     Ivec2 tileCords;
// };

using Tilemap=std::unordered_map<Ivec2, Tile&, Ivec2Hasher>;

class World
{

public:
    World(TimersSystem &timers);
    ~World();

    void init(Renderer &renderer, RecourceManager &manager);
    void loadTiles();

    void clampCameraZoom();
    void clampCameraPos();
    
    void updateCamera(Vector2 RTVSize, float deltaZoom);
    void moveCamera(Vector2 deltaMove);
    void centerPlayer();
    
    // void updateTiles();

    Rectangle getInWorldViewBounds();
    Vector2 posToWorld(Vector2 pos);

    Ivec2 getPlayerTile();
    Tilemap getActiveTiles();

    void drawTiles();
    void render();
    
    void addBiom(Tile &tile);


    //directly needed
    Camera2D camera;
    float m_zoomCoeff = 1.1f;
    float m_zoomMultiplier = 1;
    Player player;
    std::vector<Tile> tiles;
    

    //click registring
    bool click = false;
    bool release = false;  
    bool releaseClose = false;    
    bool hold = false;    
    Vector2 clickPosScreen;
    const int loadSize = 1;
private:
    
//Util
    TimersSystem &m_timers;
    Renderer *m_renderer;
    RecourceManager *m_manager;
};


    /*
    Player
    needs:
    stats:
    bool moving
    max currentSpeed
    curr currentSpeed
    currentAccel
    position
    targetPos
    stats.health
    stats.water
    stats.hunger etc.
    

    other:
    texture
    ref: gData

    probably should load itself from some data in gData or directly from loader

    World scene should update it.
    When moving should be checked if can move, otherwise reset to zero

    */