// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "App.h"



UW::App::App()
  :scene(window)
#ifndef PRODUCTION
  , ui(window, fps, scene)
#endif
{
  Logger::get().info("App", "App Initialization");
  
  initWindow();
  onLoad();

  Logger::get().info("App", "App Initialized");
};



UW::App::~App(){
  Logger::get().info("App", "App Destroying");

  onDestroy();
  
  Logger::get().info("App", "App Destroyed");
};



bool UW::App::isRunning(){
  return !window.getWindowData()->should_close;
};



void UW::App::run(){
  update();
  fixedUpdate();
  render();
};



// ===================================== //
// ========== Core Operations ========== //
// ===================================== //
void UW::App::onLoad(){
  Logger::get().info("App", "App Loading");

#ifndef PRODUCTION
  ui.onLoad();
  Logger::get().info("App", "UI Loaded");
#endif

  scene.onLoad();
  Logger::get().info("App", "Scene Loaded");

  Logger::get().info("App", "App Loaded");
};



void UW::App::onDestroy() {
  Logger::get().info("App", "Destroying App");

  ui.onDestroy();
  Logger::get().info("App", "UI Destroyed");

  scene.onDestroy();
  Logger::get().info("App", "Scene Destroyed");
  
  Resources::get().destroy();
  Logger::get().info("App", "Resources Destroyed");

  Logger::get().info("App", "App Destroyed");

#ifndef PRODUCTION
  Logger::get().info("App", "Recorded AVG FPS = " + std::to_string(total_fps_acc / total_fps_id));
#endif
};



void UW::App::render(){
  scene.render();

#ifndef PRODUCTION
  ui.render();
#endif

  window.windowEvents();
  window.swapBuffer();
};




void UW::App::update(){
#ifndef PRODUCTION
  updateFps();
  swapCamera();
#endif

  scene.onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;
  
  float fixed_time_step = 1.0f / UW::Config::FIXED_HZ;

  while(fixed_update_time_acc >= fixed_time_step){
    
#ifndef PRODUCTION
    guiSettings.window_width = window.getWindowData()->width;
    guiSettings.window_height = window.getWindowData()->height;
#endif
    
    scene.onFixedUpdate(fixed_time_step);

    fixed_update_time_acc -= fixed_time_step;
  };
};



// ============================= //
// ========== Helpers ========== //
// ============================= //
void UW::App::initWindow(){
  Logger::get().info("App", "Window Initialization");

  window.setWindowTitle(UW::Config::WINDOW_TITLE);
  Logger::get().info("App", "Title set to - " + UW::Config::WINDOW_TITLE);

  window.setCursorVisibility(UW::Config::DEFAULT_CURSOR_IS_VISIBLE);
  Logger::get().info("App", "Cursor visiblity set to - " + std::string(UW::Config::DEFAULT_CURSOR_IS_VISIBLE == 1 ? "On" : "Off"));

  window.setVsync(UW::Config::VSYNC);
  Logger::get().info("App", "VSync set to - " + std::string(UW::Config::VSYNC != 0 ? "On" : "Off"));

  Logger::get().info("App", "Window Initialized");
};



#ifndef PRODUCTION
void UW::App::swapCamera(){
  if(window.getInputData()->is_key_down(UW::Config::SWAP_CAMERA_BTN) && camera_swap_cooldown_acc <= 0.0f) {
    scene.debug_camera_on = !scene.debug_camera_on;
    camera_swap_cooldown_acc = UW::Config::CAMERA_SWAP_COOLDOWN;

    Logger::get().info("App", "Camera Swapped to { "+ std::string(scene.debug_camera_on ? "DEBUG CAMERA" : "NORMAL CAMERA") + " }");
  };

  if(camera_swap_cooldown_acc >= 0.0f) camera_swap_cooldown_acc -= window.getWindowData()->delta_time;
};



void UW::App::updateFps(){
  if(fps_id > UW::Config::FPS_SAMPLES){
    fps = fps_id / fps_acc;
    fps_acc = 0.0f;
    fps_id = 0;
    total_fps_acc += fps;
    total_fps_id++;
  }
  else{
    fps_acc += window.getWindowData()->delta_time;
    fps_id++;
  };
};
#endif
