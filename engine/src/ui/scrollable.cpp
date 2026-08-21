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

    if (auto* e = std::get_if<InputKeyEvent>(&event)){
        if (!hovered || !e->pressed) return false;
        float jump = majorDim(m_lastViewport);
        if (e->key == KEY_HOME) {
            if (e->repeat) return false;
            scrollOffset = 0;
            atBottom = false;
        } else if (e->key == KEY_END) {
            if (e->repeat) return false;
            scrollOffset = maxOffset;
            atBottom = true;
        } else if (e->key == KEY_PAGE_UP) {
            scrollOffset -= jump;
            atBottom = false;
        } else if (e->key == KEY_PAGE_DOWN) {
            scrollOffset += jump;
        } else {
            return false;
        }
        myClamp(scrollOffset, 0.0f, maxOffset);
        if (scrollOffset >= maxOffset) atBottom = true;
        return true;
    }

    if (auto* e = std::get_if<CursorActionEvent>(&event)){
        if (e->button == CursorAction::MOUSE_BUTTON_LEFT){
            if (e->pressed){
                Rectangle track = m_lastViewport;
                if (CheckCollisionPointRec(e->pos, m_thumbRect)){
                    dragging = true;
                    m_dragStartMouse = majorPos(e->pos);
                    m_dragStartOffset = scrollOffset;
                    return true;
                }
                if (CheckCollisionPointRec(e->pos, track)){
                    auto g = thumbGeometry(m_lastViewport);
                    float trackPos = majorPos({track.x, track.y});
                    float clickPos = majorPos(e->pos);
                    float norm = 0.0f;
                    if (g.maxThumbPos > 0.0f) {
                        float thumbMajor = majorDim(g.thumb);
                        norm = (clickPos - trackPos - thumbMajor * 0.5f) / g.maxThumbPos;
                        norm = std::clamp(norm, 0.0f, 1.0f);
                    }
                    scrollOffset = norm * maxOffset;
                    myClamp(scrollOffset, 0.0f, maxOffset);
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

ScrollThumbGeometry Scrollable::thumbGeometry(Rectangle viewport) const {
    float vMajor = direction == ScrollDirection::Vertical ? viewport.height : viewport.width;
    float visibleRatio = std::min(1.0f, vMajor / (vMajor + maxOffset));
    float thumbMajor = vMajor * visibleRatio;

    ScrollThumbGeometry g;
    if (direction == ScrollDirection::Vertical){
        g.track = {viewport.x + viewport.width - scrollbarWidth, viewport.y,
                   scrollbarWidth, viewport.height};
        g.maxThumbPos = g.track.height - thumbMajor;
        float thumbY = g.maxThumbPos > 0 ? g.track.y + (scrollOffset / maxOffset) * g.maxThumbPos : g.track.y;
        g.thumb = {g.track.x, thumbY, g.track.width, thumbMajor};
    } else {
        g.track = {viewport.x, viewport.y + viewport.height - scrollbarWidth,
                   viewport.width, scrollbarWidth};
        g.maxThumbPos = g.track.width - thumbMajor;
        float thumbX = g.maxThumbPos > 0 ? g.track.x + (scrollOffset / maxOffset) * g.maxThumbPos : g.track.x;
        g.thumb = {thumbX, g.track.y, thumbMajor, g.track.height};
    }
    return g;
}

void Scrollable::DrawScrollbar(Rectangle viewport){
    if (maxOffset <= 0 || !showScrollbar) return;

    auto g = thumbGeometry(viewport);
    m_thumbRect = g.thumb;
    ::DrawRectangleRec(g.track, trackColor);
    ::DrawRectangleRec(m_thumbRect, m_thumbHovered ? thumbHoverColor : thumbColor);
}
