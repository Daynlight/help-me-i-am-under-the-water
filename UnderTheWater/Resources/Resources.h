#pragma once
#include "Renderer.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "../utils.h"
#include "Lights/Lights.h"
#include "Materials/Materials.h"
#include "Shaders/Terrain.h"
#include "Shaders/SkyBox.h"
#include "Shaders/Water.h"
#include "Shaders/Cube.h"



namespace UW{
struct MaterialsRecord{
  std::string name = "";
  glm::vec3 albedo = glm::vec3(1.0f);
  float metallic = 0.0f;
  float roughness = 1.0f;
  glm::vec3 emission_color = glm::vec3(0.0f);
  float emission_strength = 0.0f;
  float ambient_occlusion = 1.0f;

  friend std::ostream& operator<<(std::ostream& os, const MaterialsRecord& record);
  friend std::istream& operator>>(std::istream& is, MaterialsRecord& record);
};




class Resources{
public:
  std::unordered_map<std::string, CW::Renderer::Mesh> meshes;
  std::unordered_map<std::string, CW::Renderer::Texture> textures;
  std::unordered_map<std::string, CW::Renderer::Shader> shaders;
  UW::Materials materials;
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
  void save(const std::string& filepath = "GameData/materials.pbr");
  void load(const std::string& filepath = "GameData/materials.pbr");
  void initMeshes();
  void initTextures();
  void initShaders();
  void initMaterials();
  void initLights();
};
};
