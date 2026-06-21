// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



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

  void onLoad() override;
  void onDestroy() override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate(float fixed_delta_time) override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) override;

private:
  void generateChunks();
  bool isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh);

};
}; // namespace UW
