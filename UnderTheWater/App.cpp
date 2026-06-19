#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :scene(window)
#ifndef PRODUCTION
  , ui(window, fps, scene)
#endif
  {
  Logger::get().info("App", "App Initialization");
  
  initWindow();
  onLoad();
};



UW::App::~App(){
  onDestroy();
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
  Logger::get().info("App", "Loading Scene");

  #ifndef PRODUCTION
  ui.onLoad();
  #endif

  DataSerializer::get().loadAll(scene.object_manager.objects);
  scene.onLoad();
};



void UW::App::onDestroy() {
  Logger::get().info("App", "Destroying Scene");

  scene.onDestroy();
  
  Resources::get().destroy();
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

  scene.onUpdate(window);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    
#ifndef PRODUCTION
    guiSettings.window_width = window.getWindowData()->width;
    guiSettings.window_height = window.getWindowData()->height;
#endif
    
    scene.onFixedUpdate(window, fixed_update_time_acc);

    fixed_update_time_acc -= 1.0f / UW::Config::FIXED_HZ;
  };
};



// ============================= //
// ========== Helpers ========== //
// ============================= //
void UW::App::initWindow(){
  Logger::get().info("App", "Window Initialization");

  window.setWindowTitle(UW::Config::WINDOW_TITLE);
  window.setCursorVisibility(UW::Config::DEFAULT_CURSOR_IS_VISIBLE);
  window.setVsync(UW::Config::VSYNC);
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
#endif



#ifndef PRODUCTION
void UW::App::updateFps(){
  if(fps_id > UW::Config::FPS_SAMPLES){
    fps = fps_id / fps_acc;
    fps_acc = 0.0f;
    fps_id = 0;
  }
  else{
    fps_acc += window.getWindowData()->delta_time;
    fps_id++;
  };
};
#endif
