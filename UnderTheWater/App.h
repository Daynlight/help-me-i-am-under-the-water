#pragma once
#include "Renderer.h"

#include <vector>
#include <functional>

#ifndef PRODUCTION
#include "UI/UI.h"
#endif

#include "config.h"
#include "Utils/Logger.h"
#include "Camera/Camera.h"
#include "Resources/Resources.h"
#include "Objects/ObjectManager.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/Terrain/Terrain.h"
#include "Objects/Water/Water.h"
#include "Objects/Skybox/Skybox.h"
#include "DataSerializer/DataSerializer.h"
#include "SDF.h"



namespace UW{
class App{
private:
  // app
  CW::Renderer::Renderer window;
  CW::Renderer::Framebuffer fbo;
  CW::Renderer::Framebuffer shadows_fbo;
  
  #ifndef PRODUCTION
  UW::UI ui;
  #endif
  
  UW::Camera camera;
  UW::Camera light_camera;
  
  #ifndef PRODUCTION
  UW::Camera debug_camera;
  bool debug_camera_on = UW::Config::DEFAULT_DEBUG_CAMERA_ON;
  bool post_processing_on = UW::Config::DEFAULT_POST_PROCESSING_ON;

  float fps = 0.0f;
  float fps_acc = 0.0f;
  unsigned int fps_id = 0;

  float camera_swap_cooldown_acc = 0.0f;
  float save_acc = 0.0f;
  #endif

  float fixed_update_time_acc = 0.0f;

  // objects
  UW::Terrain terrain;
  UW::Water water;
  UW::Skybox skybox;
  UW::ObjectManager object_manager;
  UW::SDF sdf_register;


public:
  App();
  ~App();

  bool isRunning();
  void run();
  
private:
  // core operations
  void onLoad();
  void onDestroy();
  void render();
  void update();
  void fixedUpdate();

  // helpers
  void initWindow();
  void postProcessing();
  void compileShadows();
  void renderSFD(CW::Renderer::Framebuffer& fbo, UW::Camera& camera);
  
#ifndef PRODUCTION
  void swapCamera();
  void updateFps();
#endif
};
};
