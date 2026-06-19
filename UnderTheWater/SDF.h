#pragma once
#include "Renderer.h"

#include "Camera/Camera.h"
#include "Resources/Resources.h"



namespace UW{
struct SDFObject{
  alignas(16) int type;
  alignas(16) glm::vec3 position;
  alignas(16) float size;

  SDFObject(int type, glm::vec3 position, float size);
};



class SDF{
private:
  std::string shader_name;

public:
  SDF(const std::string& shader_name);
  ~SDF();

  void render(CW::Renderer::Framebuffer& fbo, UW::Camera& camera, CW::Renderer::Renderer& window, CW::Renderer::Uniform* uniform = nullptr);
};
};
