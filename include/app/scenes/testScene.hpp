#pragma once
#include "core/scene.hpp"
#include "core/ui.hpp"
#include "core/recManager.hpp"


class TestScene: public BaseScene{
public:
    TestScene(RecourceManager& manager);
    void OnEnter() override;
    void OnResize() override;
    void OnDrawContent() override;
    void OnUpdateState() override;
private:
    RecourceManager& m_manager;
};