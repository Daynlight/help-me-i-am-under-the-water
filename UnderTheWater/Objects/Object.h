#pragma once
#include "Renderer.h"

#include "../Camera/Camera.h"



namespace UW{
class Object{
public:
  virtual void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera) = 0;
  virtual void onUpdate(float delta_time) = 0;
  virtual void onFixedUpdate() = 0;
};
};
