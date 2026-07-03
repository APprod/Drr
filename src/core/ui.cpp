#include "core/ui.hpp"
#include "core/util.hpp"
#include "core/structs.hpp"

// helper type for the visitor #4
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}

Layout::Layout(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
        : UIComponent(uiSpec), layoutSpec(layoutSpec) {}

void Layout::AddChild(std::unique_ptr<UIComponent>&& child)
{
    children.push_back(std::move(child));
}

Color GetColor(){
    return Color{225,225,225,100};
}

void Layout::OnDraw()
{
    // ::DrawRectangleRec(actual,GetColor());
    for(auto& child : children)
        child->OnDraw();
}

void Layout::OnUpdate()
{
    for(auto& child : children)
        child->OnUpdate();
}

Vector2 Root::getPos(const MyEvent& event){
    Vector2 res = std::visit(overloaded{
        [](const CursorMoveEvent& e) -> Vector2 {return e.pos;},
        [](const CursorActionEvent& e) -> Vector2 {return e.pos;},
        [](const auto&){throw std::runtime_error("Required to get a position of an event without the position"); return Vector2{0,0};}
    }, event);
    return res;
}

EventResult Root::OnEvent(const MyEvent& event){
    if (m_captured){
        EventMask mask = m_captured->getCaptureTypes();
        if (mask & getEventType(event)){
            EventResult result = m_captured->OnEvent(event);
            if (result == EventResult::ReleaseCapture){
                m_captured = nullptr;
            }
            return result;
        }
    }
    { //normal execution
        EventResult result = Layout::OnEvent(event);
        if (result == EventResult::RequireCapture){
            Vector2 target = getPos(event);
            m_captured = FindTarget(target);
        }
        return result;
    }
}


UIComponent* Layout::FindTarget(Vector2 point){
    for (auto it = children.rbegin(); it != children.rend(); ++it){
        auto& child = (*it);
        UIComponent* comp = child->FindTarget(point);
        if (comp){ //we found it
            return comp;
        }
    }
    if (this->HitTest(point)) return this;
    return nullptr;
}
EventResult Layout::OnEvent(const MyEvent& event){
    // bool hitTestedEvent = std::holds_alternative<CursorActionEvent>(event);
    for (auto it = children.rbegin(); it != children.rend(); ++it){
        auto& child = (*it);
        EventResult result = child->OnEvent(event);
        if (result == EventResult::NotHandled) continue;        
        return result;
    }
    return UIComponent::OnEvent(event);
}

void Layout::MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis) {
    contentSize = {0, 0};
    for (auto& child : children) {
        child->OnMeasure(available);

        contentSize.*crossAxis = std::max(
            contentSize.*crossAxis,
            child->DesiredSize().*crossAxis
        );
        contentSize.*mainAxis += child->DesiredSize().*mainAxis + layoutSpec.spacing;
    }
    if (!children.empty()){
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
    for (auto& child : children) {
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
    for (auto&child: children){
        child->OnMeasure(available);
    }
}
void Stack::ArrangeContent(Rectangle rect){
    for (auto&child: children){
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
    ::DrawTexturePro(texture,rect(texture), target,{0,0},0.f,RAYWHITE);
    ::DrawText(m_text.c_str(),
                 static_cast<int>(target.x + 5),
                 static_cast<int>(target.y + target.height / 2 - 10),
                 20, BLACK);                   
    if (m_hover && !m_hold){
        ::DrawRectanglePro(target,{0,0},0.f,{255,255,255,50});
    }
    else if (m_hover && m_hold){
        ::BeginBlendMode(BlendMode::BLEND_MULTIPLIED);
        ::DrawRectanglePro(target,{0,0},0.f,{150,150,150,255});
        ::EndBlendMode();
    }
}

EventResult Button::OnEvent(const MyEvent& event){
    if (std::holds_alternative<CursorActionEvent>(event)){
        CursorActionEvent btn = std::get<CursorActionEvent>(event);
        if (btn.button == CursorAction::MOUSE_BUTTON_LEFT && btn.pressed){
            if (HitTest(btn.pos)){
                m_hold = true;
                return EventResult::RequireCapture;
            }
        }
        if (btn.button == CursorAction::MOUSE_BUTTON_LEFT && !btn.pressed){
            if (HitTest(btn.pos)){
                m_onClick();
            }
            m_hold = false;
            return EventResult::ReleaseCapture;
        }
    }
    if (std::holds_alternative<CursorMoveEvent>(event)){
        CursorMoveEvent move = std::get<CursorMoveEvent>(event);
        m_hover = HitTest(move.pos);
    }
    return EventResult::NotHandled;
}

void Button::OnUpdate(){
}
