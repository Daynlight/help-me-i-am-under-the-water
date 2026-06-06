cmake_minimum_required(VERSION 3.15)

project(UnderTheWater LANGUAGES CXX C)

set(src
  "main.cpp"
  "App.cpp"
  "UI.cpp"
  "ObjectManager.cpp"
  "Resources/Resources.cpp"
  "Camera/Camera.cpp"
  "Objects/Terrain/Terrain.cpp"
  "Objects/Water/Water.cpp"
  "Objects/Skybox/Skybox.cpp"
  "Objects/GameObject.cpp"
)

cmrc_add_resource_library(assets
  "Assets/Skybox/Skybox.png"
  "Assets/Terrain/Terrain.png"
)

add_executable(UnderTheWater ${src})
target_link_libraries(UnderTheWater CWindow assets)
target_include_directories(UnderTheWater PUBLIC .)