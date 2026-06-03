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

public:
  std::vector<Material> materials;

public:
  Materials() = default;

  Materials(std::initializer_list<Material> materials)
  : materials(materials) {
    compile();
  };

  ~Materials(){
    destroy();
  };

  void compile(){
    buffer.create();
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
};
};
