#include "Camera.h"







UW::Camera::Camera(CW::Renderer::Renderer* renderer, glm::vec3 position, glm::vec3 direction) 
  :position(position), direction(direction){
  resetMouse();
};



void UW::Camera::rotate(float xoffset, float yoffset) {
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if(pitch > 89.0f) pitch = 89.0f;
  if(pitch < -89.0f) pitch = -89.0f;

  updateDirection();
};



void UW::Camera::updateDirection() {
  glm::vec3 dir;
  dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  dir.y = sin(glm::radians(pitch));
  dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction = glm::normalize(dir);
};



glm::mat4 UW::Camera::transformation(CW::Renderer::Renderer* renderer){
  glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0,1,0));
  glm::mat4 projection = glm::perspective(glm::radians(80.0f), renderer->getWindowData()->width / (float)renderer->getWindowData()->height, 0.1f, 4000.0f);
  glm::mat4 mvp = projection * view;
  
  return mvp;
};



glm::mat4 UW::Camera::view(CW::Renderer::Renderer* renderer){
  glm::mat4 view = glm::lookAt(position, position + direction, glm::vec3(0,1,0));
  return view;
};



glm::mat4 UW::Camera::projection(CW::Renderer::Renderer* renderer){
  glm::mat4 projection = glm::perspective(glm::radians(80.0f), renderer->getWindowData()->width / (float)renderer->getWindowData()->height, 0.1f, 4000.0f);
  return projection;
};



void UW::Camera::event(CW::Renderer::Renderer* renderer) {

  if(cursor_lock) renderer->setCursorOn(true);
  else renderer->setCursorOn(false);

  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_SWAP_MODE_BTN) && cursor_visible_lock <= 0.0f) {
    cursor_lock = !cursor_lock;
    cursor_visible_lock = 0.5f;
    resetMouse();
  }
  else if(cursor_visible_lock > 0.0f) cursor_visible_lock -= renderer->getWindowData()->delta_time;

  if(cursor_lock) return;

  glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));

  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_ACCELERATE)) velocity += 40.0f * renderer->getWindowData()->delta_time;
  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_DECELERATE)) velocity -= 40.0f * renderer->getWindowData()->delta_time;
  if(velocity < 1.1f) velocity = 1.1f;

  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_FORWARD)) position += direction * velocity * renderer->getWindowData()->delta_time;
  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_BACK)) position -= direction * velocity * renderer->getWindowData()->delta_time;
  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_LEFT)) position -= right * velocity * renderer->getWindowData()->delta_time;
  if(renderer->getInputData()->is_key_down(UW::Config::CAMERA_MOVE_RIGHT)) position += right * velocity * renderer->getWindowData()->delta_time;

  float xoffset = renderer->getInputData()->mouse_x - lastMouseX;
  float yoffset = lastMouseY - renderer->getInputData()->mouse_y;

  lastMouseX = renderer->getInputData()->mouse_x;
  lastMouseY = renderer->getInputData()->mouse_y;

  if(mouse_is_active) rotate(xoffset, yoffset);

  mouse_is_active = true;
};



void UW::Camera::resetMouse(){
  mouse_is_active = false;
};
