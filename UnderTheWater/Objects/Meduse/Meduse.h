// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the Apache License, Version 2.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"
#include <glm/gtc/quaternion.hpp>

#include <random>
#include <vector>
#include <deque>

#include "config.h"
#include "Utils/utils.h"
#include "Camera/Camera.h"
#include "Resources/Resources.h"
#include "Objects/Object.h"



namespace UW{
class Meduse : public Object{
private:
  CW::Renderer::Uniform sdf_uniform;
  std::deque<glm::vec3> path;

  glm::vec3 position = {153.0f, 28.0f, -116.0f};
  glm::vec3 scale = {0.5f, 0.5f, 0.5f};
  
  glm::vec3 last_tangent = glm::vec3(0.0f);
  glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  float t = 0.0f;
  float speed = 10.0f;

  unsigned int screen_quad_mesh_id = 0;
  unsigned int meshes_version = -1;

public:
  Meduse();
  ~Meduse();

  void onLoad() override;
  void onDestroy() override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate(float fixed_delta_time) override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) override;

  void setPosition(glm::vec3 position);
  void setOrientation(glm::vec3 rotation);
  void setScale(glm::vec3 scale);
  void setSpeed(float speed);
  void setPath(std::deque<glm::vec3> path);

  void genRandom(int i, glm::vec3 position_min, glm::vec3 position_max, glm::vec3 center, glm::vec3 rotation_min, glm::vec3 rotation_max, float scale_min, float scale_max);

};
};
