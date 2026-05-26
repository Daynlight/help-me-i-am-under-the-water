#pragma once
#include "Renderer.h"

#include <vector>
#include <string>



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

public:
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
};
};
