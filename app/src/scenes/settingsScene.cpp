#include "scenes/settingsScene.hpp"
#include "ui.hpp"
#include "ui/dropdown.hpp"
#include "userSettings.hpp"

void SettingsScene::OnEnter(){
    m_transitionTime = 0.2f;    
    UICSpec sliderSpec;
    auto [sBarH, sMinTh, sMaxTh, sTarget] = std::make_tuple(4.0f, 5.0f, 10.0f, Vector2{200,10});
    auto& usrCfg = GetServices().runtimeCfg.user;
    m_pendingSizeIndex = usrCfg.theme.m_currentSizeIndex;
    m_pendingFontName = usrCfg.theme.m_fontName;

    auto fonts = GetServices().resManager.getLoadedFonts();
    std::vector<std::pair<std::string, std::string>> fontItems;
    for (auto& f : fonts) fontItems.emplace_back(f, f);
    auto row = std::make_unique<HorizontalLayout>(
        UICSpec{}.SetPaddingPct({.top = 10.0, .bottom =  10.0, .left = 10.0, .right = 10.0}).SetFlex({0,1}),
        LayoutSpec{}.AlignEnd().CrossBegin(),
        VerticalLayout( UICSpec{}.SetFlex({0,1}).FillMaxHeight(), LayoutSpec{}.AlignCenter().CrossEnd().UniformCross(),
            
            Background(
                UICSpec{}.SetFlex({0,1}), 
                TextureSpec("button_default").Shader(ProcessingSh().saturation(0.0f)),
                VerticalLayout( UICSpec{}.SetPadding({20,20,60,60}).SetFlex({1,1}), LayoutSpec{}.AlignCenter().CrossBegin(),
                    Checkbox(Text("VSYNC", "text"), &usrCfg.vsync,
                        [](bool vsync){
                            if (vsync) {SetWindowState(FLAG_VSYNC_HINT);}
                            else {ClearWindowState(FLAG_VSYNC_HINT);}
                        }
                    ),
                    HorizontalLayout( UICSpec{}.SetFlex({1,0}), LayoutSpec{}.AlignEnd(),
                        Slider<int>(&usrCfg.targetFPS, 0, 240, 
                            [](int value){
                                SetTargetFPS(value);
                            }
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget, 2
                        ),
                        ValueLabel<int>("Target FPS: {:0>3}", &usrCfg.targetFPS, Text("", "text"))
                    )
                )
            ),
            Background(
                UICSpec{}.SetFlex({0,1}), TextureSpec("button_default").Shader(ProcessingSh().saturation(0.0f)),
                VerticalLayout( UICSpec{}.SetPadding({20,20,60,60}).SetFlex({1,1}), LayoutSpec{}.AlignCenter().CrossBegin(),
                    Checkbox(Text("Show FPS", "text"), &usrCfg.showFPS,
                        [](bool){},UICSpec{}
                    ),
                    HorizontalLayout( UICSpec{}.SetFlex({1,0}), LayoutSpec{}.AlignBegin(),
                        Slider<float>(&usrCfg.userBrightness, 0.1f, 3.0f, 
                            [](float ){}
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget,{},usrCfg.userBrightness
                        ),
                        ValueLabel<float>("Set Brightness: {:0.3f}", &usrCfg.userBrightness, Text("", "text"))
                    ),
                    ValueLabel<int>("Font size Current: {}", &usrCfg.theme.m_currentSizeIndex, Text("", "text")),
                    HorizontalLayout( UICSpec{}.SetFlex({1,0}), LayoutSpec{}.AlignBegin(),
                        Slider<int>(&m_pendingSizeIndex, 0, 21,
                            [](int){}
                            , UICSpec{}.SetFlex({0,1}).MinSize({50,0}), sBarH, sMinTh, sMaxTh, sTarget, 1
                        ),
                        ValueLabel<int>("Font size index: {}", &m_pendingSizeIndex, Text("", "text"))
                    )
                )
            ),
            Label(
                Text("Select Font:", "text"),
                UICSpec{},TextAlign::Center
            ),
            Dropdown<std::string>(
                fontItems, &m_pendingFontName,
                TextureSpec("button_default"), {200,100}, nullptr,
                Text("Font:", "button"),
                UICSpec{}.SetFlex({0,1})
            ),
            Label(
                Text("Window mode:", "text"),
                UICSpec{},TextAlign::Center
            ),
            Dropdown<WindowMode>(
                {{"Fullscreen", WindowMode::Fullscreen},{"Windowed", WindowMode::Windowed},{"Borderless", WindowMode::Borderless}},
                &GetServices().runtimeCfg.user.windowMode,
                TextureSpec("button_default"),
                {200,100},
                [](WindowMode sel){
                    switch(sel){
                    case WindowMode::Fullscreen:
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
                },
                Text{"","button"},
                UICSpec{}.SetFlex({0,1})
            ),
            Button(Text("Apply Settings", "button"),
                [this](){
                    auto& theme = GetServices().runtimeCfg.user.theme;
                    theme.m_currentSizeIndex = m_pendingSizeIndex;
                    theme.m_fontName = m_pendingFontName;
                },
                TextureSpec("button_default"), {200,100},
                UICSpec{}.SetFlex({0,1})
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
    auto& manager = GetServices().resManager;
    ::DrawTexture(manager.getTexture("menu"), 0, 0, RAYWHITE);
    root.OnDraw();
    ::BaseScene::DrawFadeTransition();
}

void SettingsScene::OnUpdateState(){
    
}
