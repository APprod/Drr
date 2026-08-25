#pragma once
#include "ui/component.hpp"

class TestComp: public UIComponent{
public:
    TestComp(Vector2 targetSize);
    bool OnUpdate(float dt) override;
    void OnDrawContent() override;
    void MeasureContent(Vector2 available) override;
    void ArrangeContent(Rectangle actualRect) override;
};
