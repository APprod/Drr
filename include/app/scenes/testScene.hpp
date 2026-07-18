#pragma once
#include "core/scene.hpp"
#include "core/recManager.hpp"
#include "core/animated.hpp"


class TestScene: public BaseScene{
public:
    TestScene(RecourceManager& manager);
    void OnEnter() override;
    void OnRestore() override;
    void OnDrawContent() override;
    void OnUpdateState() override;

private:
    RecourceManager& m_manager;
    
};