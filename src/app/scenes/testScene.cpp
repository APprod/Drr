#include "app/scenes/testScene.hpp"
#include "app/scenes/customUIComps.hpp"
#include "core/util.hpp"

TestScene::TestScene(RecourceManager& manager): m_manager{manager}
{}

void TestScene::OnEnter(){
    auto column= std::make_unique<VerticalLayout>(UIComponentSpec{},LayoutSpec{Alignment::End});
    for(int i = 0; i < 1; i++){
        auto row = std::make_unique<HorizontalLayout>(
                    UIComponentSpec{FillMode::FillMaxWidth}.Padding({.left = 50, .right = 50}),
                    LayoutSpec{Alignment::Center, Alignment::Center,JustifyContent::SpaceEvenly});
        for(int j = 0; j < 5; j++){
            row->Add(
                Button("Test",
                    [j](){ dbg::GetLogger().Info("Button"+ std::to_string(j) +" clicked!"); },
                    "button_default",{(float)300,(float)200}, 
                    UICSpec().SetFlex({.growth = 0, .shrink = 1}).
                    MinSize({100,50}))
            );
        }
        column->AddChild(std::move(row));    
    }
    root.AddChild(std::move(column));
    OnResize();
}

void TestScene::OnUpdateState(){
}

void TestScene::OnResize(){
    Tester tester("OnResize",0,true);
    Vector2 dims = {
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    MyRectangle rectangle = {{0,0}, dims};
    dbg::GetLogger().LogFormat(dbg::Severity::ERROR, "hey this: {}", dims.x);

    root.OnMeasure(dims);
    root.OnArrange(rectangle);
}
void TestScene::OnDrawContent(){
    PerfTester tester = GetServices().perfLog.log("OnDrawContent");
    DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
    root.OnDraw();
}
