#pragma once
#include "Renderer.h"

#include <vector>
#include <string>
#include <initializer_list>



namespace UW{
struct Light{
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 color;
  float strength;

  Light(glm::vec3 position, glm::vec3 color, float strength = 1.0f)
  :position(position), color(color), strength(strength) {};
};



class Lights{
private:
  CW::Renderer::GPUStore buffer;
  bool is_compiled = false;
  std::vector<Light> lights;

public:
  Lights() = default;

  Lights(std::initializer_list<Light> lights)
  : lights(lights) {
    compile();
  };

  ~Lights(){
    destroy();
  };

  void compile(){
    buffer.create();
    buffer.set<Light>(lights);
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

  Light& operator[](unsigned int index){
    is_compiled = false;
    return lights[index];
  };

  void clear(){
    is_compiled = false;
    lights.clear();
  };

  void erase(unsigned int index){
    is_compiled = false;
    lights.erase(lights.begin() + index);
  };

  unsigned int size() const {
    return lights.size();
  };

  void emplace_back(Light light){
    is_compiled = false;
    lights.emplace_back(light);
  };

  void emplace_back(std::initializer_list<Light> lights){
    is_compiled = false;
    for (Light el : lights) this->lights.emplace_back(el);
  };
};
};
