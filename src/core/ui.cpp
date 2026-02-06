#include "core/ui.hpp"
#include "core/util.hpp"


void Layout::AddChild(std::unique_ptr<UIComponent>&& child)
{
    childs.push_back(std::move(child));
}

void Layout::OnDraw()
{
    for(auto& child : childs)
        child->OnDraw();
}

void Layout::OnUpdate()
{
    for(auto& child : childs)
        child->OnUpdate();
}


void VerticalLayout::OnMeasure(Vector2 available)
{
    Vector2 size{0,0};
    for(auto& child : childs){
        child->OnMeasure(available);
        size.x = std::max(size.x, child->DesiredSize().x);
        size.y += child->DesiredSize().y + spacing;
    }
    desiredSize = size;
}

void VerticalLayout::OnArrange(Rectangle actualRect)
{
    actual = actualRect;
    Vector2 pos{actualRect.x, actualRect.y};
    for(auto& child: childs){
        child->OnArrange(rect(pos, child->DesiredSize()));
        pos.y += child->FinalRect().height + spacing;
    }
}

void HorizontalLayout::OnMeasure(Vector2 available)
{
    Vector2 size{0,0};
    for(auto& child : childs){
        child->OnMeasure(available);
        size.y = std::max(size.y, child->DesiredSize().y);
        size.x += child->DesiredSize().x + spacing;
    }
    desiredSize = size;
}

void HorizontalLayout::OnArrange(Rectangle actualRect)
{
    actual = actualRect;
    Vector2 pos{actualRect.x, actualRect.y};
    for(auto& child: childs){
        child->OnArrange(rect(pos, child->DesiredSize()));
        pos.x += child->FinalRect().width + spacing;
    }
}

void Root::OnMeasure(Vector2 available){
    desiredSize = available;
    for (auto&child: childs){
        child->OnMeasure(available);
    }
}
void Root::OnArrange(Rectangle rect){
    for (auto&child: childs){
        child->OnArrange(rect);
    }
}
