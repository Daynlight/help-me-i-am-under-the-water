// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include <vector>
#include <functional>

#ifndef PRODUCTION
#include "UI/UI.h"
#endif

#include "config.h"
#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "Scene.h"



namespace UW{
class App{
private:
  CW::Renderer::Renderer window;
  UW::Scene scene;
  
#ifndef PRODUCTION
  UW::UI ui;
  float fps = 0.0f;
  float fps_acc = 0.0f;
  unsigned int fps_id = 0;

  float total_fps_acc = 0.0f;
  unsigned int total_fps_id = 0;

  float camera_swap_cooldown_acc = 0.0f;
#endif

  float fixed_update_time_acc = 0.0f;

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
  
#ifndef PRODUCTION
  void swapCamera();
  void updateFps();
#endif

};
};
