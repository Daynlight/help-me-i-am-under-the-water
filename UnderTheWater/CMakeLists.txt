# Help me I'am Under The Water
# Copyright 2025 Daynlight
# Licensed under the Apache License, Version 2.0.
# See LICENSE file for details.



cmake_minimum_required(VERSION 3.15)

project(UnderTheWater LANGUAGES CXX C)



set(src
  "main.cpp"
  "App.cpp"
  "Scene.cpp"
  "UI/UI.cpp"
  "UI/UI_Logs.cpp"
  "UI/UI_Materials.cpp"
  "UI/UI_Shaders.cpp"
  "UI/UI_Objects.cpp"
  "UI/UI_Lights.cpp"
  "UI/UI_AssetLoader.cpp"
  "UI/UI_ShaderEditors.cpp"
  "UI/UI_Info.cpp"
  "Utils/Logger.cpp"
  "Camera/Camera.cpp"
  "Resources/Resources.cpp"
  "Resources/Meshes/Meshes.cpp"
  "Resources/Lights/Lights.cpp"
  "Resources/Materials/Materials.cpp"
  "Objects/Terrain/Terrain.cpp"
  "Objects/Water/Water.cpp"
  "Objects/Skybox/Skybox.cpp"
  "Objects/Meduse/Meduse.cpp"
  "Objects/GameObject.cpp"
  "Objects/ObjectManager.cpp"
  "DataSerializer/DataSerializer.cpp"
  "DataSerializer/MeshSerialization.cpp"
  "DataSerializer/ObjectsSerialization.cpp"
  "DataSerializer/MaterialsSerialization.cpp"
  "DataSerializer/LightsSerialization.cpp"
  "DataSerializer/ShaderSerialization.cpp"
)



add_executable(UnderTheWater ${src})

target_link_libraries(UnderTheWater CWindow GameData)
target_include_directories(UnderTheWater PUBLIC .)
