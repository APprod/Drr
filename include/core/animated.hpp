#pragma once

#include <functional>
#include <algorithm>
#include <cmath>

#include "raylib.h"

using EasingFn = float(*)(float t);

namespace Easing {
    inline float linear(float t) { return t; }

    inline float easeOutCubic(float t) {
        return 1.0f - std::pow(1.0f - t, 3.0f);
    }

    inline float easeInCubic(float t) {
        return t * t * t;
    }

    inline float easeInOutQuad(float t) {
        return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
    }

    inline float easeOutBack(float t) {
        constexpr float c = 1.70158f;
        return 1.0f + (c + 1.0f) * std::pow(t - 1.0f, 3.0f) + c * std::pow(t - 1.0f, 2.0f);
    }
}

template<typename T>
T lerp(const T& a, const T& b, float t);

template<> inline float   lerp(const float& a,   const float& b,   float t) { return a + (b - a) * t; }
template<> inline Vector2 lerp(const Vector2& a,  const Vector2& b, float t) { return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t}; }
template<> inline Vector4 lerp(const Vector4& a,  const Vector4& b, float t) {
    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
}

template<typename T>
struct Animated {
    T        current;
    T        target;
    float    duration;
    float    elapsed = 0.0f;
    EasingFn easing  = Easing::easeOutCubic;

    Animated(T initial, float dur = 0.15f, EasingFn ease = Easing::easeOutCubic)
        : current(initial), target(initial), duration(dur), easing(ease) {}

    void setTarget(const T& value) {
        if (value == target) return;
        elapsed = 0.0f;
        target  = value;
    }

    void setImmediate(const T& value) {
        current = target = value;
        elapsed = duration;
    }

    void update(float dt) {
        if (elapsed >= duration) return;
        elapsed = std::min(elapsed + dt, duration);
        float t = easing(elapsed / duration);
        current = lerp(current, target, t);
    }

    bool isDone() const { return elapsed >= duration; }

    operator const T&() const { return current; }
};
