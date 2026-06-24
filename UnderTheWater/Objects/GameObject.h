// Help me I'am Under The Water
// Copyright 2026 Daynlight
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
#include "ScriptShared/GameObjectData.h"
#include "ScriptController/ScriptController.h"



namespace UW{
class GameObjectScriptRecord;



class GameObject : public Object{
private:
  CW::Renderer::Uniform uniform;

public:
  std::vector<UW::GameObjectScriptRecord> scripts;

  std::string mesh_last = "";
  unsigned int mesh_id = -1;
  unsigned int mesh_version = -1;

  UW::GameObjectData game_object_data;

public:
  GameObject(const std::string& name, const std::string& mesh, const std::string& shader, const std::vector<std::string>& materials = {}, const std::vector<std::string>& textures = {}, const std::vector<UW::GameObjectScriptRecord>& scripts = {}, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
  GameObject(const std::string& name, const GameObject& other);
  ~GameObject();
  GameObject(const GameObject& other);
  GameObject& operator=(const GameObject& other);
  GameObject(GameObject&& other) noexcept;
  GameObject& operator=(GameObject&& other) noexcept;

  void onLoad() override;
  void onDestroy() override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate(float fixed_delta_time) override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) override;

  bool isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh);

};
};
