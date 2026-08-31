#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <numbers>

#include "raylib.h"

using EasingFn = float(*)(float t);
namespace Easing {

inline float linear(float t) { return t; }

// Sine
inline float easeInSine(float t)    { return 1.0f - std::cos(t * std::numbers::pi_v<float> / 2.0f); }
inline float easeOutSine(float t)   { return std::sin(t * std::numbers::pi_v<float> / 2.0f); }
inline float easeInOutSine(float t) { return -(std::cos(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f; }

// Cubic
inline float easeInCubic(float t)    { return t * t * t; }
inline float easeOutCubic(float t)   { return 1.0f - std::pow(1.0f - t, 3.0f); }
inline float easeInOutCubic(float t) { return t < 0.5f ? 4.0f*t*t*t : 1.0f - std::pow(-2.0f*t + 2.0f, 3.0f) / 2.0f; }

// Quad
inline float easeInQuad(float t)    { return t * t; }
inline float easeOutQuad(float t)   { return 1.0f - (1.0f - t) * (1.0f - t); }
inline float easeInOutQuad(float t) { return t < 0.5f ? 2.0f*t*t : 1.0f - std::pow(-2.0f*t + 2.0f, 2.0f) / 2.0f; }

// Expo
inline float easeInExpo(float t)    { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f*t - 10.0f); }
inline float easeOutExpo(float t)   { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f*t); }
inline float easeInOutExpo(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    return t < 0.5f ? std::pow(2.0f, 20.0f*t - 10.0f) / 2.0f
                    : (2.0f - std::pow(2.0f, -20.0f*t + 10.0f)) / 2.0f;
}

// Circ
inline float easeInCirc(float t)    { return 1.0f - std::sqrt(1.0f - t*t); }
inline float easeOutCirc(float t)   { return std::sqrt(1.0f - (t - 1.0f)*(t - 1.0f)); }
inline float easeInOutCirc(float t) {
    return t < 0.5f ? (1.0f - std::sqrt(1.0f - 4.0f*t*t)) / 2.0f
                    : (std::sqrt(1.0f - (-2.0f*t + 2.0f)*(-2.0f*t + 2.0f)) + 1.0f) / 2.0f;
}

// Back
inline float easeInBack(float t) {
    constexpr float c = 1.70158f;
    return (c + 1.0f)*t*t*t - c*t*t;
}
inline float easeOutBack(float t) {
    constexpr float c = 1.70158f;
    return 1.0f + (c + 1.0f)*std::pow(t - 1.0f, 3.0f) + c*std::pow(t - 1.0f, 2.0f);
}
inline float easeInOutBack(float t) {
    constexpr float c = 2.5949f;
    return t < 0.5f
        ? std::pow(2.0f*t, 2.0f) * ((c + 1.0f)*2.0f*t - c) / 2.0f
        : (std::pow(2.0f*t - 2.0f, 2.0f) * ((c + 1.0f)*(2.0f*t - 2.0f) + c) + 2.0f) / 2.0f;
}

// Elastic
inline float easeInElastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    return -std::pow(2.0f, 10.0f*t - 10.0f) * std::sin((t*10.0f - 10.75f) * 2.0f*std::numbers::pi_v<float> / 3.0f);
}
inline float easeOutElastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    return std::pow(2.0f, -10.0f*t) * std::sin((t*10.0f - 0.75f) * 2.0f*std::numbers::pi_v<float> / 3.0f) + 1.0f;
}
inline float easeInOutElastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    return t < 0.5f
        ? -(std::pow(2.0f, 20.0f*t - 10.0f) * std::sin((20.0f*t - 11.125f) * 2.0f*std::numbers::pi_v<float> / 4.5f)) / 2.0f
        :  (std::pow(2.0f,-20.0f*t + 10.0f) * std::sin((20.0f*t - 11.125f) * 2.0f*std::numbers::pi_v<float> / 4.5f)) / 2.0f + 1.0f;
}

// Bounce
inline float easeOutBounce(float t) {
    constexpr float n = 7.5625f, d = 2.75f;
    if (t < 1.0f/d)       return n*t*t;
    if (t < 2.0f/d)       { t -= 1.5f/d;   return n*t*t + 0.75f; }
    if (t < 2.5f/d)       { t -= 2.25f/d;  return n*t*t + 0.9375f; }
    /*  t < 1  */           t -= 2.625f/d;  return n*t*t + 0.984375f;
}
inline float easeInBounce(float t)    { return 1.0f - easeOutBounce(1.0f - t); }
inline float easeInOutBounce(float t) {
    return t < 0.5f ? (1.0f - easeOutBounce(1.0f - 2.0f*t)) / 2.0f
                    : (1.0f + easeOutBounce(2.0f*t - 1.0f)) / 2.0f;
}

using Fn = float(*)(float);

} // namespace Easing

template<typename T>
T lerp(const T& a, const T& b, float t);

template<> inline float   lerp(const float& a,   const float& b,   float t) { return a + (b - a) * t; }
template<> inline Vector2 lerp(const Vector2& a,  const Vector2& b, float t) { return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t}; }
template<> inline Vector4 lerp(const Vector4& a,  const Vector4& b, float t) {
    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
}

// Animates value between start and target between duration
template<typename T>
struct Animated {
    T        current;
    T        start;
    T        target;
    float    duration;
    float    elapsed = 0.0f;
    EasingFn easing  = Easing::easeOutCubic;

    Animated(T initial, float dur = 0.15f, EasingFn ease = Easing::easeOutCubic)
        : current(initial), start(initial), target(initial), duration(dur), easing(ease) {}

        
    bool isDone() const { return elapsed >= duration; }
    operator const T&() const { return current; }

    void setTarget(const T& value) {
        if (value == target) return;
        start   = current;
        elapsed = 0.0f;
        target  = value;
    }

    void setImmediate(const T& value) {
        current = start = target = value;
        elapsed = duration;
    }
    // reset state completely
    void reset(const T& s, float dur, EasingFn ease) {
        current = start = target = s;
        duration = dur;
        easing = ease;
        elapsed = 0.0f;
    }

    //called from mainloop
    void update(float dt) {
        if (elapsed >= duration) return;
        elapsed = std::min(elapsed + dt, duration);
        float t = easing(elapsed / duration);
        current = lerp(start, target, t);
    }
};