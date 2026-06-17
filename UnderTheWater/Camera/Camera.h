#pragma once
#include "Renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "../config.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../vendor/glm/glm/gtx/euler_angles.hpp"
#include "../vendor/glm/glm/gtx/quaternion.hpp"



namespace UW {
class Camera {
public:
  glm::vec3 position = {0.0f, 0.0f, 0.0f};
  glm::vec3 direction = {0.0f, 0.0f, 1.0f};
  
private:
  glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); 

  float sensitivity = UW::Config::CAMERA_SENSITIVITY;
  float velocity = UW::Config::CAMERA_DEFAULT_VELOCITY;
  
  float lastMouseX = 0.0f;
  float lastMouseY = 0.0f;
  bool mouse_is_active = false;

  float cursor_visible_lock = 0.0f;
  bool cursor_lock = true;

public:
  Camera(CW::Renderer::Renderer* renderer, glm::vec3 position = {0.0f, 0.0f, 0.0f}, glm::vec3 direction = {0.0f, 0.0f, 1.0f});
  
private:
  void rotate(float xoffset, float yoffset, float zoffset);
  void updateDirection();

public:
  glm::mat4 transformation(CW::Renderer::Renderer* renderer);
  glm::mat4 projection(CW::Renderer::Renderer* renderer);
  glm::mat4 view(CW::Renderer::Renderer* renderer);
  void resetMouse();
  void event(CW::Renderer::Renderer* renderer);

};
};
