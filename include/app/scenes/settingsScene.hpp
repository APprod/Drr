#pragma once
#include "core/scene.hpp"

class SettingsScene:  public BaseScene{
    void OnEnter() override;
    void OnDrawContent() override;
    void OnUpdateState() override;
};