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

  Light(glm::vec3 position, glm::vec3 color, float strength = 1.0f);
};



class Lights{
private:
  CW::Renderer::GPUStore buffer;
  bool is_compiled = false;
  std::vector<Light> lights;

public:
  Lights() = default;

  Lights(std::initializer_list<Light> lights);
  ~Lights();

  void compile();
  void destroy();

  void bind(GLuint socket);
  void unbind();

  Light& operator[](unsigned int index);
  Light get(unsigned int index) const;

  void clear();
  void erase(unsigned int index);
  unsigned int size() const;

  void emplace_back(Light light);
  void emplace_back(std::initializer_list<Light> lights);

};
};
