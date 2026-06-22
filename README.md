<div align=center>

# Help Me I'm Under The Water

[![wakatime](https://wakatime.com/badge/user/233b40bd-5512-4e3e-9573-916f7b4127c3/project/408b7767-840b-45a2-b96f-d4c1d6a11ab1.svg)](https://wakatime.com/badge/user/233b40bd-5512-4e3e-9573-916f7b4127c3/project/408b7767-840b-45a2-b96f-d4c1d6a11ab1)
</div>



## Screenshots



## About
Project for **GRK** under water scene. Build on top of [**CWindow library**](https://github.com/Daynlight/CWindow.git). Uses **ImGui** build in **CWindow** for parameters adjustment and visualization.



## Methods
* normal mapping 
* PBR 
* quaternion camera
* shadow mapping
* Parallel Transport Frames 
* underwater skybox
* **A09** Ray-marched SDF object
* **B07** Heightmap-based seabed mesh



## Controls
* **SWAP_CAMERA_BTN**: P (Editor)
* **CAMERA_SWAP_MODE_BTN**: ESC (Editor)
* **CAMERA_ACCELERATE**: O (Editor, Product)
* **CAMERA_DECELERATE**: I (Editor, Product)
* **CAMERA_MOVE_FORWARD**: W (Editor, Product)
* **CAMERA_MOVE_BACK**: S (Editor, Product)
* **CAMERA_MOVE_RIGHT**: D (Editor, Product)
* **CAMERA_MOVE_LEFT**: A (Editor, Product)
* **CAMERA_ROLL_LEFT**: Q (Editor, Product)
* **CAMERA_ROLL_RIGHT**: E (Editor, Product)



## UI
* **Info**
* **Logs**
* **Materials Explorer**
* **Materials Editor**
* **Lights Explorer**
* **Lights Editor**
* **Objects Explorer**
* **Objects Editor**
* **Asset Loader**
* **Shader Explorer**
* **Shader Editor**
* **Script Explorer**
* **Script Editor**



## TOC
- [Screenshots](#screenshots)
- [About](#about)
- [Methods](#methods)
- [Controls](#controls)
- [UI](#ui)
- [TOC](#toc)
- [Installation and Usage](#installation-and-usage)
- [Compiling End Product](#compiling-end-product)
- [Supported Platforms](#supported-platforms)
- [Architecture](#architecture)
  - [Build System](#build-system)
  - [Lights](#lights)
  - [Materials](#materials)
  - [Assets](#assets)
  - [Resources](#resources)
  - [Shaders](#shaders)
  - [Terrain](#terrain)
  - [Water](#water)
- [Prerequisites](#prerequisites)
- [License](#license)
- [Author](#author)
- [TODO](#todo)



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
cmake -B . -DPRODUCTION=ON ..
cmake --build .
cd ..
./build-prod/bin/UnderTheWater
```



## Supported Platforms
* Linux X11
* Windows in plans (Scripts issue) 



## Architecture
### Build System
We use **cmake** for ease of build with **git submodules** for git packages.

### Lights
Lights are store on **SSBO GPU side** with parameters like ```position```, ```color```, ```strength```. It's provides easier way to store light parameters with multiple instances and avoids unnsescary uniforms bindings. They are created once in ```Resources``` Singleton class and reuse for each object. This way we save memory.

### Materials
Materials like Lights are stored on **SSBO GPU side** that allows multiple materials by one mesh. Material contains typical **PBR parameters** like ```albedo```, ```roughness```, ```metallic```, ```emission_color```, ```emission_strength```, ```ambient_occlusion```. We can edit materials via **ImGui** every time we change parameters we recompile **SSBO**.

### Assets
All assets are baked into executable with ```cmrc``` this secures them for **modifications by users** and **stealing**.

### Resources
Resources is singleton class where we initialize all our assets. We use them in other objects like ```terrain```. Resources contains data about ```materials```, ```textures```, ```meshes```, ```shaders```, ```lights```.

### Shaders
Shaders are loaded via cmrc or from folder as fallback are **private** and **safety**.

### Terrain

### Water


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
<summary>Iteration 1</summary>

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
<summary>Iteration 2 (7.06.2026)</summary>

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
<summary>Iteration 3 (15.06.2026)</summary>
  
- [x] Mesh serialization.
- [x] Compile flag for end product ```PRODUCTION```
- [x] Logger UI
- [x] Asset loader UI
- [x] Lights UI
- [x] Full clean up
- [x] Full optimization
</details>

<details>
<summary>Iteration 4(19.06.2026)</summary>

- [x] FBO Render to texture
- [x] Under water fog
- [x] Quaternion Camera, without gimbal lock.
- [x] Shadow Mapping
- [x] SDF ray-marching
- [x] Parallel Transport Layer
</details>

</details open>
<summary> 🌟 Iteration 5 🌟 (25.06.2026) </summary>

- [x] Clean up
- [ ] Optimization
- [ ] Docs
- [ ] Assets
- [ ] Releases
- [ ] Scripts Win support

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
- [x] Asset Loader UI Check
</details>

<details>
<summary>Iteration 6</summary>

- [ ] Terrain sometimes break after adding new mesh (mesh_id).
- [ ] Engine like Shaders, Assets, Textures:
    - [ ] Bake into exe
    - [ ] When edit save to folder
    - [ ] Create new once
    - [ ] Auto load update on folder change
    - [ ] Explorers
    - [ ] Editor
- [ ] Texture UI
- [ ] Mesh UI 
- [ ] Optimization for Compile version (avoid maps)
- [ ] Fix CMRC bad alloc
- [ ] Fix cube data
- [ ] Editor Camera with movement
- [ ] Editor mode load from folder instead of cmrc 
- [ ] Last Time Write sync
- [ ] Script system
- [ ] Move terrain, water, skybox to object_register vector.
- [ ] Window Data Serialization.
- [ ] Fix black lights for new Scene class.
- [ ] Script System for ```PRODUCTION```.
- [ ] Scripts multiple compilation issue.
- [ ] Script async compile.
</details>