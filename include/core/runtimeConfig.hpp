#pragma once

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
    float brightness{1.0f};
};