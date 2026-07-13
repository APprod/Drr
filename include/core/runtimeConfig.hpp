#pragma once

struct RuntimeConfig {
    bool showFPS{true};
    bool showLayoutBounds{false};
    bool showLayoutContentBounds{false};
    bool showCursorPos{true};
    bool showPerformance{true};
    bool showDebugLog{true};
    bool showDebugOverlay{false};
    int debugMessagesCount{20};
};