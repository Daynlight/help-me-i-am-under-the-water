// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/euler_angles.hpp"

#include <string>

#include "Camera/Camera.h"
#include "Objects/Object.h"
#include "Resources/Resources.h"
#include "ScriptController/ScriptController.h"



namespace UW{
class GameObject : public Object{
private:
  CW::Renderer::Uniform uniform;

public:
  std::string name = "";
  std::string mesh = "";
  std::string shader = "";
  std::vector<std::string> textures;
  std::vector<std::string> materials;
  std::vector<UW::GameObjectScriptRecord> scripts;

  unsigned int mesh_id = -1;
  unsigned int mesh_version = -1;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);

public:
  GameObject(const std::string& name, const std::string& mesh, const std::string& shader, const std::vector<std::string>& materials = {}, const std::vector<std::string>& textures = {}, const std::vector<UW::GameObjectScriptRecord>& scripts = {}, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
  ~GameObject();

  void onLoad() override;
  void onDestroy() override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate(float fixed_delta_time) override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) override;

  bool isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh);

};
};
