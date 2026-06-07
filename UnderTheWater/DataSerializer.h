#pragma once
#include "Renderer.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "config.h"
#include "Objects/Object.h"
#include "Resources/Resources.h"
#include "Objects/GameObject.h"
#include "Resources/Materials/Materials.h"



namespace UW{
struct GameObjectRecord{
  std::string name = "";
  std::string mesh = "";
  std::string shader = "";
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  std::vector<std::string> textures;
  std::vector<std::string> materials;

  friend std::ostream& operator<<(std::ostream& os, const GameObjectRecord& record);
  friend std::istream& operator>>(std::istream& is, GameObjectRecord& record);
};

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

struct LightsRecord{
  std::string name = "";
  glm::vec3 position;
  glm::vec3 color;
  float strength;

  friend std::ostream& operator<<(std::ostream& os, const LightsRecord& record);
  friend std::istream& operator>>(std::istream& is, LightsRecord& record);
};

  


class DataSerializer{
public:
  void save(std::vector<UW::GameObject> &objects);
  void load(std::vector<UW::GameObject> &objects);

  void save(UW::Materials &materials);
  void load(UW::Materials &materials);

  void save(std::unordered_map<std::string, UW::Lights> &lights);
  void load(std::unordered_map<std::string, UW::Lights> &lights);


  void save(const std::string& path_to_asset, GLuint type);
};
}; // namespace UW
