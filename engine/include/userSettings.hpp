#pragma once

#include <vector>

#include "raylib.h"
#include "utils/util.hpp"

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
    int targetFPS{0}; //0 for auto
    bool vsync{true};

    bool showFPS{true};
    float userBrightness{1};
    
    WindowMode windowMode{WindowMode::Windowed};
    Ivec2 windowSize{};
    Ivec2 windowPos{};
    std::vector<int> fontSizes{12,14,16,18,20,22,24,26,28,32,36,40,44,48,52,56,60,64,72,80,88,96};
};


inline void SwitchWindowMode(WindowMode mode){
    switch (mode){
    case WindowMode::Fullscreen:
        SetWindowState(FLAG_FULLSCREEN_MODE);
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
        break;
    case WindowMode::Borderless:
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        ClearWindowState(FLAG_FULLSCREEN_MODE);
        break;
    case WindowMode::Windowed:
        ClearWindowState(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_UNDECORATED);
        break;
    }
}