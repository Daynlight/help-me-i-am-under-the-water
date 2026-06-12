#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window)
  #ifndef END_GAME
  , debug_camera(&window), ui(window, fps, debug_camera_on, camera, debug_camera, object_manager, serializer)
  #endif
  {
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
  #ifndef END_GAME
  ui.onLoad();
  #endif
  
  camera.position = {1055, 797, 1331};
  #ifndef END_GAME
  debug_camera.position = {1157, 2048, 1310};
  debug_camera.direction = {-0.57, -0.76, -0.28};
  #endif

  serializer.load(Resources::get().meshes);
  serializer.load(object_manager.objects);
  serializer.load(Resources::get().materials);
  serializer.load(Resources::get().lights);
};



void UW::App::onDestroy() {
  #ifndef END_GAME
  serializer.save(Resources::get().meshes);
  serializer.save(Resources::get().materials);
  serializer.save(Resources::get().lights);
  serializer.save(object_manager.objects);
  #endif
  object_manager.objects.clear();
  Resources::get().destroy();
};



void UW::App::render(){
  window.beginFrame();  

  Resources::get().lights["static"].bind(0);
  Resources::get().materials.bind(1);

  #ifndef END_GAME
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
  #ifndef END_GAME
  };
  #endif
  
  Resources::get().materials.unbind();
  Resources::get().lights["static"].unbind();
  
  #ifndef END_GAME
  ui.render();
  #endif

  window.windowEvents();
  window.swapBuffer();
};



void UW::App::update(){
  updateFps();
  #ifndef END_GAME
  swapCamera();
  #endif

  camera.event(&window);
  
  for(UW::GameObject& el : object_manager.objects) el.onUpdate(window.getWindowData()->delta_time);
  water.onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    
    guiSettings.window_width = window.getWindowData()->width;
    guiSettings.window_height = window.getWindowData()->height;

    #ifndef END_GAME
    if(save_acc >= UW::Config::SAVE_TIMESTAMP){
      save_acc -= UW::Config::SAVE_TIMESTAMP;
      serializer.save(Resources::get().meshes);
      serializer.save(Resources::get().materials);
      serializer.save(Resources::get().lights);
      serializer.save(object_manager.objects);
    }
    else{
      save_acc += fixed_update_time_acc;
    }
    #endif
    
    for(UW::GameObject& el : object_manager.objects) el.onFixedUpdate();
    fixed_update_time_acc -= 1.0f / UW::Config::FIXED_HZ;
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
  #ifndef END_GAME
  if(window.getInputData()->is_key_down(UW::Config::SWAP_CAMERA_BTN) && camera_swap_cooldown_acc <= 0.0f) {
    debug_camera_on = !debug_camera_on;
    camera_swap_cooldown_acc = UW::Config::CAMERA_SWAP_COOLDOWN;
  };

  if(camera_swap_cooldown_acc >= 0.0f) camera_swap_cooldown_acc -= window.getWindowData()->delta_time;
  #endif
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
