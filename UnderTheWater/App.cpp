#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window), light_camera(&window), fbo(1920, 1080), shadows_fbo(1920 * 5, 1080 * 5)
  #ifndef PRODUCTION
  , debug_camera(&window), ui(window, fps, post_processing_on, debug_camera_on, camera, debug_camera, object_manager)
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
  
  camera.position = {779.276f, 101.339f, 1218.920f};
  camera.direction = {-0.873f, -0.198f, -0.447f};
  #ifndef PRODUCTION
  debug_camera.position = {1468.995, 2074.450, 1875.771};
  debug_camera.direction = {-0.327, -0.919, -0.220};
  #endif

  DataSerializer::get().loadAll(object_manager.objects);
  compileShadows();
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
  fbo.bind();

  window.beginFrame();

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, shadows_fbo.getDepthTexture());
  CW::Renderer::Uniform shadows_uniform;
  shadows_uniform["u_ShadowEnabled"]->set<int>(1);
  shadows_uniform["u_ShadowDepthTexture"]->set<int>(16);
  glm::mat4 light_space_matrix = light_camera.transformation(&window);
  shadows_uniform["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);

  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);

  #ifndef PRODUCTION
  if(debug_camera_on){ 
    terrain.render(&window, camera, debug_camera, shadows_uniform);
    skybox.render(&window, camera, debug_camera); 
    water.render(&window, camera, debug_camera);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, debug_camera);
  }
  else {
  #endif
    terrain.render(&window, camera, camera, shadows_uniform);
    skybox.render(&window, camera, camera);
    water.render(&window, camera, camera);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, camera);
  #ifndef PRODUCTION
  };
  #endif
  
  Resources::get().materials.unbind();
  Resources::get().lights.unbind();

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, 0);
  fbo.unbind();
  
  int width, height;
  glfwGetFramebufferSize(window.getWindow(), &width, &height);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  #ifndef PRODUCTION
  if(!post_processing_on)
    fbo.blitToScreen(width, height);
  else
  #endif
    postProcessing();

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
  
  if(Resources::get().lights[0].position != shadow_last_position) compileShadows();

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



void UW::App::postProcessing(){
  CW::Renderer::Uniform post_uniform; 

  unsigned int quad_mesh_id = Resources::get().meshes.get_id("screen_quad");

  std::string shader_name = "PostProcessing";

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fbo.getColorTexture());
  post_uniform["u_SceneColorTexture"]->set<int>(0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fbo.getDepthTexture());
  post_uniform["u_SceneDepthTexture"]->set<int>(1);

  post_uniform["u_water_height"]->set<float>(UW::Config::WATER_HEIGHT);
  post_uniform["u_Near"]->set<float>(0.1f);
  post_uniform["u_Far"]->set<float>(4000.0f);
  post_uniform["u_FogDensity"]->set<float>(0.003f);
  glm::vec3 fog_color = {0.0f, 0.4f, 0.55f};
  post_uniform["u_FogColor"]->set<glm::vec3>(fog_color);

#ifndef PRODUCTION
  if(debug_camera_on){
    glm::mat4 invViewProj = glm::inverse(debug_camera.projection(&window) * debug_camera.view(&window));
    post_uniform["u_InvViewProj"]->set<glm::mat4>(invViewProj);
    post_uniform["u_CamPos"]->set<glm::vec3>(debug_camera.position);
  }
  else{
#endif
    glm::mat4 invViewProj = glm::inverse(camera.projection(&window) * camera.view(&window));
    post_uniform["u_InvViewProj"]->set<glm::mat4>(invViewProj);
    post_uniform["u_CamPos"]->set<glm::vec3>(camera.position);
#ifndef PRODUCTION
  }
#endif

  Resources::get().getShader(shader_name).getUniforms().emplace_back(&post_uniform);
  
  Resources::get().getShader(shader_name).bind();
  
  Resources::get().meshes[quad_mesh_id].render();
  
  Resources::get().getShader(shader_name).unbind();
  Resources::get().getShader(shader_name).getUniforms().clear();

  glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
};



void UW::App::compileShadows(){
    shadows_fbo.bind();
  light_camera.setOrthographic(true);
  light_camera.fov = 110.0f;
  light_camera.position = Resources::get().lights[0].position;
  light_camera.direction = glm::normalize(-Resources::get().lights[0].position);
  glm::mat4 light_space_matrix = light_camera.transformation(&window);
  
  CW::Renderer::Uniform shadows_uniform1;
  shadows_uniform1["u_ShadowEnabled"]->set<int>(0);
  shadows_uniform1["u_ShadowDepthTexture"]->set<int>(16);
  shadows_uniform1["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);

  window.beginFrame();

  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);

  terrain.render(&window, light_camera, light_camera, shadows_uniform1);
  for(UW::GameObject& object : object_manager.objects) object.render(&window, light_camera, light_camera);
  
  Resources::get().materials.unbind();
  Resources::get().lights.unbind();

  shadows_fbo.unbind();

  shadow_last_position = Resources::get().lights[0].position;
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
