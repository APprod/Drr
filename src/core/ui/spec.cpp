#include "core/ui/spec.hpp"

Padding UIComponentSpec::ResolvePadding(Vector2 dims) const{
    return {
        static_cast<int>(padding.top + dims.y*paddingPct.top / 100.0f),
        static_cast<int>(padding.bottom + dims.y*paddingPct.bottom / 100.0f),
        static_cast<int>(padding.left + dims.x*paddingPct.left / 100.0f),
        static_cast<int>(padding.right + dims.x*paddingPct.right / 100.0f),
    };
}
