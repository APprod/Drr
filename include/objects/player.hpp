#pragma once
#include "raylib.h"
#include <string>
#include "core/recManager.hpp"

class Player
{
public:
static Player createPlayer(
            Vector2 iworldPos,
        float ispeed,
        float imaxSpeed,
        float iaccel,
        std::string itexture)
    {
        Player newPlayer;
        newPlayer.texture = itexture;
        newPlayer.currentAccel = iaccel;
        newPlayer.worldPos = iworldPos;
        newPlayer.currentSpeed = ispeed;
        newPlayer.currentMaxSpeed = imaxSpeed;
        return newPlayer;
    }

    void move(float dt);
    void render(RecourceManager &manager, Rectangle bounds, float zoom);
    
    Player();
    ~Player();

    Vector2 worldPos{0,0};

    float currentSpeed = 0;
    float currentMaxSpeed = 1;
    float baseSpeed = 5; // for future
    float currentAccel = 1;

    Vector2 target{0,0};
    bool moving = false;

    std::string texture = "default";
    Vector2 playerIconSize = {20, 20};
private:

};
