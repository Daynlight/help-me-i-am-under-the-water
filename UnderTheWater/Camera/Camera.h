#pragma once
#include "Renderer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/euler_angles.hpp"

#include "../config.h"



namespace UW{
class Camera{
public:
  glm::vec3 position = {0, 0, 0};
  glm::vec3 direction = {0, 0, 1};
  
private:
  float yaw = 0.0f;
  float pitch = 0.0f;
  float sensitivity = 0.1;
  float velocity = UW::Config::CAMERA_DEFAULT_VELOCITY;
  
  float lastMouseX = 0.0f;
  float lastMouseY = 0.0f;

  bool mouse_is_active = false;

  float time = 0.0f;
  float cursor_visible_lock = 0.0f;
  bool cursor_lock = true;

public:
  Camera(CW::Renderer::Renderer* renderer, glm::vec3 position = {0, 0, 0}, glm::vec3 direction = {0, 0, 1});
  
private:
  void rotate(float xoffset, float yoffset);
  void updateDirection();

public:
  glm::mat4 transformation(CW::Renderer::Renderer* renderer);
  glm::mat4 projection(CW::Renderer::Renderer* renderer);
  glm::mat4 view(CW::Renderer::Renderer* renderer);
  void resetMouse();

  void event(CW::Renderer::Renderer* renderer);

};
};
