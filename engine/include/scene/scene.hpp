#pragma once

#include "utils/log.hpp"
#include "ui/root.hpp"
#include "utils/util.hpp"
#include "utils/animated.hpp"

class IScene{ //Base class for Scene
public:
    virtual ~IScene() = default; 

    virtual void OnEnter() = 0; //Allocate everything
    virtual void OnRestore() = 0; //Called only if it was Suspended

    virtual void OnUpdate(float dt) = 0;
    virtual void OnDraw() = 0; 

    // returns  true if animation finished
    virtual bool AnimateEnter(float ) { return true; } 
    virtual bool AnimateExit(float )  { return true; }

    virtual void OnExit() = 0; //DeAllocate, called on normal transit
    virtual void OnSuspend() = 0;  // called on suspended transit
private:
};

class BaseScene: public IScene{
public:
    void OnRestore() override {};
    void OnSuspend() override {};
    void OnExit() override {};

    void OnDraw() override {
        BeginDrawing();
        ClearBackground(BLACK);
        OnDrawContent();
        EndDrawing();
    };
    void OnUpdate(float dt) override {
        PerfTester tester = GetServices().perfLog.log("Scene OnUpdate Full");
        Tester perfProblemCheck("Scene OnUpdate", 20, true);
        auto& input = GetServices().input;
        {
            PerfTester innerTester = GetServices().perfLog.log("Scene input.pollEvents");
            input.pollEvents();
        }
        auto events = input.getEvents();
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
    }
    virtual void OnDrawContent() = 0;
    virtual void OnUpdateState() = 0;
    Root root;
    Animated<float> m_transitionProggress{0.0f, 0.0f, Easing::easeInOutCubic};
    float m_transitionTime{0}; //Set this to 0 to have zero transition time
};
