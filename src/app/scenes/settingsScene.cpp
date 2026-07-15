#include "app/scenes/settingsScene.hpp"
#include "core/ui/layout.hpp"
#include "core/ui/slider.hpp"
#include "core/ui/checkbox.hpp"
#include "core/ui/valueLabel.hpp"
#include "core/ui/overlay.hpp"
#include "core/ui/debugOverlay.hpp"
#include "core/ui/modifier.hpp"

void SettingsScene::OnEnter(){
    
    UICSpec sliderSpec;
    auto [sBarH, sMinTh, sMaxTh, sTarget] = std::make_tuple(4.0f, 5.0f, 10.0f, Vector2{200,10});
    auto& usrCfg = GetServices().runtimeCfg.user;
    auto fontSize = 20;

    auto row = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct({.top =  10.0, .bottom =  10.0, .left = 10.0, .right = 10.0}),
        LayoutSpec{}.AlignEnd().CrossBegin(),

        HorizontalLayout( UICSpec{}.SetPaddingPct({1.0,1.0,1.0,1.0}), LayoutSpec{}.AlignBegin(),

            Checkbox(Text("VSYNC", "TNR", fontSize, 0), &usrCfg.vsync,
                [](bool vsync){
                    if (vsync) {SetWindowState(FLAG_VSYNC_HINT);}
                    else {ClearWindowState(FLAG_VSYNC_HINT);}
                }
            ),
            Slider<int>(&usrCfg.targetFPS, 0, 240, 
                [](int value){
                    SetTargetFPS(value);
                }
                , sliderSpec, sBarH, sMinTh, sMaxTh, sTarget, 5
            ),
            ValueLabel<int>("Target FPS: {}", &usrCfg.targetFPS, Text("", "TNR", fontSize, 0))
        )
    );

    root.AddChild(std::move(row));
    auto overlay = std::make_unique<Overlay>();
    overlay->Bind(InputKey::KEY_ESCAPE, [](){
        GetServices().sceneManager.QuePop();
    });
    root.AddChild(std::move(overlay));
    root.AddChild(std::make_unique<DebugOverlay>());
    OnResize();
}

void SettingsScene::OnDrawContent(){
    auto& manager = GetServices().recManager;
    auto drawCall = [&manager](){
        ::DrawTexture(manager.getTexture("menu"), 0, 0, RAYWHITE);
    };
    useShaderUnchecked(
        manager.getShaderProgram("processing"),
        {{"brightness", 2.0f},
        {"saturation", 1.2f},
        },
        drawCall
    );
    root.OnDraw();
}

void SettingsScene::OnUpdateState(){
    
}