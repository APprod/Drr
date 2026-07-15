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

    int targetFPS{60};
    bool vsync{true};
    bool showFPS{true};

    //not yet
    float userBrightness;
    //not yet
    enum class WindowMode { Windowed, Fullscreen, Borderless } windowMode{WindowMode::Windowed};
    
    //not yet
    std::string activeFont{"TNR"};
    //not yet
    int activeFontSize{20};
};

