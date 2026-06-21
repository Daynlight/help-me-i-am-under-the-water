// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include <string.h>
#include <cmath>

#include "Renderer.h"

#include "Resources/Resources.h"
#include "Camera/Camera.h"
#include "Objects/Object.h"
#include "Utils/utils.h"



namespace UW {
class Skybox : public Object {
private:
  CW::Renderer::Uniform uniform;
  unsigned int mesh_id = 0;
  unsigned int mesh_version = -1;

public:
  Skybox();
  ~Skybox();

  void onLoad() override;
  void onDestroy() override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate(float fixed_delta_time) override;
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform) override;
};
}; // namespace UW
