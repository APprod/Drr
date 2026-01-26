/*just definitions of all structs used in different parts*/
#pragma once
#include "raylib.h"
#include <string>
#include <cstdint>

enum class Resolution
{
    S_SMALL, // 860:540
    S_MEDIUM, // 1920:1080
    S_HIGH, // 2k?
    S_MEDIUM_21, //21:16?
};


class Button
{
public:
    Vector2 pos;
    Vector2 size;
    std::string textureName;
    std::string message;

    std::string fontName;
    Vector2 textDimensions;
    Color textTint;

    float textSize = 20;
    float textSpacing = 1;


    bool hold = false;
    bool hover = false;
    bool click = false;
    bool release = false;
    void calculateLength(Font &font, float textsize, float spacing) { textDimensions = MeasureTextEx(font, message.c_str(), textsize, spacing);}//textLength = MeasureTextEx();}
};

//TODO:
class SwichButton : Button
{

};

struct GData
{
    // std::string page;
    float dt = 16.7f;
    Resolution res;
    std::string scene = "";
    bool changeScene = false;
};

struct Ivec2
{
    int32_t x;
    int32_t y;
};