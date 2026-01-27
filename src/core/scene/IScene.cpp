#include <memory>
#include <vector>

class ILayer{

};

class IScene{ //Base class for Scene
public:
    IScene(){}
    virtual ~IScene() = delete; 

    virtual void OnEnter() = delete; //Allocate everything


    virtual void OnUpdate() = delete;
    virtual void OnDraw() = delete; 

    virtual void OnExit() = delete; //DeAllocate
    virtual void OnSuspend() = delete;  // called on transit
private:
    std::vector<ILayer> m_layers;
    bool m_suspended;
};

class SceneManager{ //will be a part of Services so acessible everywhere

};

// menu <-> main(can be suspended)/awaiting result -> btl

// menu(transits beetween layers), menu -> SceneManeger.Transit(MainScene, suspend = false)
// SceneManager holds scenes stack
// Transit ques swap // disables next frame input// instead call OnExit/OnSuspend on first and OnEnter for second
// 
// Layers
// scene holds Layers stack
// Layer can transit his position in stack to any other layer, it is suspend by default, probably no need fo others. Layers allocated/deallocated on creation of scene
// Probaly each scene have layerManager so you get layerManager.get("layer1").transit(NewLayer) // this destroys layer 1 and creates layer 2
// 
// layer has Transit(unique_pointer<ILayer> layer)
// it should queue transition to manager -> layer holds manager pointer? 
// manager ques by getting layer that transits and layer to transit to.  So we can get layerManager.transit(this, NewLayer)
// inside: find this, -> idx, m_layer[idx] = NewLayer (unique pointer) but layer should register stuff in constructor, unregister in destructor.
// if it is suspend, just change a bool and do not execute
// manager should manually go through layers and call update, draw 
// 