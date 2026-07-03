#pragma once

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "raylib.h"
#include "core/debug.hpp"


struct Ivec2
{
    int32_t x;
    int32_t y;
};

struct MyRectangle {
    Rectangle rect;

    MyRectangle() = default;
    MyRectangle(const Vector2& a, const Vector2& b)
        : rect{a.x, a.y, b.x - a.x, b.y - a.y} {}

    // Implicit conversion to Raylib Rectangle
    operator Rectangle() const {
        return rect;
    }
};

std::ostream& operator<<(std::ostream& out, Vector2 vec);

std::ostream& operator<<(std::ostream& out, Ivec2 vec);
std::ostream& operator<<(std::ostream& out, Rectangle rec);

Vector2 operator+(Vector2 vec, int var);
Vector2 operator-(Vector2 vec, int var);

Vector2 operator-(Vector2 vec, Vector2 vec2);
Vector2 operator+(Vector2 vec, Vector2 vec2);
Vector2 norm(Vector2 vec);

Vector2& operator+=(Vector2 &vec, Vector2 vec2);
Vector2& operator+=(Vector2 &vec, float val);
Vector2 operator*(Vector2 vec, Vector2 vec2);
Vector2 operator/(Vector2 vec, Vector2 vec2);

bool operator==(Vector2 vec, Vector2 vec2);
bool operator!=(Vector2 vec, Vector2 vec2);

Vector2 operator*(Vector2 vec, float val);
Vector2 operator/(Vector2 vec, float val);

float distanceSquared(const Vector2 vec, const Vector2 vec2);

Rectangle rect(Vector2 vec, Vector2 vec2);
Rectangle rect(Texture2D texture);

struct Ivec2Hasher
{
    std::hash<int> ihash;

    size_t operator() (const Ivec2 &vector) const
    {
        const constexpr size_t coef = 2'946'901;
        return ihash(vector.x) + coef * ihash(vector.y);
    }
};

Ivec2 ivec(Vector2 vec);

bool operator==(const Ivec2 &vec1, const Ivec2 &vec2);

Rectangle source(Texture2D &tex);
std::string toString(Vector2 vec);
std::string toString(Rectangle vec);

template <class K, class V>
std::vector<K> getKeys(std::unordered_map<K,V> map)
{
    std::vector<K> keys;
    keys.reserve(map.size());
    for (auto it: map)
    {
        keys.push_back(it.first);
    }
    return keys;
}


template<class T>
void myClamp(T &val, T min, T max)
{
    if (val < min) val = min;
    if (val > max) val = max;
}


class Tester
{
    std::chrono::steady_clock::time_point start;
    std::string name;
    float edge;
    bool active;
public:

    Tester(std::string name, float edge, bool active);
    ~Tester();
};

constexpr uint32_t BIT(int v){return 1 << v;}
