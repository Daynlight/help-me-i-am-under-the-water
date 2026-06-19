#include "Scene.h"



UW::Scene::Scene(CW::Renderer::Renderer& window)
  : window(window), light_camera(&window), fbo(1920, 1080), shadows_fbo(1920 * 5, 1080 * 5), camera(&window)
#ifndef PRODUCTION
  , debug_camera(&window)
#endif
   {};



UW::Scene::~Scene(){};



void UW::Scene::onLoad(){
  Logger::get().info("Scene", "Loading Scene");

  camera.position = {174.780f, 26.939f, -80.027f};
  camera.direction = {-0.847f, -0.466f, -0.256f};
  #ifndef PRODUCTION
  debug_camera.position = {453.198f, 250.233f, -26.842f};
  debug_camera.direction = {-0.668f, -0.734f, -0.122f};
  #endif
  
  for(int i = 0; i < 2; i++){
    meduses.emplace_back();
    meduses[i].genRandom(i, 
      glm::vec3(-50, -10, -50), glm::vec3(50, 10, 50), glm::vec3(174.780f, 40.939f, -80.027f),
      glm::vec3(-0.2, -0.2, -0.2), glm::vec3(0.2, 0.2, 0.2),
      0.4f, 0.7f);
  }

  meduses[0].setPath({glm::vec3(117.610, 51.472, -39.445), 
                      glm::vec3(89.665, 25.785, -152.533), 
                      glm::vec3(282.921, 21.784, 0.884)});
    
  compileShadows();
};



void UW::Scene::onUpdate(CW::Renderer::Renderer& window){
  camera.event(&window);

  for(UW::GameObject& el : object_manager.objects) el.onUpdate(window.getWindowData()->delta_time);
  water.onUpdate(window.getWindowData()->delta_time);
};



void UW::Scene::onFixedUpdate(CW::Renderer::Renderer &window, float fixed_delta_time){
#ifndef PRODUCTION
    save_acc += fixed_delta_time;

    if(save_acc >= UW::Config::SAVE_TIMESTAMP){
      save_acc -= UW::Config::SAVE_TIMESTAMP;
      DataSerializer::get().saveAll(object_manager.objects);
    };
#endif

  for(UW::GameObject& el : object_manager.objects) el.onFixedUpdate();
  // for(UW::Meduse& meduse : meduses) meduse.fixedUpdate(fixed_update_time_acc);  
};



void UW::Scene::onDestroy() {
  Logger::get().info("Scene", "Destroying Scene");

  #ifndef PRODUCTION
  DataSerializer::get().saveAll(object_manager.objects);
  #endif
  
  object_manager.objects.clear();
};



void UW::Scene::compileShadows(){
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



void UW::Scene::render(){
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
};



void UW::Scene::renderSFD(CW::Renderer::Framebuffer& fbo, UW::Camera& camera){
  for(UW::Meduse& meduse : meduses) meduse.render(fbo, camera, window);
};



void UW::Scene::postProcessing(){
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
