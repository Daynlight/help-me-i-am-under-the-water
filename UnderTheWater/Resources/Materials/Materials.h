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


  Material(glm::vec3 albedo = glm::vec3(1.0f), 
           float metallic = 0.0f,
           float roughness = 1.0f,
           glm::vec3 emission_color = glm::vec3(0.0f),
           float emission_strength = 0.0f,
           float ambient_occlusion = 1.0f):
           albedo(albedo),
           metallic(metallic),
           roughness(roughness),
           emission_color(emission_color),
           emission_strength(emission_strength),
           ambient_occlusion(ambient_occlusion){};
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

  // Materials(std::initializer_list<Material> materials)
  // : materials(materials) {
  //   compile();
  // };

  ~Materials(){
    destroy();
  };


  void genVectors(){
    materials.clear();
    material_translate.clear();

    for(const std::pair<std::string, Material>& el : material_reg){
      material_translate[el.first] = materials.size();
      materials.emplace_back(el.second);
    };
  };

  void compile(){
    buffer.create();
    genVectors();
    buffer.set<Material>(materials);
    is_compiled = true;
  };

  void destroy(){
    buffer.destroy();
    is_compiled = false;
  };

  void bind(GLuint socket){
    if(!is_compiled) compile();

    buffer.bind(socket);
  };

  void unbind(){
    if(!is_compiled) return;

    buffer.unbind();
  };

  unsigned int translate_material(std::string name){
    return material_translate[name];
  };

  Material& operator[](std::string name){
    is_compiled = false;
    return material_reg[name];
  };

  const std::unordered_map<std::string, Material>& getMaterialReg(){
    return material_reg;
  };

  bool find(std::string name){
    auto it = material_reg.find(name);
    if(it == material_reg.end()) return false;
    return true;
  };

  Material getMaterial(std::string name){
    return material_reg[name];
  };

  void clear(){
    is_compiled = false;
    material_reg.clear();
    materials.clear();
  };

  void erase(std::string name){
    is_compiled = false;
    material_reg.erase(name);
  };

  unsigned int size() const {
    return material_reg.size();
  };

  void emplace_back(std::string name, Material material){
    is_compiled = false;
    material_reg[name] = material;
  };

  void emplace_back(std::initializer_list<std::pair<std::string, Material>> materials){
    is_compiled = false;
    for (std::pair<std::string, Material> el : materials) this->material_reg[el.first] = el.second;
  };
};
};
