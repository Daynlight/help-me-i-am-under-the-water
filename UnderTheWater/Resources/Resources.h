#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <string>

#include "../utils.h"
#include "Lights/Lights.h"
#include "Shaders/Terrain.h"
#include "Shaders/SkyBox.h"
#include "Shaders/Water.h"



namespace UW{
class Resources{
public:
  std::unordered_map<std::string, CW::Renderer::Mesh> meshes;
  std::unordered_map<std::string, CW::Renderer::Texture> textures;
  std::unordered_map<std::string, CW::Renderer::Shader> shaders;
  std::unordered_map<std::string, CW::PBRMaterial> materials;
  std::unordered_map<std::string, UW::Lights> lights;


public:
  static Resources& get();

  Resources(const Resources&) = delete;
  Resources& operator=(const Resources&) = delete;
  Resources(Resources&&) = delete;
  Resources& operator=(Resources&&) = delete;

  void destroy();

private:
  Resources();
  ~Resources();

private:
  void initMeshes();
  void initTextures();
  void initShaders();
  void initMaterials();
  void initLights();
};
};
