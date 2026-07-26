#include "ui/scrollable.hpp"
#include <algorithm>

bool Scrollable::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<ScrollEvent>(&event)){
        if (!hovered){
            return false;
        }
        scrollOffset -= scrollSpeed * e->delta.y;
        atBottom = (scrollOffset >= maxOffset);
        return true;
    }
    return false;
}

void Scrollable::OnUpdate(Rectangle drawRect, Vector2 contentSize){
    maxOffset = contentSize.y - drawRect.height;
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
