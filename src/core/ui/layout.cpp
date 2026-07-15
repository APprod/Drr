#include "core/ui/layout.hpp"
#include "core/util.hpp"

#include <algorithm>
#include <numeric>
#include <cmath>



void Layout::AddChild(std::unique_ptr<UIComponent>&& child)
{
    m_children.push_back(std::move(child));
}

void Layout::OnDrawContent()
{
    for(auto& child : m_children){
        if (!child->visible) continue;
        child->OnDraw();
    }
}

bool Layout::OnUpdate()
{
    bool dirty = false;
    for(auto& child : m_children){
        dirty = child->OnUpdate() || dirty;
    }
    return dirty;
}

UIComponent* Layout::FindTarget(Vector2 point){
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it){
        auto& child = (*it);
        if (!child->interactive) continue;
        UIComponent* comp = child->FindTarget(point);
        if (comp){
            return comp;
        }
    }
    return nullptr;
}

EventResult Layout::OnEvent(const MyEvent& event){
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it){
        auto& child = (*it);
        EventResult result = child->OnEvent(event);
        if (result == EventResult::NotHandled) continue;
        return result;
    }
    return EventResult::NotHandled;
}

void Layout::MeasureAxialLayout(Vector2 available, Axis mainAxis, Axis crossAxis) {
    m_contentDesiredSize = {0, 0};
    for (auto& child : m_children) {
        child->OnMeasure(available);

        m_contentDesiredSize.*crossAxis = std::max(
            m_contentDesiredSize.*crossAxis,
            child->DesiredSize().*crossAxis
        );
        m_contentDesiredSize.*mainAxis += child->DesiredSize().*mainAxis + m_layoutSpec.spacing;
    }
    if (!m_children.empty()){
        m_contentDesiredSize.*mainAxis -= m_layoutSpec.spacing;
    }
}

void Layout::ResolveFlex(std::vector<Vector2>& sizes, Vector2 innerDim, Axis mainAxis, float& spare,
    float Flex::*flexField, Vector2 UIComponentSpec::*limitField, float tolerance){

    std::vector<bool> frozen(m_children.size());
    for (size_t i = 0; i < m_children.size(); ++i)
        frozen[i] = m_children[i]->Spec().flex.*flexField == 0;

    while ((spare > 0 && spare > tolerance) || (spare < 0 && spare < -tolerance))
    {
        float total = 0;
        for (size_t i = 0; i < m_children.size(); ++i)
            if (!frozen[i]) total += m_children[i]->Spec().flex.*flexField;
        if (total == 0) break;

        bool changed = false;
        for (size_t i = 0; i < m_children.size(); ++i)
        {
            if (frozen[i]) continue;
            sizes[i].*mainAxis += spare * m_children[i]->Spec().flex.*flexField / total;

            float limit = (m_children[i]->Spec().*limitField).*mainAxis;
            if ((spare > 0 && sizes[i].*mainAxis >= limit) ||
                (spare < 0 && sizes[i].*mainAxis <= limit))
            {
                sizes[i].*mainAxis = limit;
                frozen[i] = true;
            }
            changed = true;
        }

        float used = static_cast<float>(m_layoutSpec.spacing * std::max(0, static_cast<int>(m_children.size()) - 1));
        for (auto& s : sizes) used += s.*mainAxis;
        spare = innerDim.*mainAxis - used;
        if (!changed) break;
    }
}

std::vector<Vector2> Layout::CalculateFlex(Vector2 innerDim, Axis mainAxis, Axis crossAxis, float& spare){
    std::vector<Vector2> res; res.reserve(m_children.size());
    Flex totalFlex{0,0};
    float totalDesiredSize{0};
    for (auto& child : m_children) {
        totalDesiredSize += child->DesiredSize().*mainAxis + m_layoutSpec.spacing;
        totalFlex.growth += child->Spec().flex.growth;
        totalFlex.shrink += child->Spec().flex.shrink;
    }
    if (!m_children.empty())
        totalDesiredSize -= m_layoutSpec.spacing;

    spare = innerDim.*mainAxis - totalDesiredSize;

    for (auto& child : m_children)
        res.push_back(child->DesiredSize());

    if (spare > 0.01f)
        ResolveFlex(res, innerDim, mainAxis, spare, &Flex::growth, &UIComponentSpec::maxSize, 0.01f);
    else if (spare < -0.01f)
        ResolveFlex(res, innerDim, mainAxis, spare, &Flex::shrink, &UIComponentSpec::minSize, 0.01f);

    for (size_t i = 0; i < m_children.size(); ++i){
        if (m_layoutSpec.crossShrink)
            res[i].*crossAxis = std::min(res[i].*crossAxis, innerDim.*crossAxis);

        res[i].x = std::clamp(res[i].x,
            m_children[i]->Spec().minSize.x, m_children[i]->Spec().maxSize.x);
        res[i].y = std::clamp(res[i].y,
            m_children[i]->Spec().minSize.y, m_children[i]->Spec().maxSize.y);
    }
    return res;
}

void Layout::ArrangeAxialLayout(Rectangle innerRect, Axis mainAxis, Axis crossAxis) {
    Vector2 innerPosStart = { innerRect.x, innerRect.y };
    Vector2 innerPos = innerPosStart;
    Vector2 innerDim = { innerRect.width, innerRect.height };

    float spare{0};
    auto sizes = CalculateFlex(innerDim, mainAxis, crossAxis, spare);
    for (auto& s : sizes) {
        s.*mainAxis = std::round(s.*mainAxis);
        s.*crossAxis = std::round(s.*crossAxis);
    }
    for (auto& s : sizes) {
        s.*mainAxis = std::round(s.*mainAxis);
        s.*crossAxis = std::round(s.*crossAxis);
    }

    for (size_t i = 0; i < m_children.size(); ++i)
    {
        m_children[i]->OnMeasure(sizes[i]);
    }

    spare = {0};
    sizes = CalculateFlex(innerDim, mainAxis, crossAxis, spare);
    for (auto& s : sizes) {
        s.*mainAxis = std::round(s.*mainAxis);
        s.*crossAxis = std::round(s.*crossAxis);
    }
    for (auto& s : sizes) {
        s.*mainAxis = std::round(s.*mainAxis);
        s.*crossAxis = std::round(s.*crossAxis);
    }

    float spacing = static_cast<float>(m_layoutSpec.spacing);

    if (m_layoutSpec.justifyContent == JustifyContent::SpaceEvenly){
        if (sizes.size() > 1)
        {
            float total = std::accumulate(sizes.begin(),sizes.end(),0.0f,
            [mainAxis](float a, const Vector2& v){return a + v.*mainAxis;});

            float occupied = total + m_layoutSpec.spacing * (sizes.size() - 1);
            float missed = innerDim.*mainAxis - occupied;
            missed = std::max(0.0f, missed);

            spacing = m_layoutSpec.spacing + missed / (sizes.size() - 1);
            spacing = std::round(spacing);
        }
    }else if (m_layoutSpec.justifyContent == JustifyContent::None){
        Vector2 offset{0,0};
        if (std::abs(spare) > 0.01f){
            switch (m_layoutSpec.align)
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

    for (size_t i = 0; i < m_children.size(); ++i) {
        auto finalSize = sizes[i];

        switch (m_layoutSpec.crossAlign)
        {
        case Alignment::Beginning: innerPos.*crossAxis = innerPosStart.*crossAxis; break;
        case Alignment::Center: innerPos.*crossAxis = std::round((innerPosStart + (innerDim - finalSize)/2).*crossAxis); break;
        case Alignment::End: innerPos.*crossAxis = std::round((innerPosStart + (innerDim - finalSize)).*crossAxis); break;
        default: break;
        }

        Rectangle r = rect(innerPos, finalSize);
        m_children[i]->OnArrange(r);
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
    m_contentDesiredSize = available;
    for (auto&child: m_children){
        child->OnMeasure(available);
    }
}
void Stack::ArrangeContent(Rectangle rect){
    for (auto&child: m_children){
        child->OnArrange(rect);
    }
}
