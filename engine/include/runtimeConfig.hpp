#pragma once

#include "userSettings.hpp"

struct DebugFlags {
    bool showLayoutBounds{false};
    bool showLayoutContentBounds{false};
    bool showCursorPos{true};
    bool showPerformance{true};
    bool showDebugLog{true};
    bool showDebugOverlay{false};
    bool debugFeaturesAllowed{true};
    int debugMessagesCount{20};
    bool showOverlayGradient{true};
};

inline const DebugFlags defaultDebugFlags{
    .showLayoutBounds = false,
    .showLayoutContentBounds = false,
    .showCursorPos = true,
    .showPerformance = true,
    .showDebugLog = true,
    .showDebugOverlay = true,
    .debugFeaturesAllowed = true,
    .debugMessagesCount = 20,
    .showOverlayGradient = true,
};

inline const DebugFlags defaultReleaseFlags{
    .showLayoutBounds = false,
    .showLayoutContentBounds = false,
    .showCursorPos = false,
    .showPerformance = false,
    .showDebugLog = false,
    .showDebugOverlay = false,
    .debugFeaturesAllowed = true,
    .debugMessagesCount = 20,
    .showOverlayGradient = false,
};

struct RuntimeConfig {
    DebugFlags debug;
    UserSettings user;
};

void setupDebugConfig();
