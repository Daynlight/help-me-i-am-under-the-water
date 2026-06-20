#pragma once
#include "Renderer.h"

#include "../Camera/Camera.h"



namespace UW{
class Object{
public:
  virtual void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) = 0;
  virtual void onUpdate(float delta_time) = 0;
  virtual void onFixedUpdate(float fixed_delta_time) = 0;

};
};
