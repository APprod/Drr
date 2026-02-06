#include "core/ui.hpp"
#include "core/util.hpp"


Layout::Layout(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
        : UIComponent(uiSpec), layoutSpec(layoutSpec) {}

void Layout::AddChild(std::unique_ptr<UIComponent>&& child)
{
    childs.push_back(std::move(child));
}

Color GetColor(){
    return Color{225,225,225,100};
}

void Layout::OnDraw()
{
    // ::DrawRectangleRec(actual,GetColor());
    for(auto& child : childs)
        child->OnDraw();
}

void Layout::OnUpdate()
{
    for(auto& child : childs)
        child->OnUpdate();
}

void Layout::MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis) {
    contentSize = {0, 0};

    for (auto& child : childs) {
        child->OnMeasure(available);

        contentSize.*crossAxis = std::max(
            contentSize.*crossAxis,
            child->DesiredSize().*crossAxis
        );
        contentSize.*mainAxis += child->DesiredSize().*mainAxis + layoutSpec.spacing;
    }
    if (!childs.empty()){
        contentSize.*mainAxis -= layoutSpec.spacing;
    }
    desiredSize = contentSize;
    switch (compSpec.fillMode) {
        case FillMode::FillMaxWidth: desiredSize.x = available.x; break;
        case FillMode::FillMaxHeight: desiredSize.y = available.y; break;
        case FillMode::FillMaxSize: desiredSize = available; break;
        default: break;
    }
}
void Layout::ArrangeAxialLayout(Rectangle actualRect, Axis mainAxis) {
    actual = actualRect;

    Vector2 pos{};
    switch (layoutSpec.align)
    {
    case Alignment::Beginning:{
        pos = {actualRect.x, actualRect.y};
        for (auto& child : childs) {
            Rectangle r = rect(pos, child->DesiredSize());
            child->OnArrange(r);
            Vector2 dims = {child->FinalRect().width, child->FinalRect().height};
            pos.*mainAxis += dims.*mainAxis + layoutSpec.spacing;
        }
        break;
    }
    case Alignment::Center:{
        Vector2 actualDim = {actualRect.width, actualRect.height};
        auto difference = actualDim.*mainAxis - contentSize.*mainAxis;
        pos = {actualRect.x, actualRect.y};
        pos.*mainAxis += difference/2;
        for (auto& child : childs) {
            Rectangle r = rect(pos, child->DesiredSize());
            child->OnArrange(r);
            Vector2 dims = {child->FinalRect().width, child->FinalRect().height};
            pos.*mainAxis += dims.*mainAxis + layoutSpec.spacing;
        }
        break;
    }
    case Alignment::End:{
        pos = {actualRect.x + actualRect.width, actualRect.y + actualRect.height};
        for (auto& child : childs) {
            Rectangle r = rect(pos - child->DesiredSize(), child->DesiredSize());
            child->OnArrange(r);
            Vector2 dims = {child->FinalRect().width, child->FinalRect().height};
            pos.*mainAxis -= dims.*mainAxis + layoutSpec.spacing;
        }
        break;
    }
    default:
        break;
    }
}

void VerticalLayout::OnMeasure(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::y, &Vector2::x);
}

void VerticalLayout::OnArrange(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::y);
}


void HorizontalLayout::OnMeasure(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::x, &Vector2::y);
}

void HorizontalLayout::OnArrange(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::x);
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

void Button::OnDraw(){
    auto& manager = GetServices().recManager;
    auto texture = manager.getTexture(m_textureName);
    DrawTexturePro(texture,rect(texture),actual,{0,0},0.f,RAYWHITE);
    DrawText(m_text.c_str(),
                 static_cast<int>(actual.x + 5),
                 static_cast<int>(actual.y + actual.height / 2 - 10),
                 20, BLACK);                   
}

void Button::OnUpdate(){
    bool hovered = CheckCollisionPointRec(GetMousePosition(), actual);
    // Если мышь над кнопкой и отпустили левую кнопку
    if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (m_onClick) m_onClick();
    }
}