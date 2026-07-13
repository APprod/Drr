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

struct RuntimeConfig {
    bool showFPS{true};
    bool showLayoutBounds{false};
    bool showLayoutContentBounds{false};
    bool showCursorPos{true};
    bool showPerformance{true};
    bool showDebugLog{true};
    bool showDebugOverlay{false};
    bool debugFeaturesAllowed{true};
    int debugMessagesCount{20};
    bool useProcessingShader{true};
    bool showOverlayGradient{true};
    ProcessingValues processing;
};

inline const RuntimeConfig kDebugConfig{
    .showFPS = true,
    .showLayoutBounds = false,
    .showLayoutContentBounds = false,
    .showCursorPos = true,
    .showPerformance = true,
    .showDebugLog = true,
    .showDebugOverlay = true,
    .debugFeaturesAllowed = true,
    .debugMessagesCount = 20,
    .useProcessingShader = true,
    .showOverlayGradient = false,
    .processing = {},
};

inline const RuntimeConfig kReleaseConfig{
    .showFPS = false,
    .showLayoutBounds = false,
    .showLayoutContentBounds = false,
    .showCursorPos = false,
    .showPerformance = false,
    .showDebugLog = false,
    .showDebugOverlay = false,
    .debugFeaturesAllowed = false,
    .debugMessagesCount = 20,
    .useProcessingShader = true,
    .showOverlayGradient = false,
    .processing = {},
};

void setupDebugConfig();
