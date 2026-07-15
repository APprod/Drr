#pragma once

#include "raylib.h"

struct ProcessingValues {
    float brightness{1.0f};
    float contrast{1.0f};
    float saturation{1.0f};
    float gamma{1.0f};
    float alpha{1.0f};
    Vector3 tint{1.0f, 1.0f, 1.0f};
};

class UserSettings {
public:
    ProcessingValues processing;

    //not yet
    int targetFPS{60};
    //not yet
    bool vsync{true};
    //not yet
    float userBrightness;
    //not yet
    enum class WindowMode { Windowed, Fullscreen, Borderless } windowMode{WindowMode::Windowed};
    //not yet
    std::string activeFont{"TNR"};
    //not yet
    int activeFontSize{20};
};

