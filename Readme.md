## ApEngine
ApEngine is a small library engine/wrapper built around raylib  that implements some additional features like Texture management, retained UI layout, event system, Scene system and some utils to speed up and simplify process of making small games.
- [Overview](#overview)
	- [Features](#features)
	- [Planned](#planned)
- [Install/Build](#installbuild)
	- [Prerequisites](#prerequisites)
	- [Installation process/Quick Start](#installation-processquick-start)
- [User Guide](#user-guide)
	- [Filestructure of Engine](#filestructure-of-engine)
	- [Create scripts](#create-scripts)
	- [Root build scripts](#root-build-scripts)
	- [Project File structure](#project-file-structure)
	- [Customizing build](#customizing-build)
	- [Basic ApEngine app principles.](#basic-apengine-app-principles)
		- [Creating and registering App to Engine](#creating-and-registering-app-to-engine)
		- [Creating Scene](#creating-scene)
		- [Services](#services)
		- [Scene Transitions and lifecycle](#scene-transitions-and-lifecycle)
		- [UI Layout](#ui-layout)
		- [ResourceManager](#resourcemanager)
		- [Renderer](#renderer)
		- [MyInput](#myinput)
		- [UserSettings](#usersettings)
		- [loader:Loader](#loaderloader)
		- [Utils](#utils)
		- [UITheme](#uitheme)
		- [Creating Custom UiComponents](#creating-custom-uicomponents)
--- 
## Overview
### Features:
- Expandable retained mode UI System
- Simple scene system
- Input abstraction level (Work in progress)
- Textures and Fonts manager
- Event system
- Logging system
- Performance measurement system
- Rendering abstractions to simplify shaders and textures use
- Fonts theme system
- Simple animated values
- Web build
- Tracy integration
- Additional utils
- Build scripts
### Planned:
- Audio manager
- Thread pool
--- 
## Install/Build
### Prerequisites
Cmake is the only supported build system as of now. 
You need Cmake >=3.14
C++ Compiler supporting C++ 20 or higher standard
Current supported compilers - (Linux) GCC, (Win) MinGW(ucrt), (Win) MSVC. Clang wasn't tested
Emscripten installed in your PATH is needed for Web build 

---
### Installation process/Quick Start
Currently engine's intended way of installing is by adding it as submodule, but it may be changed in the future.
1.  Create a git repository 
```
git init
git branch -m main
```
or checkout existing one.
2. Add ApEngine as submodule
```
git submodule add https://github.com/APprod/ApEngine.git ApEngine
git submodule update --init
```
3. Commit new submodule, for example:
```
git add ApEngine .gitmodules
git commit -m"Add ApEngine"
```
4. Create a project
	1. (Windows)
	```
	./ApEngine/create.bat <Project name>
	```
	2. (Linux)
	```
	./ApEngine/create.sh <Project name>
	```
	3. If you want automatic CI to build your project in github actions:
	```
	./ApEngine/create.bat --ci
	```
This copies CmakeListsm build, run scripts to your folder. It also creates a new project with basic template in apps/\<Your new project name>
The root CMakeLists and project CMakeLists are automatically created in root and project folders. ^f1f7dc
5. Build the project 
(Mingw)
```
./build.bat static debug --target <Project name>
```
(MSVC)
```
./build.bat static debug msvc --target <Project name>
```
(Linux GCC)
```
./build.sh static debug --target <Project name>
```
The script builds the engine and the project. Result is put into \_build/Debug folder
6. Run the app
FInd the executable in it and run. You should see example Window.
Or run 
```
./run.bat static debug --target <Project name>
```
Or (MSVC) 
```
./run.bat static debug msvc --target <Project name>
```

Continue the development under apps/\<Project name>
7. If you want to see a demo using engine features run:
```
./run.bat static debug <msvc> demo --target demo
```
Demo files are located under ApEngine/examples/demo
8. Create more projects
See step 4 - create project
running create.bat or bat.sh with a new project name appends existing CmakeLists files so no edditional steps needed. Select build target with --target

--- 
## User Guide
### Filestructure of Engine
```
.github // internal CI scripts             
assets // assets distributed with engine
cmake // main function called to create project
engine // engine code
examples // demo app
template // template files used for all new projects
third-party // Raylib/Tracy/Json deps
web // shell for web build
_deps-cache // will be created to cache deps
.gitignore 
CMakeLists // engine level smake
create.bat // project creating scripts
create.sh // project creating scripts
```
### Create scripts
create.bat / create.sh scripts are used to simplify project creation
Create script when invoked the first time copies following files
```
template/build.bat
template/run.bat
template/build.sh
template/CMakeLists.txt
template/.gitignore
``` 
to project root.
Copies template/app/* into apps/\<provided name>
Adds new project to copied root Cmake 
```
add_subdirectory(apps/<ProjectName>)
```
Project-level Cmake (aka \*ProjectName/CmakeLists.txt) automatically sets its project name to /\<ProjectName> (name of its folder)

---
Create scripts support --ci flag
When --ci provided standalone (without ProjectName provided) or with project name provided it copies premade CI for github actions from ApEngine/templates/.github
CI executes:
Triggered on **dev** branch:
- mingw windows build
- msvc windows build
- gcc linux
- web build (compiled on linux)
Uploads created executables as artifacts with 1 day retention. All builds are Debug builds
**Manually dispatched scripts**:
- build.yml - Builds release configuration, creates release (prerelease, draft set to true) when dispatched from v* tag
- msvc-debug.yml - builds projects with msvc and **Tracy** enabled, you can select debug/release, includes pdb. Main purpose - create a build that can be used with Tracy
---
### Root build scripts
Lets u build and run project with one line call.
usage:
```
./build.bat(sh) [debug|release|static|dynamic|web|msvc|profile|demo] --target <app_name> 
```
- All flags are optional.
- Default config - static debug, non msvc, builds all projects, doesn't build demo.
- Parameters order doesnt matter. `debug static msvc` is same as `static msvc debug`
- --target flag builds Engine + specified project
- static/dynamic applies to CRT (C++ runtime linking) only.
- web switches to web build, needs emscripten installed on the system
- profile enables Tracy
- demo builds examples/demo in addition
- msvc for msvc build, to enable msvc flags instead of gcc ones
./run.bat runs ./build.bat under the hood + runs the built exe/ linux executable, or starts emrun.
All dependencies (Raylib, nlohmann::json, Tracy) are automatically installed into ApEngine/third-party folder and are exposed to user.

Provide -DSHELL_PATH=<path to shell file>
if you want to use custom shell
---
### Project File structure
Project created with create script has following structure by default:
```
assets //     folder for your assets
include //    folder for includers (hpp/h)
src //        folder for source (cpp) files
CMakeLists // project level cmake
include/mainApp.hpp
src/mainApp.cpp
```
Cmake handles getting project name from folder name and uses engine/cmake/ function to handle all necessary for build code.
Everything else is up to user.
assets folder will be copied near final binary so everything should be put here. It is also copied as well as engine level assets, so no need to add engine assets to it , or vice versa, both are merged.

---
### Customizing build
If you want to customize build scripts for project, copy contents from ApEngine/cmake/add_ap_project.cmake, set ${target} variable to your project name, you can add whatever other changes you need then.
Or you can change build.bat\/sh\/run if it is needed

---
### Basic ApEngine app principles.
This part covers basic architecture and principles of programm using ApEngine
#### Creating and registering App to Engine
`#include "app.hpp"`
Entry point to your code is function 
`std::unique_ptr<IApp> createApp();`
It is defined in Engine but not implemented. Implement the function to provide your class as following, for example:
```
std::unique_ptr<IApp> createApp(){

    return std::make_unique<App>();

}
```
Functions of IApp will be called in their definition order and can be used if you need some initialization or loading before the first scene.

---
#### Creating Scene
`#include "scene/scene.hpp"` or 
`#include "scene/baseScene.hpp"`
IApp should implement createScene to return your first Scene. Example:
```
std::unique_ptr<IScene> createScene() override {
	return std::make_unique<HelloScene>();
};
```
To create a scene Derive from IScene or baseScene (Recommended).
IScene has lifecycle functions OnEnter, OnReStore, OnExit, OnSuspend, AimateEnter/Exit. And main functions: OnUpdate, OnDraw.
Implement your game logic in OnUpdate and game drawing in OnDraw.
Better approach is to use baseScene, which automatically handles resizing, animation, input polling. 
baseScene contains Root object which is a root of UI tree
Implement OnUpdateState with your game logic and OnDrawContent with screen content. 
Add your UI elements to the UI tree and they will be updated automatically.
Call `root.OnDraw();` to draw the UI.
Use override OnDrawToScreen for custom post processing. (OnDrawContent is rendered to a RenderTarget that can be used for post-processing).

---
#### Services
`#include "services.hpp"`
Services is a global singleton containing Engine services. Main place to interact with the Engine.
Contents:
- [SceneManager](scene-transitions-and-lifecycle)
- [ResourceManager](resourcemanager)
- [Input manager (MyInput)](myinput)
- [Renderer](renderer)
- loader::Loader
- PerformanceLog
- [UITheme](uitheme)
- [UserSettings](usersettings)
- DebugFlags
call `GetServices();`
---
#### Scene Transitions and lifecycle
`#include "services.hpp"` (to get SceneManager)
SceneManager in Services handles scenes Stack and transitions.
Mainloop is handled by engine. For each frame or step Engine calls OnUpdate() and OnDraw() for the current scene in Scene stack.
Scene lifecycle:
- When the scene is first created `OnEnter` is called.
- `OnUpdate` `OnDraw` called in mainloop
- OnExit is called before Scene transitions to another and is deleted from the stack
- OnSuspend called when Scene transitions to another, but is kept in scene stack.
- OnRestore is called when the scene was previously suspended and returned to via Pop.
Animation:
- Animate enter/exit called each frame when transition is requested until it returns true;
**SceneManager functions**
In order to transit between scenes SceneManager has functions
- QueTransit # current scene will be deleted from stack
- QueTransitSus # suspend + transit, current scene kept in stack
- QuePop # return to previous scene in scene stack, current is deleted
If Transition is called before the last one was performed it will be ignored
---
#### UI Layout
`#include "ui/component.hpp"
`#include "ui/layout.hpp"` and others in ui/*
If you are using baseScene
Compose UI layout by adding UIComponents to a Root like this:
```
root.AddChild(std::make_unique<Label>(Text("Hello, ApEngine!", "default")));
```
There are three types of Components:
1. Layouts:
	1. Root - handles UI tree
	2. HorizontalLayout/VerticalLayout/HorizontalScrollView/VerticalScrollView - layouts arranging contents in specific axis
	3. Stack - stacks content above each other at the same space.
	Layouts store other UiComponents (can store other Layouts too), those can be added with `Add`, or `AddChild`. 
2. Widgets
	1. Button
	2. checkbox
	3. debugOverlay
	4. HotkeysListener
	5. label
	6. overlay
	7. slider
	8. textInputField
	9. valueLabel
	10. dropdown
3.  Modifiers - Modifier, Popup - contains only one child
Base class for components - UIComponent
Specify fill mode, padding, flex, min/max size for any component with UIComponentSpec passed into constructor.
Base class for layout - Layout, you can pass LayoutSpec to specify content arrangement, and UIComponentSpec to specify Layout component itself to any Layout component.
Once Component added to UI tree ( aka added to  Root or another layout, that is held by some amount of other layouts that eventually reach root) it will automatically recieve OnUpdate,  OnDraw, OnEvent calls. Arrangement and resizing is handled automatically.

---
#### ResourceManager
`#include "services.hpp"`
Handles texture, font, shader loading.
All Resources stored through string aliases. Texture loaded under the name "texture123" can be retrieved with getTexture("texture123")
Use loadTexture to load a texture if needed before usage.
GetTexture returns already loaded texture, or Lazy-loads the texture if it exists under assets/\<name>
Textures should be unloaded unloadTexture if not used anymore.
Use loadFont/getFont and loadShader/getShaderProgram for fonts and Shaders respectively.
```
std::optional<SliceMargins> getSliceData(const std::string& name) const;
void loadNPatchData(std::string path);
```
are used to load and use 9-Patch data for textures that want to use it.

---
#### Renderer
Helper class that keeps stack of Texture targets, shader and blending modes.
It is Recommended to use its functions
    void beginTextureMode(RenderTexture2D& target);
    void endTextureMode();
    void beginShaderMode(const std::string& name, const Uniforms& uniforms); 
    void beginShaderMode(const ShaderProgram& program, const Uniforms& uniforms); 
    void beginShaderMode(const ShaderInput& input);
    void endShaderMode();
    void beginBlendMode(BlendMode mode);
    void endBlendMode();
instead of Raylib onse because it allows to have multiple nested Render targets and custom UI Components to have their own render targets if needed.

---
#### MyInput
``#include "services.hpp"``
Abstracts input from differnet devices Creates list of events. Should be polled each fram, which is done by baseScene. All event are disptched by baseScene automatically.
All events are presented in ``#include "input/events.hpp"``
#### UserSettings 
``#include "services.hpp"`` or
`#include "userSettings.hpp"`
userSettings are used at startup and can be a place where current state of Screen and app stored.
#### loader::Loader
provides functions to parse data from json.
#### Utils.
`#include "utils/util.hpp"` Provides overriden math operations for Ralibprimitives abnd a few other helpers. 
`#include "utils/util.hpp"` Adds logging to the programm.
use GetLogger().Info/Warn, etc. or .WarnFmt .. etc. for std::format formatting. Add your custom Sink with AddSink if you need to log to more places. Supports any data type with ostream << overriden.
`#include "utils/animated.hpp"` Adds Animated\<T> class that animates a value and some math animation functions.

---
#### UITheme
`#include "services.hpp"``
Handles fonts in the app. allows to use font roles (eg. MainFont, AdditionalFont, etc). and size roles (eg. caption, text, header, etc). 
Each are stored as string keys and retrieved with specified size role and font role keys.
Allows to registed font roles and size roles as well as change them. 
#### Creating Custom UiComponents
There are helper classes for creating new Components. And you can always derive from already existing one, override the logic, or add new one on top of existing.
All components  have following:
- OnUpdate - if it returns true, triggers rearrange of whole Ui tree.
- OnEvent - returns true if Event is handled and shouldn propagate
- OnDrawContent() -drawing content
- MeasureContent - sets m_contentDesiredSize - amount of size components wants
- ArrangeContent - arrange content inside the component if needed. recieved Rectangle is only awailable space for component contents
- HitTest - checks when point is over component
- FindTarget - Continues the search for component under the poing, returns if out of component bounds.
- getCaptureTypes - return a mask with all types that this component wants to recieve before others when it is capturing (example - click/move/unpress for a button)
- ResolvePadding - allows programmatically increase padding if needed.
- Padding is automatically handled when OnMeasure and OnArrange called
- OnHoverEnter OnHoverExit will be called by Root when component is being hovered over
Internally available (protected):
- GetActualRect returns rect of position of component on screen including scroll and other types of offsets
- GetVisualWithoutOffset - returns arranged rectabgles with padding substracted, not includes offsets
- GetVisualRect returns actual place of content on screen. with padding and offset handled

**Capturing**
If component needs to recieve events before other components, use Capturing;
Call UIContext().SetCapture(this) to capture (ReleaseCapture - release).  Override getCaptureTypes to return types you want to capture.
use UIContext().PushPopup(Pupup(...)) if you need a component above the UI tree.
**Helpers**
Some of the helper classes:
- Text - handles text storing, wrapping, drawing
	- use  auto textSize = m_text.ReMeasure(available); to measure new/current text
	- m_text.IsDirty() for text change check
	- m_text.DrawCentered/Draw for drawing
- TextureSpec - Adds 9-Patch rendering if SliceMargins are set, and supports drawing with specified shader automatically.
- Clickable (UIComponent) - handles cleick/release/hold logic
- Modifer - handles ine child
- Scrollable - handles scrolling data