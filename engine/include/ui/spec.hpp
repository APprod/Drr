#pragma once

#include <limits>

#include "raylib.h"

// paddings stack 
// padding in pixels 
struct Padding
{
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
};

// padding in percent
struct PaddingPct
{
    float top = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
};

enum class FillMode {
    FillMaxWidth,
    FillMaxHeight,
    FillMaxSize,
    ByTargetSize
};
// weight for how much the component can give up space or use space space
struct Flex{
    float growth{0};
    float shrink{0};
};

struct UIComponentSpec {
    FillMode fillMode = FillMode::ByTargetSize;
    Padding padding{};
    PaddingPct paddingPct{};
    Flex flex{0,0};
    Vector2 minSize{0,0};
    Vector2 maxSize{std::numeric_limits<float>::max(),std::numeric_limits<float>::max()};

    UIComponentSpec& SetFlex(Flex iflex){flex = iflex; return *this;}
    UIComponentSpec& MinSize(Vector2 size){minSize = size; return *this;}
    UIComponentSpec& MaxSize(Vector2 size){maxSize = size; return *this;}
    UIComponentSpec& SetFillMode(FillMode mode){fillMode = mode; return *this;}
    UIComponentSpec& FillMaxWidth(){fillMode = FillMode::FillMaxWidth; return *this;}
    UIComponentSpec& FillMaxHeight(){fillMode = FillMode::FillMaxHeight; return *this;}
    UIComponentSpec& FillMaxSize(){fillMode = FillMode::FillMaxSize; return *this;}
    UIComponentSpec& FillTargetSize(){fillMode = FillMode::ByTargetSize; return *this;}
    UIComponentSpec& SetPadding(Padding p){padding = p; return *this;}
    UIComponentSpec& SetPaddingPct(PaddingPct p){paddingPct = p; return *this;}

    Padding ResolvePadding(Vector2 dims) const;
};
using UICSpec = UIComponentSpec;
