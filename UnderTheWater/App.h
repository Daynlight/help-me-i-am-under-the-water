#pragma once
#include "Renderer.h"

#include <vector>
#include <functional>

#include "UI.h"
#include "config.h"
#include "Camera/Camera.h"
#include "Resources/Resources.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Objects/Terrain/Terrain.h"
#include "Objects/Water/Water.h"
#include "Objects/Skybox/Skybox.h"



namespace UW{
class App{
private:
  // app
  CW::Renderer::Renderer window;
  UW::UI ui;

  UW::Camera camera;
  UW::Camera debug_camera;

  bool debug_camera_on = UW::Config::DEFAULT_DEBUG_CAMERA_ON;
  float fps = 0.0f;
  float fps_acc = 0.0f;
  unsigned int fps_id = 0;
  float camera_swap_cooldown_acc = 0.0f;
  float fixed_update_time_acc = 0.0f;

  // objects
  UW::Terrain terrain;
  UW::Water water;
  UW::Skybox skybox;
  std::vector<UW::GameObject> objects;

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
  void swapCamera();
  void updateFps();
};
};
