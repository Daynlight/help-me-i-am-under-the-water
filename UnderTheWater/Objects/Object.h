// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include "../Camera/Camera.h"



namespace UW{
class Object{
public:
  virtual void onLoad() = 0;
  virtual void onDestroy() = 0;
  virtual void onUpdate(float delta_time) = 0;
  virtual void onFixedUpdate(float fixed_delta_time) = 0;
  virtual void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) = 0;

};
};
