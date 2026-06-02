#pragma once
#include "Renderer.h"
#include "Gui.h"

#include <unordered_map>
#include <functional>
#include <memory>

#include "config.h"
#include "Camera/Camera.h"
#include "Resources/Resources.h"
#include "Objects/Object.h"
#include "Objects/Terrain/Terrain.h"
#include "Objects/Water/Water.h"
#include "Objects/Skybox/Skybox.h"



namespace UW{
class App{
private:
  // app
  CW::Renderer::Renderer window;
  CW::Gui::Gui gui;

  UW::Camera camera;
  UW::Camera debug_camera;

  // control variables
  std::string material_name = UW::Config::DEFAULT_GUI_MATERIAL;
  bool debug_camera_on = UW::Config::DEFAULT_DEBUG_CAMERA_ON;
  float fps = 0.0f;
  float fps_acc = 0.0f;
  unsigned int fps_id = 0;
  float camera_swap_cooldown_acc = 0.0f;
  float fixed_update_time_acc = 0.0f;

  // gui
  bool infoWindowOn = false;
  bool materialWindowOn = false;

  // objects
  std::unordered_map<std::string, std::shared_ptr<Object>> objects;

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

private:
  // gui
  void guiInfo();
  void guiControlsInfo();
  void guiMaterialParameters();
  void guiMaterialList();

  void menuBarGui();

  std::function<void(std::function<void()> render_windows)> appWorkspace();

  std::function<void(CW::Renderer::iRenderer *window)> windowGui();
  std::function<void(CW::Renderer::iRenderer *window)> materialExplorerGui();
};
};
