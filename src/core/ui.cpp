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

bool Layout::OnEvent(const MyEvent& event){
    bool hitTestedEvent = std::holds_alternative<CursorActionEvent>(event);
    for (auto it = childs.rbegin(); it != childs.rend(); ++it){
        auto& child = (*it);
        if (hitTestedEvent){
            if (child->HitTest(std::get<CursorActionEvent>(event).pos)){
                bool handled = child->OnEvent(event);
                if (handled) return true;        
            }
        }else{
            bool handled = child->OnEvent(event);
            if (handled) return true;        
        }
    }
    return UIComponent::OnEvent(event);
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
}
void Layout::ArrangeAxialLayout(Rectangle innerRect, Axis mainAxis) {
    Vector2 innerPos = { innerRect.x, innerRect.y };
    Vector2 innerDim = { innerRect.width, innerRect.height };
    float offset = 0.0f;
    
    switch (layoutSpec.align)
    {
    case Alignment::Center:{
        offset = (innerDim.*mainAxis - contentSize.*mainAxis) / 2;
        break;
    }
    case Alignment::End:{
        offset = (innerDim.*mainAxis - contentSize.*mainAxis);
        break;
    }
    default:
        break;
    }
    innerPos.*mainAxis += offset;
    for (auto& child : childs) {
        Rectangle r = rect(innerPos, child->DesiredSize());
        child->OnArrange(r);
        Vector2 dims = {child->FinalRect().width, child->FinalRect().height};
        innerPos.*mainAxis += dims.*mainAxis + layoutSpec.spacing;
    }
}

void VerticalLayout::MeasureContent(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::y, &Vector2::x);
}

void VerticalLayout::ArrangeContent(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::y);
}


void HorizontalLayout::MeasureContent(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::x, &Vector2::y);
}

void HorizontalLayout::ArrangeContent(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::x);
}

void Stack::MeasureContent(Vector2 available){
    contentSize = available;
    for (auto&child: childs){
        child->OnMeasure(available);
    }
}
void Stack::ArrangeContent(Rectangle rect){
    for (auto&child: childs){
        child->OnArrange(rect);
    }
}

Button::Button(
    std::string text,
    std::function<void()> onClick,
    std::string textureName,
    Vector2 targetSize,
    UIComponentSpec spec
): UIComponent{spec}, m_text{text}, m_onClick{onClick}, m_textureName{textureName} {
    this->targetSize = targetSize;
}

void Button::OnDraw(){
    auto& manager = GetServices().recManager;
    auto texture = manager.getTexture(m_textureName);
    auto target = GetDrawRect();
    DrawTexturePro(texture,rect(texture), target,{0,0},0.f,RAYWHITE);
    DrawText(m_text.c_str(),
                 static_cast<int>(target.x + 5),
                 static_cast<int>(target.y + target.height / 2 - 10),
                 20, BLACK);                   
}

bool Button::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorActionEvent>(event)){
        CursorActionEvent btn = std::get<CursorActionEvent>(event);
        if (btn.button == CursorAction::MOUSE_BUTTON_LEFT && btn.pressed){
            m_onClick();
            return true;
        }
    }
    return false;
}

void Button::OnUpdate(){
    // auto target = GetDrawRect();
    // bool hovered = CheckCollisionPointRec(GetMousePosition(), target);

    // if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    //     if (m_onClick) m_onClick();
    // }
}