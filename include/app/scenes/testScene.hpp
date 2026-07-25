#pragma once
#include "core/scene/scene.hpp"
#include "core/rendering/resManager.hpp"
#include "core/utils/animated.hpp"


class TestScene: public BaseScene{
public:
    TestScene();
    void OnEnter() override;
    void OnRestore() override;
    void OnDrawContent() override;
    void OnUpdateState() override;

};