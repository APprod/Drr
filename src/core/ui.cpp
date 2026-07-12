#include "core/ui.hpp"
#include "core/util.hpp"
#include "core/structs.hpp"
#include <numeric>


// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}

Padding UICSpec::ResolvePadding(Vector2 dims) const{
    return {
        static_cast<int>(padding.top + dims.y*paddingPct.top / 100.0f),
        static_cast<int>(padding.bottom + dims.y*paddingPct.bottom / 100.0f),
        static_cast<int>(padding.left + dims.x*paddingPct.left / 100.0f),
        static_cast<int>(padding.right + dims.x*paddingPct.right / 100.0f),
    };
}

Layout::Layout(UIComponentSpec uiSpec, LayoutSpec layoutSpec)
        : UIComponent(uiSpec), layoutSpec(layoutSpec) {}

void Layout::AddChild(std::unique_ptr<UIComponent>&& child)
{
    children.push_back(std::move(child));
}

void Layout::OnDraw()
{
    for(auto& child : children)
        child->OnDraw();
}

void Layout::OnUpdate()
{
    for(auto& child : children)
        child->OnUpdate();
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
    contentDesiredSize = {0, 0};
    for (auto& child : children) {
        child->OnMeasure(available);

        contentDesiredSize.*crossAxis = std::max(
            contentDesiredSize.*crossAxis,
            child->DesiredSize().*crossAxis
        );
        contentDesiredSize.*mainAxis += child->DesiredSize().*mainAxis + layoutSpec.spacing;
    }
    if (!children.empty()){
        contentDesiredSize.*mainAxis -= layoutSpec.spacing;
    }
}

std::vector<Vector2> Layout::CalculateFlex(Vector2 innerDim, Axis mainAxis, Axis crossAxis, float& spare, Flex& totalFlex){
    std::vector<Vector2> res; res.reserve(children.size());
    //Calculate total desired size
    float totalDesiredSize{0};
    {
        for (auto& child : children) {
            totalDesiredSize += child->DesiredSize().*mainAxis + layoutSpec.spacing;
            totalFlex.growth += child->Spec().flex.growth;
            totalFlex.shrink += child->Spec().flex.shrink;
        }
        if (!children.empty()) totalDesiredSize -= layoutSpec.spacing; //compensate for the extra one
    }

    spare = innerDim.*mainAxis - totalDesiredSize;

    for (auto& child : children) {
        auto finalSize = child->DesiredSize();
        auto childFlex = child->Spec().flex;
        if (spare > 0 && childFlex.growth > 0 && totalFlex.growth > 0){
            finalSize.*mainAxis += spare * childFlex.growth / totalFlex.growth;
        }
        else if (spare < 0 && childFlex.shrink > 0 && totalFlex.shrink > 0){
            finalSize.*mainAxis += spare * childFlex.shrink / totalFlex.shrink;
        }
        if (layoutSpec.crossShrink){
            finalSize.*crossAxis = std::min(finalSize.*crossAxis, innerDim.*crossAxis);
        }
        finalSize.y = std::min(finalSize.y, child->Spec().maxSize.y);
        finalSize.y = std::max(finalSize.y, child->Spec().minSize.y);
        finalSize.x = std::min(finalSize.x, child->Spec().maxSize.x);
        finalSize.x = std::max(finalSize.x, child->Spec().minSize.x);
        res.push_back(finalSize);
    } 
    return res;
}

void Layout::ArrangeAxialLayout(Rectangle innerRect, Axis mainAxis, Axis crossAxis) {
    //Assume we now have flex.
    Vector2 innerPosStart = { innerRect.x, innerRect.y };
    Vector2 innerPos = innerPosStart;
    Vector2 innerDim = { innerRect.width, innerRect.height };
    
    float spare{0};
    Flex totalFlex{0,0};
    auto sizes = CalculateFlex(innerDim, mainAxis, crossAxis, spare, totalFlex);
    
    
    
    float spacing = layoutSpec.spacing;

    if (layoutSpec.justifyContent == JustifyContent::SpaceEvenly){    
        if (sizes.size() > 1)
        {
            float total = std::accumulate(sizes.begin(),sizes.end(),0.0f,
            [mainAxis](float a, const Vector2& v){return a + v.*mainAxis;});

            float occupied = total + layoutSpec.spacing * (sizes.size() - 1);
            float missed = innerDim.*mainAxis - occupied;
            missed = std::max(0.0f, missed);
            
            spacing += missed / (sizes.size() - 1);
        }
    }else if (layoutSpec.justifyContent == JustifyContent::None){
        Vector2 offset{0,0};
        if (!((spare > 0 && totalFlex.growth > 0) || (spare < 0 && totalFlex.shrink > 0))){
            switch (layoutSpec.align)
            {
            case Alignment::Center:{
                offset.*mainAxis = spare / 2; break;
            }
            case Alignment::End:{
                offset.*mainAxis = spare; break;
            }
            default: break;
            }
        }

        innerPos.*mainAxis += offset.*mainAxis;
    }

    for (size_t i = 0; i < children.size(); ++i) {
        auto finalSize = sizes[i];

        switch (layoutSpec.crossAlign)
        {
        case Alignment::Beginning: innerPos.*crossAxis = innerPosStart.*crossAxis; break;
        case Alignment::Center: innerPos.*crossAxis = (innerPosStart + (innerDim - finalSize)/2).*crossAxis; break;
        case Alignment::End: innerPos.*crossAxis = (innerPosStart + (innerDim - finalSize)).*crossAxis; break;
        default: break;
        }

        Rectangle r = rect(innerPos, finalSize);
        children[i]->OnArrange(r);
        innerPos.*mainAxis += finalSize.*mainAxis + spacing;
    } 
}



void VerticalLayout::MeasureContent(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::y, &Vector2::x);
}

void VerticalLayout::ArrangeContent(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::y,&Vector2::x);
}


void HorizontalLayout::MeasureContent(Vector2 available)
{
    MeasureAxialLayout(available, &Vector2::x, &Vector2::y);
}

void HorizontalLayout::ArrangeContent(Rectangle actualRect)
{
    ArrangeAxialLayout(actualRect, &Vector2::x, &Vector2::y);
}


void Stack::MeasureContent(Vector2 available){
    contentDesiredSize = available;
    for (auto&child: children){
        child->OnMeasure(available);
    }
}
void Stack::ArrangeContent(Rectangle rect){
    for (auto&child: children){
        child->OnArrange(rect);
    }
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
    PerfTester tester = GetServices().perfLog.log("Root::OnEvent");
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
            if (m_hold){
                if (HitTest(btn.pos)){
                    m_onClick();
                }
                m_hold = false;   
                return EventResult::ReleaseCapture; // always release if held
            }
            
        }
    }
    else if (std::holds_alternative<CursorMoveEvent>(event)){
        CursorMoveEvent move = std::get<CursorMoveEvent>(event);
        m_hover = HitTest(move.pos);
    }
    else if (std::holds_alternative<ScreenInterEvent>(event)){
        ScreenInterEvent inter = std::get<ScreenInterEvent>(event);
        if (inter.action == ScreenInteraction::EXIT){
            m_hover = false;
        }
        else if (inter.action == ScreenInteraction::ENTER){
            m_hover = false;
        }
    }
    return EventResult::NotHandled;
}

void Button::OnUpdate(){
}
