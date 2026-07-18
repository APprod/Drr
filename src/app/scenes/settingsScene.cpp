#include "app/scenes/settingsScene.hpp"
#include "core/ui.hpp"
#include "core/ui/dropdown.hpp"
#include "core/userSettings.hpp"

void SettingsScene::OnEnter(){
    
    UICSpec sliderSpec;
    auto [sBarH, sMinTh, sMaxTh, sTarget] = std::make_tuple(4.0f, 5.0f, 10.0f, Vector2{200,10});
    auto& usrCfg = GetServices().runtimeCfg.user;
    auto fontSize = 20;

    auto fonts = GetServices().recManager.getLoadedFonts();
    std::vector<std::pair<std::string, std::string>> fontItems;
    for (auto& f : fonts) fontItems.emplace_back(f, f);
    auto buttonText = Text("Test", "TNR", 32, 0, RAYWHITE);
    auto row = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct({.top =  10.0, .bottom =  10.0, .left = 10.0, .right = 10.0}),
        LayoutSpec{}.AlignEnd().CrossBegin(),
        VerticalLayout( UICSpec{}.SetFlex({0,1}), LayoutSpec{}.AlignBegin().CrossEnd().UniformCross(),
            
            Background(
                UICSpec{}.SetFlex({1,1}), 
                BackgroundStyle{.texture = "button_default", .tint = WHITE, .processing = ProcessingValues{.saturation = 0} },
                VerticalLayout( UICSpec{}.SetPadding({20,20,60,60}).SetFlex({1,1}), LayoutSpec{}.AlignCenter().CrossBegin(),
                    Checkbox(Text("VSYNC", "TNR", fontSize, 0), &usrCfg.vsync,
                        [](bool vsync){
                            if (vsync) {SetWindowState(FLAG_VSYNC_HINT);}
                            else {ClearWindowState(FLAG_VSYNC_HINT);}
                        }
                    ),
                    HorizontalLayout( UICSpec{}.SetFlex({1,1}), LayoutSpec{}.AlignEnd(),
                        Slider<int>(&usrCfg.targetFPS, 0, 240, 
                            [](int value){
                                SetTargetFPS(value);
                            }
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget, 2
                        ),
                        ValueLabel<int>("Target FPS: {:0>3}", &usrCfg.targetFPS, Text("", "TNR", fontSize, 0))
                    )
                )
            ),
            Background(
                UICSpec{}.SetFlex({1,1}), BackgroundStyle{.texture = "button_default", .tint = WHITE, .processing = ProcessingValues{.saturation = 0} },
                VerticalLayout( UICSpec{}.SetPadding({20,20,60,60}).SetFlex({1,1}), LayoutSpec{}.AlignCenter().CrossBegin(),
                    Checkbox(Text("Show FPS", "TNR", fontSize, 0), &usrCfg.showFPS,
                        [](bool){},UICSpec{}.SetFlex({1,1})
                    ),
                    HorizontalLayout( UICSpec{}.SetFlex({1,1}), LayoutSpec{}.AlignBegin(),
                        Slider<float>(&usrCfg.userBrightness, 0.1f, 3.0f, 
                            [](float ){}
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget
                        ),
                        ValueLabel<float>("Set Brightness: {:0.3f}", &usrCfg.userBrightness, Text("", "TNR", fontSize, 0))
                    ),
                    HorizontalLayout( UICSpec{}.SetFlex({1,1}), LayoutSpec{}.AlignBegin(),
                        Slider<int>(&usrCfg.activeFontSize, 12,128, 
                            [](int){}
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget, 4
                        ),
                        ValueLabel<int>("FontSize: {}", &usrCfg.activeFontSize, Text("", "TNR", fontSize, 0))
                    )
                )
            ),
            Button(buttonText, [buttonText](){
                GetUIContext().PushPopup(
                    Popup(
                        Button(buttonText,[](){dbg::GetLogger().DebugInfo("Clicked !!!!!!!!!!!!!!!"); dbg::GetLogger().DebugInfo("Popped ", GetUIContext().PopPopup());},"button_default",{200.f,100.f},UICSpec{}),
                        UICSpec{}
                    ).SetAnchor([](){return Rectangle{::GetScreenWidth()/2.0f,::GetScreenHeight()/2.0f,1,1};})
                );
            },"button_default",{200.f,100.f},UICSpec{}
            ),
            Dropdown<WindowMode>(
                {{"Fullscreen", WindowMode::Fullscreen},{"Windowed", WindowMode::Windowed},{"Borderless", WindowMode::Borderless}},
                &GetServices().runtimeCfg.user.windowMode,
                "button_default",
                {200,100},
                [](WindowMode sel){
                    switch(sel){
                    case WindowMode::Fullscreen: //Currently doesn't work on web
                        SetWindowState(FLAG_FULLSCREEN_MODE);
                        ClearWindowState(FLAG_WINDOW_UNDECORATED);
                        break;
                    case WindowMode::Borderless:
                        SetWindowState(FLAG_WINDOW_UNDECORATED);
                        ClearWindowState(FLAG_FULLSCREEN_MODE);
                        break;
                    case WindowMode::Windowed:
                        ClearWindowState(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_UNDECORATED);
                        break;
                    }
                }
            )
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
