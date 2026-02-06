#include "app/scenes/customUiComps.hpp"

TestComp::TestComp(Vector2 targetSize)
{
    this->targetSize = targetSize;
}
void TestComp::OnUpdate() {}

void TestComp::OnDraw()
{
    ::DrawRectangleRec(actual, RAYWHITE);
}

void TestComp::OnMeasure(Vector2 available)
{
    UIComponent::OnMeasure(available);
}

void TestComp::OnArrange(Rectangle actualRect)
{
    UIComponent::OnArrange(actualRect);
}


