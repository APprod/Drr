#include "app/scenes/customUiComps.hpp"

TestComp::TestComp(Vector2 targetSize)
{
    this->targetSize = targetSize;
}
bool TestComp::OnUpdate() { return false; }

void TestComp::OnDrawContent()
{
    ::DrawRectangleRec(actual, RAYWHITE);
}

void TestComp::MeasureContent(Vector2 available)
{
    UIComponent::MeasureContent(available);
}

void TestComp::ArrangeContent(Rectangle actualRect)
{
    UIComponent::ArrangeContent(actualRect);
}


