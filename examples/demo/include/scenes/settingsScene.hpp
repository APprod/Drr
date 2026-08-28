#pragma once
#include "scene/baseScene.hpp"

class SettingsScene:  public BaseScene{
    void OnEnter() override;
    void OnDrawContent() override;
    void OnUpdateState() override;
private:
    int m_pendingSizeIndex{5};
    int m_displayedSizeIndex{0};
    std::string m_pendingFontName;
};