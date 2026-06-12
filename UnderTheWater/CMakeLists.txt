cmake_minimum_required(VERSION 3.15)

project(UnderTheWater LANGUAGES CXX C)

set(src
  "main.cpp"
  "App.cpp"
  "UI.cpp"
  "ObjectManager.cpp"
  "DataSerializer.cpp"
  "DataRecords.cpp"
  "Resources/Resources.cpp"
  "Camera/Camera.cpp"
  "Objects/Terrain/Terrain.cpp"
  "Objects/Water/Water.cpp"
  "Objects/Skybox/Skybox.cpp"
  "Objects/GameObject.cpp"
)


add_executable(UnderTheWater ${src})
target_link_libraries(UnderTheWater CWindow GameData)
target_include_directories(UnderTheWater PUBLIC .)