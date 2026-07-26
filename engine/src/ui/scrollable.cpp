#include "ui/scrollable.hpp"
#include <algorithm>
#include <cmath>

bool Scrollable::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<ScrollEvent>(&event)){
        if (!hovered){
            return false;
        }
        float delta = direction == ScrollDirection::Vertical ? e->delta.y : e->delta.x;
        float speed = scrollSpeed * (1.0f + std::sqrt(maxOffset) * 0.05f);
        scrollOffset -= speed * delta;
        atBottom = (scrollOffset >= maxOffset);
        return true;
    }
    return false;
}

void Scrollable::OnUpdate(Rectangle drawRect, Vector2 contentSize){
    auto contentDim = direction == ScrollDirection::Vertical ? &Vector2::y : &Vector2::x;
    auto rectDim = direction == ScrollDirection::Vertical ? &Rectangle::height : &Rectangle::width;
    maxOffset = contentSize.*contentDim - drawRect.*rectDim;
    maxOffset = std::max(0.0f, maxOffset);
    if (maxOffset > 0.0f && atBottom){
        scrollOffset = maxOffset;
    }
    myClamp(scrollOffset, 0.0f, maxOffset);
}

void Scrollable::OnHover(){
    hovered = true;
}

void Scrollable::OnHoverExit(){
    hovered = false;
}

void Scrollable::DrawInside(Rectangle actual, std::function<void()> drawCall){
    auto ir = irect(actual);
    BeginScissorMode(ir.x, ir.y, ir.width, ir.height);
    drawCall();
    if (maxOffset > 0.0f && drawBorders)
        ::DrawRectangleLinesEx(actual, 1, WHITE);
    EndScissorMode();
}
