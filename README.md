# Help Me I'm Under The Water


## About
Project for **GRK** under water scene. Build on top of [**CWindow library**](https://github.com/Daynlight/CWindow.git). Uses **ImGui** build in **CWindow** for parameters adjustment and visualization.


## TOC
- [About](#about)
- [TOC](#toc)
- [Installation and Usage](#installation-and-usage)
- [Architecture](#architecture)
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


## Prerequisites
- [**CMake**](https://cmake.org/): 3.31.6
- [**Ninja**](https://ninja-build.org/): 1.12.1
- [**gcc**](https://gcc.gnu.org/): 14.2.0
- [**CWindow**](https://github.com/Daynlight/CWindow.git): dev
- [**cmrc**](https://github.com/vector-of-bool/cmrc.git): master


## License
[GNU GENERAL v3](LICENSE)


## Author
- Daniel Stodulski


## TODO
- [x] Skybox
- [x] Terrain
- [x] Normals
- [x] PBR
- [ ] Readme.md
- [ ] Update Materials to SSBO
- [ ] FBO Render to texture
- [ ] Quaternion Camera
- [ ] Parallel Transport Layer
- [ ] SDF ray-marching
- [ ] Shadow Mapping