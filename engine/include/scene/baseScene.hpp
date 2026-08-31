#pragma once
#include "scene/scene.hpp"
#include "utils/log.hpp"
#include "ui/root.hpp"
#include "utils/animated.hpp"
#include "services.hpp"

// Simple Scene implementation that adds additional functionality out of the box:
// Drawing piplene with OnDrawContent to override with your content rendered into separate render target
// Basic post processing (changing brightness)/ Can be overriden
// automatic Input and events dispatch to UI layout
// Auto Resising and relayout handling
// Entering and Exiting automatic value animation 
class BaseScene: public IScene{
protected:
    // Should be overriden with additional content from user (except of the UI layout)
    virtual void OnDrawContent() = 0;
    // Upadate state of objects besides ones in UI tree
    virtual void OnUpdateState() = 0;
public:
    BaseScene(){m_renderTarget = ::LoadRenderTexture(::GetScreenWidth(),::GetScreenHeight());}
    virtual ~BaseScene(){::UnloadRenderTexture(m_renderTarget);}
    void OnRestore() override {};
    void OnSuspend() override {};
    void OnExit() override {};

    void OnDraw() override {
        BeginDrawing();
        GetServices().renderer.beginTextureMode(m_renderTarget);
        ClearBackground(BLACK);
        OnDrawContent();
        GetServices().renderer.endTextureMode();
        OnDrawToScreen();
        EndDrawing();
    };
    //Use for post processing override for your custom
    virtual void OnDrawToScreen() {
        auto drawCall = [this](){
            ::DrawTextureRec(m_renderTarget.texture,
            {0, 0, static_cast<float>(m_renderTarget.texture.width), static_cast<float>(-m_renderTarget.texture.height)},
            {0, 0}, WHITE);
        };
        auto& usr = GetServices().userSettings;
        useShaderUnchecked(
            "processing",
            {{"brightness", usr.userBrightness},
            },
            drawCall
        );
        
    }
    // Automatic input, events, resizing handling 
    void OnUpdate(float dt) override {
        PerfTester tester = GetServices().perfLog.log("Scene OnUpdate Full");
        Tester perfProblemCheck("Scene OnUpdate", 20, true);
        auto& input = GetServices().input;
        {
            PerfTester innerTester = GetServices().perfLog.log("Scene input.pollEvents");
            input.pollEvents();
        }
        const auto& events = input.getEvents();
        for (auto& event: events){
            root.OnEvent(event); //updates states
        }
        bool needsResize = root.OnUpdate(dt); //updates other data related to UI states
        if (needsResize || ::IsWindowResized()) { //resize if needed
            OnResize();
            
        }
        OnUpdateState(); //updates scene data not UI
    };
    
    ///Handles automatic animation of transitionProgress can be overriden for custom behaviour
    virtual bool AnimateEnter(float dt) override {
        if (m_transitionTime == 0.0f) return true;
        if (m_transitionProggress.isDone()) {
            m_transitionProggress.reset(1.0f, m_transitionTime, Easing::easeOutCubic);
            m_transitionProggress.setTarget(0.0f);
        }
        m_transitionProggress.update(dt);
        return m_transitionProggress.isDone();
    } 
    virtual bool AnimateExit(float dt) override {
        if (m_transitionTime == 0.0f) return true;
        if (m_transitionProggress.isDone()) {
            m_transitionProggress.reset(0.0f, m_transitionTime, Easing::easeInCubic);
            m_transitionProggress.setTarget(1.0f);
        }
        m_transitionProggress.update(dt);
        return m_transitionProggress.isDone();
    }
    //Draws basic fade in/out based on m_transitionProggress
    virtual void DrawFadeTransition() {
        if (!m_transitionProggress.isDone()){
            ::DrawRectangle(0,0,::GetScreenWidth(),::GetScreenHeight(),{0,0,0,static_cast<uint8_t>(m_transitionProggress * 255)});
        }
    }
protected:
    virtual void OnResize(){
        mylog::GetLogger().DebugInfo("Resized: ", Vector2{::GetScreenWidth()/1.0f, ::GetScreenHeight()/1.0f});
        PerfTester tester = GetServices().perfLog.log("OnResize");
        auto dims = Vector2{
            static_cast<float>(::GetScreenWidth()),
            static_cast<float>(::GetScreenHeight())
        };
        MyRectangle rect = {{0,0}, dims};
        root.OnMeasure(dims);
        root.OnArrange(rect);
        Ivec2 newSize = {GetScreenWidth(),GetScreenHeight()};
        if (lastSize!= newSize){
            ::UnloadRenderTexture(m_renderTarget);
            m_renderTarget = ::LoadRenderTexture(newSize.x,newSize.y);
            lastSize = newSize;
        }
    }
    
    Root root;
    Animated<float> m_transitionProggress{0.0f, 0.0f, Easing::easeInOutCubic};
    RenderTexture2D m_renderTarget;
    Ivec2 lastSize;
    float m_transitionTime{0}; //Set this to 0 to have immediate transitions
};
