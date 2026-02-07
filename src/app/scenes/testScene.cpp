#include "app/scenes/testScene.hpp"
#include "app/scenes/customUIComps.hpp"
#include "core/util.hpp"

TestScene::TestScene(RecourceManager& manager): m_manager{manager}
{}

void TestScene::OnEnter(){
    auto column= std::make_unique<VerticalLayout>(UIComponentSpec{},LayoutSpec{Alignment::End});
    for(int i = 0; i < 1; i++){
        auto row = std::make_unique<HorizontalLayout>(
                    UIComponentSpec{FillMode::FillMaxWidth},
                    LayoutSpec{Alignment::Center});
        for(int j = 0; j < 5; j++){
            row->AddChild(std::make_unique<Button>(
                Button("Test",
                    [j](){ dbg::GetLogger().Info("Button"+ std::to_string(j) +" clicked!"); },
                "button_default",{300,100})
            ));
        }
        column->AddChild(std::move(row));    
    }
    root.AddChild(std::move(column));
    OnResize();
}

void TestScene::OnRestore(){}

void TestScene::OnUpdate(){
    root.OnUpdate();
    if (::IsWindowResized()){
        OnResize();
    }
}

void TestScene::OnResize(){
    Tester tester("OnResize",0,true);
    Vector2 dims = {
        static_cast<float>(::GetScreenWidth()),
        static_cast<float>(::GetScreenHeight())
    };
    Rectangle rectangle = rect({0,0}, dims);

    root.OnMeasure(dims);
    root.OnArrange(rectangle);
}
void TestScene::OnDraw(){
    BeginDrawing();
        ClearBackground(BLACK); 
        
        DrawTexture(m_manager.getTexture("menu"), 0,0,RAYWHITE);
        root.OnDraw();
        DrawFPS(20, 20);
    EndDrawing();  
}

void TestScene::OnExit(){}

void TestScene::OnSuspend(){}