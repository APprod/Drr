#pragma once

struct RuntimeConfig {
    bool showFPS{true};
    bool showLayoutBounds{true};
    bool showLayoutContentBounds{true};
    bool showCursorPos{true};
    bool showPerformance{true};
    bool showDebugLog{true};
    bool showDebugOverlay{true};
    int debugMessagesCount{20};
};