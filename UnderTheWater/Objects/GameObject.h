#pragma once
#include "Renderer.h"

#include "../Camera/Camera.h"
#include "Object.h"



namespace UW{
class GameObject{
private:
  std::string mesh = "";
  std::string texture = "";
  std::string shader = "";

public:
  void render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera) override;
  void onUpdate(float delta_time) override;
  void onFixedUpdate() override;
};
};
