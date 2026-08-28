#pragma once
#include "scene/baseScene.hpp"
#include "rendering/resManager.hpp"
#include "utils/animated.hpp"


class TestScene: public BaseScene{
public:
    TestScene();
    void OnEnter() override;
    void OnRestore() override;
    void OnDrawContent() override;
    void OnUpdateState() override;

};