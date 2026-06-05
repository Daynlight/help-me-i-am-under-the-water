#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window), debug_camera(&window), ui(window, fps, debug_camera_on, camera, debug_camera, objects){
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
  ui.onLoad();
  
  camera.position = {1055, 797, 1331};
  debug_camera.position = {1157, 2048, 1310};
  debug_camera.direction = {-0.57, -0.76, -0.28};

  objects.emplace_back(UW::GameObject("testing", "testing", {}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
};



void UW::App::onDestroy() {
  objects.clear();
  Resources::get().destroy();
};



void UW::App::render(){
  window.beginFrame();  

  Resources::get().lights["static"].bind(0);
  Resources::get().materials.bind(1);
  if(debug_camera_on){ 
    terrain.render(&window, camera, debug_camera);
    skybox.render(&window, camera, debug_camera); 
    water.render(&window, camera, debug_camera);
    for(UW::GameObject& object : objects) object.render(&window, camera, debug_camera);
  }
  else {
    terrain.render(&window, camera, camera);
    skybox.render(&window, camera, camera);
    water.render(&window, camera, camera);
    for(UW::GameObject& object : objects) object.render(&window, camera, camera);
  };
  
  Resources::get().materials.unbind();
  Resources::get().lights["static"].unbind();
  
  ui.render();

  window.windowEvents();
  window.swapBuffer();
};



void UW::App::update(){
  updateFps();
  swapCamera();

  camera.event(&window);
  
  for(UW::GameObject& el : objects) el.onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    fixed_update_time_acc -= 1.0f / UW::Config::FIXED_HZ;

    guiSettings.window_width = window.getWindowData()->width;
    guiSettings.window_height = window.getWindowData()->height;
    
    for(UW::GameObject& el : objects) el.onFixedUpdate();
  };
};



// ============================= //
// ========== Helpers ========== //
// ============================= //
void UW::App::initWindow(){
  window.setWindowTitle(UW::Config::WINDOW_TITLE);
  window.setCursorVisibility(UW::Config::DEFAULT_CURSOR_IS_VISIBLE);
  window.setVsync(UW::Config::VSYNC);
};



void UW::App::swapCamera(){
  if(window.getInputData()->is_key_down(UW::Config::SWAP_CAMERA_BTN) && camera_swap_cooldown_acc <= 0.0f) {
    debug_camera_on = !debug_camera_on;
    camera_swap_cooldown_acc = UW::Config::CAMERA_SWAP_COOLDOWN;
  };

  if(camera_swap_cooldown_acc >= 0.0f) camera_swap_cooldown_acc -= window.getWindowData()->delta_time;
};



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
