#pragma once
#include <string.h>
#include <cmath>

#include "Renderer.h"

#include "../../Resources/Resources.h"
#include "../../Camera/Camera.h"
#include "../Object.h"
#include "../../utils.h"



namespace UW {
class Skybox : public Object {
private:
  CW::Renderer::Uniform uniform;

public:
  Skybox();
  ~Skybox();

  void onUpdate(float delta_time) override;
  void onFixedUpdate() override;

  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera) override;
};
}; // namespace UW
