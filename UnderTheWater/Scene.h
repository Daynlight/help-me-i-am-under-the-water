#pragma once
#include "Renderer.h"

#include <vector>

#include "Camera/Camera.h"
#include "Objects/ObjectManager.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"

#include "Objects/Terrain/Terrain.h"
#include "Objects/Water/Water.h"
#include "Objects/Skybox/Skybox.h"
#include "Meduse.h"



namespace UW{
class Scene{
public:
  CW::Renderer::Renderer& window;
  CW::Renderer::Framebuffer fbo;
  CW::Renderer::Framebuffer shadows_fbo;
  UW::Camera light_camera;
  UW::Camera camera;

#ifndef PRODUCTION
  UW::Camera debug_camera;
  bool debug_camera_on = UW::Config::DEFAULT_DEBUG_CAMERA_ON;
  bool post_processing_on = UW::Config::DEFAULT_POST_PROCESSING_ON;
  
  float save_acc = 0.0f;
#endif

  UW::Terrain terrain;
  UW::Water water;
  UW::Skybox skybox;
  UW::ObjectManager object_manager;
  std::vector<UW::Meduse> meduses;


public:
  Scene(CW::Renderer::Renderer& window);
  ~Scene();
  
  void onLoad();
  void onDestroy();
  void onUpdate(CW::Renderer::Renderer& window);
  void onFixedUpdate(CW::Renderer::Renderer& window, float fixed_delta_time);
  void render();

private:
  void postProcessing();
  void compileShadows();
  void renderSFD(CW::Renderer::Framebuffer& fbo, UW::Camera& camera);

};
};