#pragma once
#include "Renderer.h"

#include <vector>
#include <string>



namespace UW{
struct Material{
  alignas(16) glm::vec3 albedo = glm::vec3(1.0f);
  alignas(4) float metallic = 0.0f;
  alignas(4) float roughness = 1.0f;
  alignas(16) glm::vec3 emission_color = glm::vec3(0.0f);
  alignas(4) float emission_strength = 0.0f;
  alignas(4) float ambient_occlusion = 1.0f;


  Material(
    glm::vec3 albedo = glm::vec3(1.0f), 
    float metallic = 0.0f,
    float roughness = 1.0f,
    glm::vec3 emission_color = glm::vec3(0.0f),
    float emission_strength = 0.0f,
    float ambient_occlusion = 1.0f);
};



class Materials{
private:
  CW::Renderer::GPUStore buffer;
  bool is_compiled = false;
  std::unordered_map<std::string, Material> material_reg;
  std::unordered_map<std::string, unsigned int> material_translate;
  std::vector<Material> materials;

public:
  Materials() = default;
  ~Materials();

  void compile();
  void destroy();
  
  void bind(GLuint socket);
  void unbind();
  
  void genVectors();
  unsigned int translate_material(const std::string& name);
  Material& operator[](const std::string& name);
  const std::unordered_map<std::string, Material>& getMaterialReg();

  bool find(const std::string& name);

  Material getMaterial(const std::string& name);

  void clear();

  void erase(const std::string& name);

  unsigned int size() const;

  void emplace_back(const std::string& name, Material material);
  void emplace_back(std::initializer_list<std::pair<std::string, Material>> materials);

};
};
