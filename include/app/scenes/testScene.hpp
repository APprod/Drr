#pragma once
#include "core/scene.hpp"
#include "core/resManager.hpp"
#include "core/animated.hpp"


class TestScene: public BaseScene{
public:
    TestScene(ResourceManager& manager);
    void OnEnter() override;
    void OnRestore() override;
    void OnDrawContent() override;
    void OnUpdateState() override;

private:
    ResourceManager& m_manager;
    
};