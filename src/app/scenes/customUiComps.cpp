#include "app/scenes/customUiComps.hpp"

TestComp::TestComp(Vector2 targetSize)
{
    this->m_targetSize = targetSize;
}
bool TestComp::OnUpdate() { return false; }

void TestComp::OnDrawContent()
{
    ::DrawRectangleRec(m_actual, RAYWHITE);
}

void TestComp::MeasureContent(Vector2 available)
{
    UIComponent::MeasureContent(available);
}

void TestComp::ArrangeContent(Rectangle actualRect)
{
    UIComponent::ArrangeContent(actualRect);
}


