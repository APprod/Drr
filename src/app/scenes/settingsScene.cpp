#include "app/scenes/settingsScene.hpp"
#include "core/ui/layout.hpp"

void SettingsScene::OnEnter(){

}
void SettingsScene::OnDrawContent(){
    auto row = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct({1.0,1.0,1.0,1.0}),
        LayoutSpec{}.AlignEnd()
    );
    
    auto settingsColumn = std::make_unique<VerticalLayout>(
        UICSpec{}.SetPaddingPct({1.0,1.0,1.0,1.0}),
        LayoutSpec{}.AlignBegin()
    );

    row->AddChild(std::move(settingsColumn));
    root.AddChild(std::move(row));
}
void SettingsScene::OnUpdateState(){
    auto& manager = GetServices().recManager;
    auto drawCall = [&manager](){
        ::DrawTexture(manager.getTexture("menu"), 0, 0, RAYWHITE);
    };
    useShader(
        manager.getShaderProgram("processing"),
        {{"brightness", 2.0f},
        {"saturation", 1.2f},
        },
        drawCall
    );
}