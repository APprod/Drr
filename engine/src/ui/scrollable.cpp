#include "ui/scrollable.hpp"
#include <algorithm>
#include <cmath>

bool Scrollable::OnEvent(const MyEvent& event){
    if (auto* e = std::get_if<ScrollEvent>(&event)){
        if (!hovered) return false;
        float delta = direction == ScrollDirection::Vertical ? e->delta.y : e->delta.x;
        float speed = scrollSpeed * (1.0f + std::sqrt(maxOffset) * 0.05f);
        scrollOffset -= speed * delta;
        atBottom = (scrollOffset >= maxOffset);
        return true;
    }

    if (maxOffset <= 0) return false;

    auto majorPos = [&](Vector2 pos){
        return direction == ScrollDirection::Vertical ? pos.y : pos.x;
    };
    auto majorDim = [&](Rectangle r){
        return direction == ScrollDirection::Vertical ? r.height : r.width;
    };

    if (auto* e = std::get_if<CursorActionEvent>(&event)){
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT){
            if (e->pressed){
                Rectangle track = m_lastViewport;
                float tSize = majorDim(track);
                if (CheckCollisionPointRec(e->pos, m_thumbRect)){
                    dragging = true;
                    m_dragStartMouse = majorPos(e->pos);
                    m_dragStartOffset = scrollOffset;
                    return true;
                }
                if (CheckCollisionPointRec(e->pos, track)){
                    float norm = (majorPos(e->pos) - majorPos({track.x, track.y})) / tSize;
                    scrollOffset = norm * maxOffset;
                    atBottom = (scrollOffset >= maxOffset);
                    dragging = true;
                    m_dragStartMouse = majorPos(e->pos);
                    m_dragStartOffset = scrollOffset;
                    return true;
                }
            } else {
                if (dragging){
                    dragging = false;
                    return true;
                }
            }
        }
        return false;
    }

    if (auto* e = std::get_if<CursorMoveEvent>(&event)){
        m_thumbHovered = CheckCollisionPointRec(e->pos, m_thumbRect);
        if (dragging){
            float tSize = majorDim(m_lastViewport);
            float thumb = majorDim(m_thumbRect);
            float usable = tSize - thumb;
            if (usable > 0){
                float delta = majorPos(e->pos) - m_dragStartMouse;
                scrollOffset = m_dragStartOffset + delta * (maxOffset / usable);
                atBottom = (scrollOffset >= maxOffset);
            }
            return true;
        }
        return false;
    }

    return false;
}

void Scrollable::OnUpdate(Rectangle drawRect, Vector2 contentSize){
    m_lastViewport = drawRect;
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

void Scrollable::DrawScrollbar(Rectangle viewport){
    if (maxOffset <= 0 || !showScrollbar) return;

    float vMajor = direction == ScrollDirection::Vertical ? viewport.height : viewport.width;
    float visibleRatio = std::min(1.0f, vMajor / (vMajor + maxOffset));
    float thumbMajor = vMajor * visibleRatio;

    if (direction == ScrollDirection::Vertical){
        Rectangle track{viewport.x + viewport.width - scrollbarWidth, viewport.y,
                        scrollbarWidth, viewport.height};
        float maxThumbPos = track.height - thumbMajor;
        float thumbY = maxThumbPos > 0 ? track.y + (scrollOffset / maxOffset) * maxThumbPos : track.y;
        m_thumbRect = {track.x, thumbY, track.width, thumbMajor};
        ::DrawRectangleRec(track, trackColor);
    } else {
        Rectangle track{viewport.x, viewport.y + viewport.height - scrollbarWidth,
                        viewport.width, scrollbarWidth};
        float maxThumbPos = track.width - thumbMajor;
        float thumbX = maxThumbPos > 0 ? track.x + (scrollOffset / maxOffset) * maxThumbPos : track.x;
        m_thumbRect = {thumbX, track.y, thumbMajor, track.height};
        ::DrawRectangleRec(track, trackColor);
    }

    ::DrawRectangleRec(m_thumbRect, m_thumbHovered ? thumbHoverColor : thumbColor);
}
