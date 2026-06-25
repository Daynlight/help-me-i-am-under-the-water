<div align=center>

# Help Me I'm Under The Water

[![wakatime](https://wakatime.com/badge/user/233b40bd-5512-4e3e-9573-916f7b4127c3/project/408b7767-840b-45a2-b96f-d4c1d6a11ab1.svg)](https://wakatime.com/badge/user/233b40bd-5512-4e3e-9573-916f7b4127c3/project/408b7767-840b-45a2-b96f-d4c1d6a11ab1)

</div>



## Demos

<table cellspacing="15" cellpadding="0" style="width: 100%; border: none;">
  <tr>
    <td width="50%">
      <img width="100%" src="./docs/Prod.gif">
    </td>
    <td width="50%">
      <img width="100%" src="./docs/Editor.gif">
    </td>
  </tr>
  <tr>
    <td width="50%">
      <img width="100%" src="./docs/Movement.gif">
    </td>
    <td width="50%">
      <img width="100%" src="./docs/SDF.gif">
    </td>
  </tr>
</table>



## About
Project for **GRK** under water scene. Build on top of my library [**CWindow**](https://github.com/Daynlight/CWindow.git). Uses **ImGui** build in **CWindow** for parameters adjustment and visualization. Focused on full compilation to **single executable** and **Game Engine like editing**. With handcrafted assets.



## TOC
- [Demos](#demos)
- [About](#about)
- [TOC](#toc)
- [Methods](#methods)
- [Controls](#controls)
- [UI](#ui)
- [Installation and Usage](#installation-and-usage)
- [Compiling End Product](#compiling-end-product)
- [Architecture](#architecture)
  - [Build System](#build-system)
  - [DataSerializer {SINGLETON}](#dataserializer-singleton)
  - [Resources {SINGLETON}](#resources-singleton)
  - [ObjectManager {SINGLETON}](#objectmanager-singleton)
  - [Logger {SINGLETON}](#logger-singleton)
  - [GameObject](#gameobject)
  - [UI](#ui-1)
  - [Script Controller and ScriptShared](#script-controller-and-scriptshared)
- [Tasks Presentation](#tasks-presentation)
  - [Normal mapping](#normal-mapping)
  - [PBR](#pbr)
  - [Quaternion camera](#quaternion-camera)
  - [Shadow mapping](#shadow-mapping)
  - [Parallel Transport Frames](#parallel-transport-frames)
  - [Underwater skybox](#underwater-skybox)
  - [**A09** Ray-marched SDF object](#a09-ray-marched-sdf-object)
  - [**B07** Heightmap-based seabed mesh](#b07-heightmap-based-seabed-mesh)
- [Writing scripts](#writing-scripts)
- [Supported Platforms](#supported-platforms)
- [Prerequisites](#prerequisites)
- [License](#license)
- [Author](#author)
- [TODO](#todo)



## Methods
- [x] normal mapping 
- [x] PBR 
- [x] quaternion camera
- [x] shadow mapping
- [ ] Parallel Transport Frames 
- [x] underwater skybox
- [x] **A09** Ray-marched SDF object
- [x] **B07** Heightmap-based seabed mesh



## Controls
* **SWAP_CAMERA_BTN**: P (Editor)
* **CAMERA_FOCUS_MODE_BTN**: ESC (Editor)
* **CAMERA_ACCELERATE**: O (Editor, Product)
* **CAMERA_DECELERATE**: I (Editor, Product)
* **CAMERA_MOVE_FORWARD**: W (Editor, Product)
* **CAMERA_MOVE_BACK**: S (Editor, Product)
* **CAMERA_MOVE_RIGHT**: D (Editor, Product)
* **CAMERA_MOVE_LEFT**: A (Editor, Product)
* **CAMERA_ROLL_LEFT**: Q (Editor, Product)
* **CAMERA_ROLL_RIGHT**: E (Editor, Product)  
Can be edited in [config.h](UnderTheWater/config.h) 



## UI
* **Info**
* **Logs**
* **Materials Explorer**
* **Materials Editor**
* **Lights Editor**
* **Objects Explorer**
* **Objects Editor**
* **Asset Loader**
* **Shader Explorer**
* **Shader Editor**
* **Script Explorer**
* **Script Editor**  
All in **Editor** mode. **Production** have turn off ui.



## Installation and Usage
1. Install from [**Releases**](https://github.com/Daynlight/help-me-i-am-under-the-water/releases)
2. Compile by yourself
  1. Install required programs
     - [**Cmake**](https://cmake.org/download/)
     - [**gcc**](https://gcc.gnu.org/) or via [**MySYS2**](https://www.msys2.org/)
     - [**Ninja**](https://ninja-build.org/)
  2. Clone repository
      ```bash
      git clone https://github.com/Daynlight/help-me-i-am-under-the-water
      ```
  3. Update Submodules
      ```bash
      git submodule update --init --recursive
      ```
  4. Compile with **CMake**
      ```bash
      mkdir -p build/
      cd build/
      cmake -B ..
      cmake --build .
      ```
  5. Run program
      ```bash
      ./UnderTheWater/UnderTheWater
      ```



## Compiling End Product
Use cmake command with ```PRODUCTION``` FLAG

```bash
mkdir -p build-prod
cd build-prod
cmake -B . -DPRODUCTION=ON -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ..
./build-prod/bin/UnderTheWater
```



## Architecture
### Build System
We use **cmake** for ease of build with **git submodules** for git packages.

### [DataSerializer](UnderTheWater/DataSerializer/) {SINGLETON}
Focuses only on reading and saving. For loading data uses ```cmrc``` that **bakes** assets into executable. Data are stored in ```binary``` format for faster access and avoiding parsing. Everything that is baked via ```cmrc``` is save in [GameData](GameData/) folder. 

* **Meshes**: Faster to read then **Assimp**. Saved as multiple files each for one  mesh in [Assets/Meshes](GameData/Assets//Meshes/) folder each with ```.msh``` extension. On load we search for file in this directory with ```.msh``` extension. Loaded to [```Resources```](UnderTheWater/Resources/) saved in ```UW::Meshes``` that controls versioning and allows avoiding ```unordered_map``` for editor.
* **Shaders**: Accessible via ```Resources```. Saved as multiple files in [Assets/Shaders](GameData/Assets/Shaders/) folders **each folder is one compiled shader** with ```.glsl``` extension. List of allowed script types is in [config.h](UnderTheWater/config.h).
* **Scripts**: In DataSerializer saves and loads ```.cpp``` script to ```UI``` editor. Logic of ```hot-reloading``` and compiling to ```PRODUCTION``` is in [ScriptController](UnderTheWater/ScriptController/) and [ScriptShared](UnderTheWater/ScriptShared/).
* **Lights**: World static lights loaded to ```Resources``` and compile as ```SSBO```. Single file [```Lights.lit```](GameData/Lights.lit).
* **Materials**: World static materials loaded to ```Resources``` and compile as ```SSBO```. Single file [```Materials.pbr```](GameData/Materials.pbr).
* **Objects**:  World objects loaded to ```ObjectManager``` singleton class. Single file [```Objects.obj```](GameData/Objects.obj).
* **Textures**: Textures are a bit different they are accessible in ```Resources``` by ```getTexture()``` after first time loading they are stored in ```unordered_map```.

### [Resources](UnderTheWater/Resources/) {SINGLETON}
Are one point where we are storing [```Meshes```, ```Textures```, ```Shaders```, ```Materials```, ```Lights```]. For now all are loaded and store to end of app life time but in future we will manage them wisely. Probably base on ```calls count``` and free ```VRAM```.

### [ObjectManager](UnderTheWater/Objects/ObjectManager.h) {SINGLETON}
One point where we stores all ```GameObjects```. Inherit from Interface [IObjectManager](UnderTheWater/ScriptShared/IObjectManger.h) for bridging between script and main app. Allows creation, deletion, and getting [GameObjectData](UnderTheWater/ScriptShared/GameObjectData.h) via name.   
**![ Script access to object manager is not full working for now ]!**

### [Logger](UnderTheWater/Utils/Logger.cpp) {SINGLETON}
Is used for logging [```info```, ```warn```, ```erro```]. Logs are saved in ```editor.log``` up to 10000 lines. Can be changed in [```config```](UnderTheWater/config.h). Mainly for debugging detecting errors and work monitoring. Can be used to report bugs. Is inherit by [ILogger](UnderTheWater/ScriptShared/ILogger.h) interface to bridge between script and main app.

#### [App](UnderTheWater/App.h) and [Scene](UnderTheWater/Scene.h)
App is main entry where we initialize scene and ui for editor. Scene is full rendering pipeline with creation of ```main framebuffer```, ```shadows framebuffer``` and applying ```post-processing```. In future we plan adding ```Scene Controller``` and swapping between scenes.

### [GameObject](UnderTheWater/Objects/GameObject.h)
Main on scene object that contains ```GameObjectData``` and list of ```scripts```. [GameObjectData](UnderTheWater/ScriptShared/GameObjectData.h) contains [```position```, ```rotation```, ```scale```, ```name```, ```mesh```, ```shader```, ```vector of textures```, ```vector of materials```, ```map of parameters```]. We use separated struct [GameObjectData](UnderTheWater/ScriptShared/GameObjectData.h) for bridging between scripts and main app that allows scripts to work on objects. Parameters are variants of predefined types that are easy to edit in editor mode and accessible in script.  
**![ For now when scripts are bind we can't edit object in editor for editing parameters we turn off temporary scripts ]!**  
**![ Some times we have to remove script in UI to full restart them ]!**

### [UI](UnderTheWater/UI/)
UI provides easy editor interface to building scene adding and managing [```objects```, ```materials```, ```lights```, ...]. In Production Mode it is turn off.

* **Info**: Basic info about [```fps```, ```selected camera```, ...]. Buttons turn turn on/off [```Mesh Mode```, ```Terrain```, ```Post-Processing```, ...].  
* **Logs**: Allows monitoring operations and changes.
* **Materials Explorer**: Selecting, Removing, Adding Materials.
* **Materials Editor**: Editing PBR parameters for current chosen Material.
* **Lights Editor**: Adding, Removing Lights. Editing ```Color```, ```Position```, ```Strength```.
* **Objects Explorer**: Allows adding, removing, duplicating objects. And selecting for editing in editor.
* **Objects Editor**: Allows setting object [```position```, ```scale```, ```size```, ```textures```, ```materials```, ```scripts```, ```parameters```].
* **Asset Loader**: Allows adding and selecting materials, meshes from files. Uses **Assimp** and saves as custom binary format.
* **Shader Explorer**: Allows refreshing and opening shader editors.
* **Shader Editor**: Multiline text editor for shader with auto reloading.
* **Script Explorer**: Allows opening script editors.  
* **Script Editor**: Multiline text editor for scripts after 5 seconds auto saved to file then hoy-reloaded by [ScriptController](UnderTheWater/ScriptController/) by objects. **![ Preferred to use external editor ]!**

### [Script Controller](UnderTheWater/ScriptController/) and [ScriptShared](UnderTheWater/ScriptShared/)
Scripts in **editor mode** are ```hot-reloaded``` by ```last-time-write``` that allows ease of editing. Each object have it own script bind. Scripts are accessible by name and stored in [Scripts](Scripts/) folder. Temporary editor dlls are created by ```fork``` + ```execvp``` (Linux) and ```system``` (Windows) and stored in [Scripts_DLL](Scripts_DLL) folder. This scripts are controlled by [Script Controller](UnderTheWater/ScriptController/). [ScriptShared](UnderTheWater/ScriptShared/) provides interfaces structures that works as bridge between main App and Script. Uses mainly header files with relative include paths for easier script search and avoiding including whole engine. Script structure is provided by interface [GameObjectScriptInterface](UnderTheWater/ScriptShared/GameObjectScriptInterface.h). For physics use ```FixedUpdate```. In **Production mode** we are compiling scripts with unique class name and register them to [ScriptRegister](UnderTheWater/ScriptShared/ScriptRegister.h) then we creates each instance by ```Factory```. We access scripts by file name that is provided in Script.  
**![ Every object compiles it own dll. That is redundant will be optimized in future ]!**  
**![ Sometimes in editor ui we have to full remove script by delete and re-add it ]!**  
**![ Accessing to object-manager is not stable after hot-reload it doesn't updates child objects ]!**  
**![ Scripts blocks every edition in ui we have to turn them off to change object properties ]!**
**![ Compilation of scripts freezes app because for now we don't use threads ]!**  
**![ No separation in Editor mode to play simulation and editing changes are overwrite by scripts ]!**

#### EDITOR MODE
Editor mode provides easy interface to [```editing/writing/adding/deleting, ...```] [```scene, materials, lights, loading assets, ...```]. Scripts works on ```hot-reloading```. We adds ```ImGui``` ui for editing them.

#### PRODUCTION MODE
Production is designed to create one executable with no additional files requirement. All Assets in [GameData](GameData/) is baked via ```cmrc```. Scripts are compiled and register to main exec soo we don't need ```any dll's```. In Production mode we turn off [```script compilation```, ```ui```, ```debug camera```, ```saving in DataSerializer```].



## Tasks Presentation
### Normal mapping 
<img width="100%" src="./docs/Normals.png">

### PBR 
<img width="100%" src="./docs/PBR.gif">

### Quaternion camera
<img width="100%" src="./docs/QuaterionCamera.gif">

### Shadow mapping
<img width="100%" src="./docs/Shadows.gif">

### Parallel Transport Frames

### Underwater skybox
<img width="100%" src="./docs/Skybox.gif">

### **A09** Ray-marched SDF object
<img width="100%" src="./docs/SDF.gif">

### **B07** Heightmap-based seabed mesh
<img width="100%" src="./docs/Terrain.gif">



## Writing scripts
**![ Docs In Future ]!**



## Supported Platforms
* Linux X11
* Windows 



## Prerequisites
- [**CMake**](https://cmake.org/): 3.31.6
- [**Ninja**](https://ninja-build.org/): 1.12.1
- [**gcc**](https://gcc.gnu.org/): 14.2.0
- [**CWindow**](https://github.com/Daynlight/CWindow.git): under_water
  - **assimp**
  - **Dear ImGui**
  - **Glad**
  - **GLFW**
  - **glm**
  - **stb**
- [**cmrc**](https://github.com/vector-of-bool/cmrc.git): master



## License
[GNU GENERAL v3](LICENSE)



## Author
- Daniel Stodulski



## TODO
<details>
<summary> Iteration 1 </summary>

- [x] Engine setup
- [x] Normals
- [x] Readme.md
- [x] Materials oo SSBO
- [x] Lights on SSBO
- [x] BRDF Shader and PBR
- [x] Add operator[] to Materials and Lights
- [x] Lazy loading and auto compile for Materials and Lights SSBO
- [x] Skybox
- [x] Terrain with height map
- [x] Shader Editor GUI
- [x] cmrc for baking assets
</details>

<details>
<summary> Iteration 2 (7.06.2026) </summary>

- [x] Unified Object system
- [x] Base Object UI
- [x] Base Example for Object with all parts.
- [x] Base Material UI (names)
- [x] Base Shader UI
- [x] Pbr serialization
- [x] lights serialization
- [x] texture load via cmrc backup from disk
- [x] Resource refactor as controller of files
- [x] Backup Plan when loading
- [x] shader movement to ```Assets```
- [x] shader reset btn
- [x] shader save on edit
- [x] Fix bug on delete material
</details>

<details>
<summary> Iteration 3 (15.06.2026) </summary>
  
- [x] Mesh serialization.
- [x] Compile flag for end product ```PRODUCTION```
- [x] Logger UI
- [x] Asset loader UI
- [x] Lights UI
- [x] Full clean up
- [x] Full optimization
</details>

<details>
<summary> Iteration 4(19.06.2026) </summary>

- [x] FBO Render to texture
- [x] Under water fog
- [x] Quaternion Camera, without gimbal lock.
- [x] Shadow Mapping
- [x] SDF ray-marching
- [x] Parallel Transport Layer
</details>

<details>
<summary> Iteration 5 (25.06.2026) </summary>

- [x] Clean up
- [x] Scripts Win support
- [x] Optimization
- [x] Docs
- [x] Assets
- [x] Releases
- [x] Fix FishMovement
- [x] Parameters serialization
- [x] Multiple Scripts seg fault (Scripts)
- [x] Object Add object issue (Scripts)
- [x] Object Delete Script (Scripts)
- [x] Object Duplicate object issue (Scripts)
- [x] Asset Loader seg fault (Scripts)
- [x] Info UI Check
- [x] Log UI Check
- [x] Materials UI Check
- [x] Lights UI Check
- [x] Shaders UI Check
- [x] Scripts UI Check
- [x] Object UI Check
- [x] Asset loader on load terrain break
- [x] Asset loader multiple meshes corrupts
- [x] Asset loader materials didn't import or missing materials
- [x] Asset Loader UI Check
</details>

<details>
<summary> Planed in Future </summary>

- [ ] Better UI.
- [ ] Texture UI.
- [ ] Mesh UI.
- [ ] Rule of 5.
- [ ] Optimization for Compile version (avoid maps).
- [ ] Editor Debug with movement.
- [ ] Editor mode load from folder instead of cmrc. 
- [ ] Last Time Write sync.
- [ ] Move terrain, water, skybox to object_register vector.
- [ ] Window Data Serialization move in different file then ```imgui.ini```.
- [ ] Scripts multiple compilation issue.
- [ ] Script async compile.
- [ ] Game data backup.
- [ ] Production optimization.
- [ ] Scripts param auto reload.
- [ ] Scripts runtime copy.
- [ ] Script runtime simulation and edit.
- [ ] Script on off btn.
</details>