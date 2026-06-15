#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window)
  #ifndef PRODUCTION
  , debug_camera(&window), ui(window, fps, debug_camera_on, camera, debug_camera, object_manager)
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
  
  camera.position = {1055, 797, 1331};
  #ifndef PRODUCTION
  debug_camera.position = {1157, 2048, 1310};
  debug_camera.direction = {-0.57, -0.76, -0.28};
  #endif

  DataSerializer::get().loadAll(object_manager.objects);
};



void UW::App::onDestroy() {
  Logger::get().info("App", "Destroying Scene");

  #ifndef PRODUCTION
  DataSerializer::get().saveAll(object_manager.objects);
  #endif
  
  object_manager.objects.clear();
  Resources::get().destroy();
};



void UW::App::render(){
  window.beginFrame();  

  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);

  #ifndef PRODUCTION
  if(debug_camera_on){ 
    terrain.render(&window, camera, debug_camera);
    skybox.render(&window, camera, debug_camera); 
    water.render(&window, camera, debug_camera);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, debug_camera);
  }
  else {
  #endif
    terrain.render(&window, camera, camera);
    skybox.render(&window, camera, camera);
    water.render(&window, camera, camera);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, camera);
  #ifndef PRODUCTION
  };
  #endif
  
  Resources::get().materials.unbind();
  Resources::get().lights.unbind();
  
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

  camera.event(&window);
  
  for(UW::GameObject& el : object_manager.objects) el.onUpdate(window.getWindowData()->delta_time);
  water.onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    
#ifndef PRODUCTION
    guiSettings.window_width = window.getWindowData()->width;
    guiSettings.window_height = window.getWindowData()->height;
#endif

#ifndef PRODUCTION
    save_acc += fixed_update_time_acc;

    if(save_acc >= UW::Config::SAVE_TIMESTAMP){
      save_acc -= UW::Config::SAVE_TIMESTAMP;
      DataSerializer::get().saveAll(object_manager.objects);
    };
#endif
    
    for(UW::GameObject& el : object_manager.objects) el.onFixedUpdate();
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
    debug_camera_on = !debug_camera_on;
    camera_swap_cooldown_acc = UW::Config::CAMERA_SWAP_COOLDOWN;

  Logger::get().info("App", "Camera Swapped to { "+ std::string(debug_camera_on ? "DEBUG CAMERA" : "NORMAL CAMERA") + " }");
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
