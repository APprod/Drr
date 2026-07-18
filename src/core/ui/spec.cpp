#include "core/ui/spec.hpp"

#include <cmath>

Padding UIComponentSpec::ResolvePadding(Vector2 dims) const{
    return {
        static_cast<int>(std::round(padding.top + dims.y*paddingPct.top / 100.0f)),
        static_cast<int>(std::round(padding.bottom + dims.y*paddingPct.bottom / 100.0f)),
        static_cast<int>(std::round(padding.left + dims.x*paddingPct.left / 100.0f)),
        static_cast<int>(std::round(padding.right + dims.x*paddingPct.right / 100.0f)),
    };
}
