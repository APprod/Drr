#include "objects/player.hpp"
#include "core/util.hpp"


void Player::move(float dt) 
{
    if (!moving) {currentSpeed = 0; return;}

    auto direction = norm(target - worldPos);

    currentSpeed += currentAccel * dt / 1000;
    if (currentSpeed > currentMaxSpeed) currentSpeed = currentMaxSpeed; //TODO: clamp
    
    auto distance = currentSpeed * dt / 1000;
    
    if (distance*distance > distanceSquared(worldPos, target)) 
    {
        worldPos = target;
        moving = false;
        currentSpeed = 0;
        return;
    }
    worldPos += direction * distance;
}

void Player::render(RecourceManager &manager, Rectangle bounds, float zoom)
{
    auto playerTextrue = manager.getTexture(texture);
    auto texturePos = worldPos;
    auto playerIconSize  = this->playerIconSize / zoom;
    texturePos.x -= playerIconSize.x/2.0f;
    texturePos.y -= playerIconSize.y/2.0f;
    
    if (::CheckCollisionRecs(bounds, rect(worldPos - playerIconSize/2, playerIconSize)))
        ::DrawTexturePro(playerTextrue, source(playerTextrue), rect(texturePos, playerIconSize), {0,0}, 0, RAYWHITE);

    auto circleSize = 5 / zoom;
    if (::CheckCollisionRecs(bounds, rect(target - circleSize / 2, {circleSize, circleSize} )))
        ::DrawCircleV(target, circleSize, RED);

    ::DrawTextEx(manager.getFont("TNR"), ("cords: "+toString(this->worldPos)).c_str(), {bounds.x, bounds.y}, 2 / zoom * 10, 0.2 / zoom * 5, WHITE);

    // ::DrawTextEx(manager.getFont("TNR"), toString(this->worldPos).c_str(), this->worldPos, 2, 0.2, WHITE);
    // ::DrawTextEx(manager.getFont("TNR"), toString(bounds).c_str(), this->worldPos + 10, 2, 0.1, WHITE);
    // if (CheckCollisionRecs(bounds, rect(target - 1, {4,4})))  DrawCircleV(target, 2,RED);
   
}

Player::Player()
{

}

Player::~Player()
{
}
