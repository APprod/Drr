#include "core/util.hpp"
#include <cmath>


std::ostream &operator<<(std::ostream &out, Vector2 vec)
{
    out << "x: " << vec.x << " | y: " << vec.y;
    return out;
}
std::ostream& operator<<(std::ostream& out, Rectangle rec)
{
    out << "x: " << rec.x << " | y: " << rec.y;
    out << "width: " << rec.width << " | height: " << rec.height;
    return out;
}

std::ostream& operator<<(std::ostream& out, Ivec2 vec)
{
    out << "x: " << vec.x << " | y: " << vec.y;
    return out;
}


Vector2 operator+(Vector2 vec, int var)
{
    return {vec.x + var, vec.y + var};
}

Vector2 operator-(Vector2 vec, int var)
{
     return {vec.x - var, vec.y - var};
}

Vector2 operator-(Vector2 vec, Vector2 vec2)
{
    return {vec.x - vec2.x, vec.y - vec2.y};
}
Vector2 operator+(Vector2 vec, Vector2 vec2)
{
    return {vec.x + vec2.x, vec.y + vec2.y};
}

Vector2 norm(Vector2 vec)
{
    auto dist = std::sqrt(vec.x*vec.x + vec.y*vec.y);
    return vec/dist;
}

Vector2 &operator+=(Vector2 &vec, Vector2 vec2)
{
    vec.x += vec2.x;
    vec.y += vec2.y;
    return vec;
}

Vector2& operator+=(Vector2 &vec, float val)
{
    vec.x += val;
    vec.y += val;
    return vec;
}

Vector2 operator*(Vector2 vec, Vector2 vec2)
{
    return {vec.x*vec2.x, vec.y*vec2.y};
}

Vector2 operator*(Vector2 vec, float val)
{
    return {vec.x * val, vec.y * val};
}
Vector2 operator/(Vector2 vec, float val)
{
    return {vec.x / val, vec.y / val};
}

Vector2 operator/(Vector2 vec, Vector2 vec2)
{
    return {vec.x/vec2.x, vec.y/vec2.y};
}

float distanceSquared(const Vector2 vec, const Vector2 vec2)
{
    auto dx = (vec.x - vec2.x);
    auto dy = (vec.y - vec2.y);
    return dx * dx + dy * dy ;
}

Rectangle rect(Vector2 vec, Vector2 vec2)
{
    return {vec.x, vec.y, vec2.x, vec2.y};
}

// Rectangle rect(Button &button)
// {
//     return Rectangle{button.pos.x, button.pos.y, button.size.x, button.size.y};
// }

Ivec2 ivec(Vector2 vec)
{
    return {static_cast<int32_t>(vec.x), static_cast<int32_t>(vec.y)};
}

bool operator==(const Ivec2 &vec1, const Ivec2 &vec2)
{
    return vec1.x == vec2.x && vec1.y == vec2.y;
}

Rectangle source(Texture2D &tex)
{
    return {0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height)};
}
std::string toString(Vector2 vec)
{
    return std::to_string(vec.x) + " "+std::to_string(vec.y);
}
std::string toString(Rectangle vec)
{
    return std::to_string(vec.x) + " "+std::to_string(vec.y) + "\n"\
            + std::to_string(vec.width) + " "+std::to_string(vec.height);
}
Tester::Tester(std::string name, float edge, bool active): 
        start(std::chrono::steady_clock::now()), 
        name(name), 
        edge(edge),
        active(active)
{
}

Tester::~Tester()
{
    if (!active) return;
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    if (dur.count() > edge * 2) dbg::GetLogger().Info("!!!!!! Tester " + name + " , ms:", dur.count() / 1000.0f, true);
    else if (dur.count() > edge) dbg::GetLogger().Info("Tester: " + name + " , ms:", dur.count() / 1000.0f, true);
}
