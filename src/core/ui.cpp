#include "core/ui.hpp"
#include "core/util.hpp"
#include "core/structs.hpp"
#include <numeric>
#include <cmath>


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

void Layout::OnDrawContent()
{
    for(auto& child : children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}

bool Layout::OnUpdate()
{
    bool dirty = false;
    for(auto& child : children){
        dirty = child->OnUpdate() || dirty;
    }
    return dirty;
}

UIComponent* Layout::FindTarget(Vector2 point){
    for (auto it = children.rbegin(); it != children.rend(); ++it){
        auto& child = (*it);
        if (!child->interactive) continue;
        UIComponent* comp = child->FindTarget(point);
        if (comp){ //we found it
            return comp;
        }
    }
    if (this->HitTest(point)) return this;
    return nullptr;
}

EventResult Layout::OnEvent(const MyEvent& event){
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


void Layout::ResolveFlex(std::vector<Vector2>& sizes, Vector2 innerDim, Axis mainAxis, float& spare,
    float Flex::*flexField, Vector2 UIComponentSpec::*limitField, float tolerance){
        
    std::vector<bool> frozen(children.size());
    for (size_t i = 0; i < children.size(); ++i)
        frozen[i] = children[i]->Spec().flex.*flexField == 0;

    while ((spare > 0 && spare > tolerance) || (spare < 0 && spare < -tolerance))
    {
        float total = 0;
        for (size_t i = 0; i < children.size(); ++i)
            if (!frozen[i]) total += children[i]->Spec().flex.*flexField;
        if (total == 0) break;

        bool changed = false;
        for (size_t i = 0; i < children.size(); ++i)
        {
            if (frozen[i]) continue;
            sizes[i].*mainAxis += spare * children[i]->Spec().flex.*flexField / total;

            float limit = (children[i]->Spec().*limitField).*mainAxis;
            if ((spare > 0 && sizes[i].*mainAxis >= limit) ||
                (spare < 0 && sizes[i].*mainAxis <= limit))
            {
                sizes[i].*mainAxis = limit;
                frozen[i] = true;
            }
            changed = true;
        }

        float used = layoutSpec.spacing * std::max<int>(0, children.size() - 1);
        for (auto& s : sizes) used += s.*mainAxis;
        spare = innerDim.*mainAxis - used;
        if (!changed) break;
    }
}


std::vector<Vector2> Layout::CalculateFlex(Vector2 innerDim, Axis mainAxis, Axis crossAxis, float& spare, Flex& totalFlex){
    std::vector<Vector2> res; res.reserve(children.size());

    float totalDesiredSize{0};
    for (auto& child : children) {
        totalDesiredSize += child->DesiredSize().*mainAxis + layoutSpec.spacing;
        totalFlex.growth += child->Spec().flex.growth;
        totalFlex.shrink += child->Spec().flex.shrink;
    }
    if (!children.empty())
        totalDesiredSize -= layoutSpec.spacing;

    spare = innerDim.*mainAxis - totalDesiredSize;

    for (auto& child : children)
        res.push_back(child->DesiredSize());

    if (spare > 0.01f)
        ResolveFlex(res, innerDim, mainAxis, spare, &Flex::growth, &UIComponentSpec::maxSize, 0.01f);
    else if (spare < -0.01f)
        ResolveFlex(res, innerDim, mainAxis, spare, &Flex::shrink, &UIComponentSpec::minSize, 0.01f);

    for (size_t i = 0; i < children.size(); ++i){
        if (layoutSpec.crossShrink)
            res[i].*crossAxis = std::min(res[i].*crossAxis, innerDim.*crossAxis);

        res[i].x = std::clamp(res[i].x,
            children[i]->Spec().minSize.x, children[i]->Spec().maxSize.x);
        res[i].y = std::clamp(res[i].y,
            children[i]->Spec().minSize.y, children[i]->Spec().maxSize.y);
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
    
    for (auto& s : sizes) {
        s.*mainAxis = std::round(s.*mainAxis);
        s.*crossAxis = std::round(s.*crossAxis);
    }

    float spacing = layoutSpec.spacing;

    if (layoutSpec.justifyContent == JustifyContent::SpaceEvenly){    
        if (sizes.size() > 1)
        {
            float total = std::accumulate(sizes.begin(),sizes.end(),0.0f,
            [mainAxis](float a, const Vector2& v){return a + v.*mainAxis;});

            float occupied = total + layoutSpec.spacing * (sizes.size() - 1);
            float missed = innerDim.*mainAxis - occupied;
            missed = std::max(0.0f, missed);
            
            spacing = layoutSpec.spacing + missed / (sizes.size() - 1);
            spacing = std::round(spacing);
        }
    }else if (layoutSpec.justifyContent == JustifyContent::None){
        Vector2 offset{0,0};
        if (std::abs(spare) > 0.01f){
            switch (layoutSpec.align)
            {
            case Alignment::Center:{
                offset.*mainAxis = std::round(spare / 2); break;
            }
            case Alignment::End:{
                offset.*mainAxis = std::round(spare); break;
            }
            default: break;
            }
        }

        innerPos.*mainAxis = std::round(innerPos.*mainAxis + offset.*mainAxis);
    }

    for (size_t i = 0; i < children.size(); ++i) {
        auto finalSize = sizes[i];

        switch (layoutSpec.crossAlign)
        {
        case Alignment::Beginning: innerPos.*crossAxis = innerPosStart.*crossAxis; break;
        case Alignment::Center: innerPos.*crossAxis = std::round((innerPosStart + (innerDim - finalSize)/2).*crossAxis); break;
        case Alignment::End: innerPos.*crossAxis = std::round((innerPosStart + (innerDim - finalSize)).*crossAxis); break;
        default: break;
        }

        Rectangle r = rect(innerPos, finalSize);
        children[i]->OnArrange(r);
        innerPos.*mainAxis += finalSize.*mainAxis + spacing;
        innerPos.*mainAxis = std::round(innerPos.*mainAxis);
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

void Root::UpdateHover()
{
    UIComponent* newHovered = nullptr;

    if (m_captured){
        if (m_captured->HitTest(m_cursorPos))
            newHovered = m_captured;
    }
    else{
        newHovered = FindTarget(m_cursorPos);
    }

    if (newHovered == m_hovered)
        return;
    if (m_hovered)
        m_hovered->OnHoverExit();

    m_hovered = newHovered;
    if (m_hovered)
        m_hovered->OnHoverEnter();
}

std::optional<EventResult> Root::CheckCaptured(const MyEvent& event){
    if (!m_captured)
        return std::nullopt;
    if (!(m_captured->getCaptureTypes() & getEventType(event)))
        return std::nullopt;

    EventResult result = m_captured->OnEvent(event);
    if (result == EventResult::ReleaseCapture)
    {
        m_captured = nullptr;
        UpdateHover();
    }
    return result;
}

EventResult Root::OnEvent(const MyEvent& event){
    PerfTester tester = GetServices().perfLog.log("Root::OnEvent");

    if (auto* move = std::get_if<CursorMoveEvent>(&event))
    {
        m_cursorPos = move->pos;
        if (auto r = CheckCaptured(event))
            return *r;
        Layout::OnEvent(event);
        UpdateHover();
        return EventResult::Handled;
    }

    if (auto* screen = std::get_if<ScreenInterEvent>(&event))
    {
        if (screen->action == ScreenInteraction::EXIT)
        {
            if (m_hovered)
            {
                m_hovered->OnHoverExit();
                m_hovered = nullptr;
            }
        }

        return EventResult::Handled;
    }
    { 
        if (auto r = CheckCaptured(event)) return *r;
        EventResult result = Layout::OnEvent(event);
        if (result == EventResult::RequireCapture)
        {
            m_captured = FindTarget(getPos(event));
            UpdateHover();
        }
        return result;
    }
}

Button::Button(
    std::string text,
    std::function<void()> onClick,
    std::string textureName,
    Vector2 targetSize,
    UIComponentSpec spec,
    float fontSize,
    std::string fontName
): UIComponent{spec}, m_text{text}, m_onClick{onClick}, m_textureName{textureName}, m_fontSize{fontSize}, m_fontName{fontName} {
    this->targetSize = targetSize;
}

void Button::OnDrawContent(){
    auto& manager = GetServices().recManager;
    auto texture = manager.getTexture(m_textureName);
    auto target = GetDrawRect();
    ::DrawTexturePro(texture, rect(texture), target, {0,0}, 0.f, RAYWHITE);

    auto font = manager.getFont(m_fontName, m_fontSize);
    Vector2 textSize = ::MeasureTextEx(font, m_text.c_str(), m_fontSize, 0);
    float textX = target.x + (target.width - textSize.x) / 2;
    float textY = target.y + (target.height - textSize.y) / 2;
    ::DrawTextEx(font, m_text.c_str(), {textX, textY}, m_fontSize, 0, BLACK);
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
        const auto& btn = std::get<CursorActionEvent>(event);
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
    return EventResult::NotHandled;
}

void Button::OnHoverEnter()
{
    m_hover = true;
}

void Button::OnHoverExit()
{
    m_hover = false;
}



Label::Label(
    std::string text,
    UIComponentSpec spec,
    std::string fontName,
    float fontSize,
    float fontSpacing,
    Color color
): UIComponent{spec}, m_text{text},
   m_fontName{fontName}, m_fontSize{fontSize},
   m_fontSpacing{fontSpacing}, m_color{color} 
{
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    m_lastMeasuredSize = ::MeasureTextEx(font, m_text.c_str(), m_fontSize, m_fontSpacing);
}

void Label::SetText(std::string text){
    m_text = std::move(text);
}

bool Label::OnUpdate()
{
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    Vector2 newSize = ::MeasureTextEx(font, m_text.c_str(), m_fontSize, m_fontSpacing);
    if (newSize != m_lastMeasuredSize) {
        m_lastMeasuredSize = newSize;
        return true;
    }
    return false;
}

void Label::MeasureContent(Vector2 available){
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    Vector2 textSize = ::MeasureTextEx(font, m_text.c_str(), m_fontSize, m_fontSpacing);
    contentDesiredSize = {
        std::min(available.x, textSize.x),
        std::min(available.y, textSize.y)
    };
}

void Label::OnDrawContent(){
    auto font = GetServices().recManager.getFont(m_fontName, m_fontSize);
    auto rect = GetDrawRect();
    ::DrawTextEx(font, m_text.c_str(), {rect.x, rect.y}, m_fontSize, m_fontSpacing, m_color);
}
