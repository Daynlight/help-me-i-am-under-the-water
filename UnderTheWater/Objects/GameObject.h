#pragma once
#include "Renderer.h"

#include "Camera/Camera.h"
#include "Resources/Resources.h"
#include "Objects/Object.h"

#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/euler_angles.hpp"



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
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);

public:
  GameObject(std::string name, std::string mesh, std::string shader, const std::vector<std::string>& materials = {}, const std::vector<std::string>& textures = {}, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));
  ~GameObject();

  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera) override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate() override;

  bool isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh);
};
};
