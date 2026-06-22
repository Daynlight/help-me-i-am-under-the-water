// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "Utils/utils.h"
#include "config.h"
#include "Lights/Lights.h"
#include "Materials/Materials.h"
#include "Resources/Meshes/Meshes.h"
#include "ScriptController/ScriptController.h"



namespace UW{
class Resources{
public:
  std::unordered_map<std::string, CW::Renderer::Texture> textures;
  std::unordered_map<std::string, CW::Renderer::Shader> shaders;
  UW::Meshes meshes;
  UW::Lights lights;
  UW::Materials materials;

public:
  static Resources& get();

  Resources(const Resources&) = delete;
  Resources& operator=(const Resources&) = delete;
  Resources(Resources&&) = delete;
  Resources& operator=(Resources&&) = delete;

  void destroy();
  
  CW::Renderer::Texture& getTexture(const std::string& path_to_asset);
  CW::Renderer::Shader& getShader(const std::string& path_to_asset);

private:
  Resources();
  ~Resources();

private:
  void initMeshes();
  void initLights();
  
};
};
