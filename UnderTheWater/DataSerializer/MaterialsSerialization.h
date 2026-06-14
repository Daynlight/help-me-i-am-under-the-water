#pragma once

#include "Renderer.h"
#include "Resources/Materials/Materials.h"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstring>

#include "Utils/Logger.h"
#include "config.h"



namespace UW {
struct MaterialsRecord {
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



class MaterialsSerialization {
public:
  MaterialsSerialization() = default;
  ~MaterialsSerialization() = default;

  void save(const UW::Material& material);
  void load(UW::Material& material);

  void saveAll(UW::Materials& materials);
  void loadAll(UW::Materials& materials);

private:
  friend std::ostream& operator<<(std::ostream& os, const MaterialsRecord& record);
  friend std::istream& operator>>(std::istream& is, MaterialsRecord& record);
};
}; // namespace UW
