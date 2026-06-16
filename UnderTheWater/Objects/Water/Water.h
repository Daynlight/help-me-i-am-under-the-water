#pragma once
#include "Renderer.h"

#include <string.h>
#include <cmath>

#include "Resources/Resources.h"
#include "Camera/Camera.h"
#include "Utils/utils.h"
#include "config.h"
#include "Objects/Object.h"



namespace UW{
class Water : public Object{
private:
  CW::Renderer::Uniform uniform;
  std::vector<glm::vec2> chunks;
  glm::vec2 map_size = glm::vec2(0.0f);
  float elapsed_time = 0.0f;
  unsigned int mesh_id = -1;
  unsigned int mesh_version = -1;

public:
  Water();
  ~Water();

  void onUpdate(float delta_time) override;
  void onFixedUpdate() override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera) override;

private:
  void generateChunks();
  bool isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh);
};
}; // namespace UW
