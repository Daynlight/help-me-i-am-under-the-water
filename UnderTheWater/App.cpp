#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window), light_camera(&window), fbo(1920, 1080), shadows_fbo(1920 * 5, 1080 * 5)
  #ifndef PRODUCTION
  , debug_camera(&window), ui(window, fps, post_processing_on, debug_camera_on, camera, debug_camera, object_manager)
  #endif
  , sdf_register("SDF")
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
  
  camera.position = {174.780f, 26.939f, -80.027f};
  camera.direction = {-0.847f, -0.466f, -0.256f};
  #ifndef PRODUCTION
  debug_camera.position = {453.198f, 250.233f, -26.842f};
  debug_camera.direction = {-0.668f, -0.734f, -0.122f};
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
  for(UW::GameObject& object : object_manager.objects) object.render(&window, light_camera, light_camera, shadows_uniform1);
  
  Resources::get().materials.unbind();
  Resources::get().lights.unbind();

  shadows_fbo.unbind();
};



void UW::App::render(){
  compileShadows();

  fbo.bind();

  window.beginFrame();

  glm::mat4 light_space_matrix = light_camera.transformation(&window);

  CW::Renderer::Uniform shadows_uniform_off;
  shadows_uniform_off["u_ShadowEnabled"]->set<int>(0);
  shadows_uniform_off["u_ShadowDepthTexture"]->set<int>(16);
  shadows_uniform_off["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, shadows_fbo.getDepthTexture());
  CW::Renderer::Uniform shadows_uniform_on;
  shadows_uniform_on["u_ShadowEnabled"]->set<int>(1);
  shadows_uniform_on["u_ShadowDepthTexture"]->set<int>(16);
  shadows_uniform_on["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);

  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);

  #ifndef PRODUCTION
  if(debug_camera_on){ 
    terrain.render(&window, camera, debug_camera, shadows_uniform_on);
    skybox.render(&window, camera, debug_camera, shadows_uniform_off); 
    water.render(&window, camera, debug_camera, shadows_uniform_off);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, debug_camera, shadows_uniform_on);
  }
  else {
  #endif
    terrain.render(&window, camera, camera, shadows_uniform_on);
    skybox.render(&window, camera, camera, shadows_uniform_off);
    water.render(&window, camera, camera, shadows_uniform_off);
    for(UW::GameObject& object : object_manager.objects) object.render(&window, camera, camera, shadows_uniform_on);
  #ifndef PRODUCTION
  };
  #endif
  
  Resources::get().materials.unbind();
  Resources::get().lights.unbind();

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, 0);
  fbo.unbind();

  // SDF
  #ifndef PRODUCTION
  if(debug_camera_on)
    renderSFD(fbo, debug_camera);
  else
#endif
  renderSFD(fbo, camera);

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



void UW::App::renderSFD(CW::Renderer::Framebuffer& fbo, UW::Camera& camera){
  CW::Renderer::Uniform sdf_uniform;
  glm::mat4 light_space_matrix = light_camera.transformation(&window);
  sdf_uniform["u_ShadowDepthTexture"]->set<int>(16);
  sdf_uniform["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);

  glm::vec3 position = {153.0f, 28.0f, -116.0f};
  glm::vec3 rotation = {0.2f, 0.707f, 0.0f};
  glm::vec3 scale = {0.5f, 0.5f, 0.5f};

  glm::vec3 pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 rotationMat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
  glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
  glm::mat4 preRotate = glm::translate(glm::mat4(1.0f), -pivotOffset);
  glm::mat4 postRotate = glm::translate(glm::mat4(1.0f), pivotOffset);
  glm::mat4 model = translationMat * postRotate * rotationMat * preRotate * scaleMat;

  sdf_uniform["model"]->set<glm::mat4>(model);

  sdf_uniform["material_id"]->set<int>(Resources::get().materials.translate_material("SDF"));
  sdf_register.render(fbo, camera, window, &sdf_uniform);
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
  post_uniform["u_Near"]->set<float>(UW::Config::CAMERA_NEAR_PLANE);
  post_uniform["u_Far"]->set<float>(UW::Config::CAMERA_ORTHO_FAR_PLANE);
  post_uniform["u_FogDensity"]->set<float>(0.015f);
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
