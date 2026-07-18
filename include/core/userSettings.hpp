#pragma once

#include "raylib.h"
#include "core/ui/uiTheme.hpp"

struct ProcessingValues {
    float brightness{1.0f};
    float contrast{1.0f};
    float saturation{1.0f};
    float gamma{1.0f};
    float alpha{1.0f};
    Vector3 tint{1.0f, 1.0f, 1.0f};
};

enum class WindowMode { Windowed, Fullscreen, Borderless };

class UserSettings {
public:
    int targetFPS{60};
    bool vsync{true};

    bool showFPS{true};
    float userBrightness{1};
    
    WindowMode windowMode{WindowMode::Windowed};
    ProcessingValues processing;
    UITheme theme;
};

