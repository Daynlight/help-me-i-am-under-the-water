#include "Camera.h"



UW::Camera::Camera(CW::Renderer::Renderer* renderer, glm::vec3 position, glm::vec3 direction) 
  : position(position) {
  if (glm::length(direction) > 0.0001f) {
    this->direction = glm::normalize(direction);
    this->orientation = glm::quatLookAt(-this->direction, glm::vec3(0.0f, 1.0f, 0.0f));
  } else {
    this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    this->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  }
  resetMouse();
};



void UW::Camera::rotate(float xoffset, float yoffset, float zoffset) {
  glm::quat qPitch = glm::angleAxis(glm::radians(yoffset * sensitivity), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::quat qYaw   = glm::angleAxis(glm::radians(-xoffset * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::quat qRoll  = glm::angleAxis(glm::radians(zoffset), glm::vec3(0.0f, 0.0f, 1.0f));
  
  orientation = orientation * qPitch * qYaw * qRoll;
  orientation = glm::normalize(orientation);
  updateDirection();
};



void UW::Camera::updateDirection() {
  direction = orientation * glm::vec3(0.0f, 0.0f, 1.0f);
  direction = glm::normalize(direction);
};



glm::mat4 UW::Camera::transformation(CW::Renderer::Renderer* renderer){
  return projection(renderer) * view(renderer);
};



glm::mat4 UW::Camera::view(CW::Renderer::Renderer* renderer){
  glm::vec3 dynamicUp = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
  return glm::lookAt(position, position + direction, dynamicUp);
};

glm::mat4 UW::Camera::projection(CW::Renderer::Renderer* renderer){
  float aspectRatio = renderer->getWindowData()->width / (float)renderer->getWindowData()->height;
  return glm::perspective(glm::radians(UW::Config::CAMERA_FOV), aspectRatio, UW::Config::CAMERA_NEAR_PLANE, UW::Config::CAMERA_FAR_PLANE);
};



void UW::Camera::event(CW::Renderer::Renderer* renderer) {
  float dt = renderer->getWindowData()->delta_time;

  if (cursor_lock) renderer->setCursorOn(true);
  else renderer->setCursorOn(false);

  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_SWAP_MODE_BTN) && cursor_visible_lock <= 0.0f) {
    cursor_lock = !cursor_lock;
    cursor_visible_lock = UW::Config::CAMERA_SWAP_COOLDOWN;
    resetMouse();
  }
  else if (cursor_visible_lock > 0.0f) {
    cursor_visible_lock -= dt;
  };

  if (cursor_lock) return;

  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_ACCELERATE)) velocity += UW::Config::CAMERA_ACCELERATION_RATE * dt;
  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_DECELERATE)) velocity -= UW::Config::CAMERA_ACCELERATION_RATE * dt;
  if (velocity < UW::Config::CAMERA_MIN_VELOCITY) velocity = UW::Config::CAMERA_MIN_VELOCITY;

  glm::vec3 right = orientation * glm::vec3(1.0f, 0.0f, 0.0f);
  float target_bank = 0.0f;

  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_FORWARD)) position += direction * velocity * dt;
  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_BACK))    position -= direction * velocity * dt;
  
  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_RIGHT)) {
    position -= right * velocity * dt;
    target_bank -= UW::Config::CAMERA_TILT_ACCELERATION;
  };
  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_LEFT)) {
    position += right * velocity * dt;
    target_bank += UW::Config::CAMERA_TILT_ACCELERATION;
  };

  float xoffset = renderer->getInputData()->mouse_x - lastMouseX;
  float yoffset = renderer->getInputData()->mouse_y - lastMouseY; 
  lastMouseX = renderer->getInputData()->mouse_x;
  lastMouseY = renderer->getInputData()->mouse_y;

  glm::vec3 localUp = glm::inverse(orientation) * glm::vec3(0.0f, 1.0f, 0.0f);
  float current_roll = glm::degrees(glm::atan2(localUp.x, localUp.y));
  
  float zoffset = 0.0f;
  bool manual_roll = false;

  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_ROLL_LEFT)) {
    if (current_roll > -UW::Config::CAMERA_MAX_TILT){
      zoffset -= UW::Config::CAMERA_MANUAL_ROLL_SPEED * dt;
      manual_roll = true;
    };
  };
  if (renderer->getInputData()->is_key_down(UW::Config::CAMERA_ROLL_RIGHT)) {
    if (current_roll < UW::Config::CAMERA_MAX_TILT){
      zoffset += UW::Config::CAMERA_MANUAL_ROLL_SPEED * dt;
      manual_roll = true;
    };
  };

  if (!manual_roll) zoffset += (target_bank - current_roll) * UW::Config::CAMERA_ROLL_INTERPOLATION_SPEED * dt;

  rotate(mouse_is_active ? xoffset : 0.0f, mouse_is_active ? yoffset : 0.0f, zoffset);
  mouse_is_active = true;
};



void UW::Camera::resetMouse(){
  mouse_is_active = false;
};
