# Help Me I'm Under The Water


## About
Project for **GRK** under water scene. Build on top of [**CWindow library**](https://github.com/Daynlight/CWindow.git). Uses **ImGui** build in **CWindow** for parameters adjustment and visualization.


## TOC
- [About](#about)
- [TOC](#toc)
- [Installation and Usage](#installation-and-usage)
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
Shaders are ```inline std::string``` we avoid reading them from file for **safety** and **baking into executable**.

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

<details open>
<summary>🌟 Iteration 2 🌟 (7.06.2026)</summary>

- [x] Unified Object system
- [x] Base Object UI
- [x] Base Example for Object with all parts.
- [x] Base Material UI (names)
- [x] Base Shader UI
- [x] Pbr serialization
- [x] lights serialization
- [x] texture load via cmrc backup from disk
- [x] Resource refactor as controller of files
- [ ] shader movement to ```GameData```
- [ ] mesh movement to ```GameData```
</details>

<details>
<summary>Iteration 3 (10.06.2026)</summary>
- [ ] Last Time Write sync
- [ ] Backup Plan
- [ ] Mesh UI 
- [ ] Lights UI
- [ ] Asset loader UI
- [ ] Texture UI
- [ ] Compile flag for end product
- [ ] Engine like Shaders, Assets, Textures:
    - [ ] Bake into exe
    - [ ] When edit save to folder
    - [ ] Create new once
    - [ ] Auto load update on folder change
    - [ ] Explorers
    - [ ] Editor
- [ ] Full clean up
- [ ] Full optimization
- [ ] Optimization for Compile version (avoid maps)
</details>

<details>
<summary>Iteration 4 (14.06.2026)</summary>

- [ ] Water surface shader
- [ ] FBO Render to texture
- [ ] Under water fog
- [ ] Quaternion Camera
- [ ] Parallel Transport Layer
- [ ] SDF ray-marching
- [ ] Shadow Mapping
- [ ] Script system
</details>
