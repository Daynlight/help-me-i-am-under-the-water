// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the Apache License, Version 2.0.
// See LICENSE file for details.



#include "Scene.h"



UW::Scene::Scene(CW::Renderer::Renderer& window)
  : window(window), light_camera(&window), fbo(1920, 1080), shadows_fbo(1920 * 5, 1080 * 5), camera(&window)
#ifndef PRODUCTION
  , debug_camera(&window)
#endif
   {};



UW::Scene::~Scene(){};



void UW::Scene::onLoad(){
  DataSerializer::get().loadAll(object_manager.objects);

  Logger::get().info("Scene", "Loading Scene");


  screen_quad_mesh_id = Resources::get().meshes.get_id("screen_quad");
  meshes_version = Resources::get().meshes.getLatestsVersion();


  post_uniform["u_water_height"]->set<float>(UW::Config::WATER_HEIGHT);
  post_uniform["u_Near"]->set<float>(UW::Config::CAMERA_NEAR_PLANE);
  post_uniform["u_Far"]->set<float>(UW::Config::CAMERA_ORTHO_FAR_PLANE);
  post_uniform["u_FogDensity"]->set<float>(UW::Config::FOG_DENSITY);
  post_uniform["u_FogColor"]->set<glm::vec3>(UW::Config::FOG_COLOR);


  light_camera.setOrthographic(true);

  light_camera.fov = 110.0f;
  last_light_camera_fov = light_camera.fov; 
  
  light_camera.position = Resources::get().lights[0].position;
  last_light_camera_pos = light_camera.position;

  light_camera.direction = glm::normalize(-Resources::get().lights[0].position);
  last_light_camera_dir = light_camera.direction;
  
  light_space_matrix = light_camera.transformation(&window);
  
  shadows_uniform_off["u_ShadowEnabled"]->set<int>(0);
  shadows_uniform_off["u_ShadowDepthTexture"]->set<int>(16);
  shadows_uniform_off["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);
  
  shadows_uniform_on["u_ShadowEnabled"]->set<int>(1);
  shadows_uniform_on["u_ShadowDepthTexture"]->set<int>(16);
  shadows_uniform_on["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);


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
      glm::vec3(-glm::radians(10.0f), -glm::radians(10.0f), -glm::radians(10.0f)), glm::vec3(glm::radians(10.0f), glm::radians(10.0f), glm::radians(10.0f)),
      0.4f, 0.7f
    );
  };

  meduses[0].setPath({glm::vec3(117.610, 51.472, -39.445), 
                      glm::vec3(89.665, 25.785, -152.533),
                      glm::vec3(253.161, 54.430, -68.562), 
                      glm::vec3(282.921, 21.784, 0.884)});
  meduses[0].setOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
  meduses[0].setSpeed(20.0f);
    
  compileShadows();
};



void UW::Scene::onUpdate(float delta_time){
  camera.event(&window);

  for(UW::GameObject& el : object_manager.objects) el.onUpdate(delta_time);
  for(UW::Meduse& meduse : meduses) meduse.onUpdate(delta_time);
  terrain.onUpdate(delta_time);
  skybox.onUpdate(delta_time);
  water.onUpdate(delta_time);
};



void UW::Scene::onFixedUpdate(float fixed_delta_time){
#ifndef PRODUCTION
  save_acc += fixed_delta_time;

  if(save_acc >= UW::Config::SAVE_TIMESTAMP){
    save_acc -= UW::Config::SAVE_TIMESTAMP;
    DataSerializer::get().saveAll(object_manager.objects);
  };
#endif

  for(UW::GameObject& el : object_manager.objects) el.onFixedUpdate(fixed_delta_time);
  for(UW::Meduse& meduse : meduses) meduse.onFixedUpdate(fixed_delta_time);
  terrain.onFixedUpdate(fixed_delta_time);
  skybox.onFixedUpdate(fixed_delta_time);
  water.onFixedUpdate(fixed_delta_time);
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
  
  if(last_light_camera_pos != light_camera.position){
    light_camera.fov = 110.0f;
    last_light_camera_fov = light_camera.fov;

    light_camera.position = Resources::get().lights[0].position;
    last_light_camera_pos = light_camera.position;
    
    light_camera.direction = glm::normalize(-Resources::get().lights[0].position);
    last_light_camera_dir = light_camera.direction;
    
    light_space_matrix = light_camera.transformation(&window);
    shadows_uniform_off["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);
    shadows_uniform_on["u_LightSpaceMatrix"]->set<glm::mat4>(light_space_matrix);
  };  

  window.beginFrame();

  terrain.render(&window, light_camera, light_camera, shadows_uniform_off);
  for(UW::GameObject& object : object_manager.objects) object.render(&window, light_camera, light_camera, shadows_uniform_off);

  shadows_fbo.unbind();
};



void UW::Scene::render(){
  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);


#ifndef PRODUCTION
  if(!shadows_on){
    shadows_fbo.bind();
    window.beginFrame();
    shadows_fbo.unbind();
  }
  else
#endif
    compileShadows();


  fbo.bind();

#ifndef PRODUCTION
  if(debug_camera_on)
    renderFrame(debug_camera);
  else
#endif
    renderFrame(camera);



#ifndef PRODUCTION
  if(debug_camera_on)
    renderSFD(debug_camera);
  else
#endif
    renderSFD(camera);

  fbo.unbind();


  Resources::get().materials.unbind();
  Resources::get().lights.unbind();


  window.beginFrame();


  #ifndef PRODUCTION
  if(!post_processing_on)
    fbo.blitToScreen(window.getWindowData()->width, window.getWindowData()->height);
  else
  #endif
    postProcessing();
};



void UW::Scene::renderFrame(UW::Camera& camera){
  window.beginFrame();

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, shadows_fbo.getDepthTexture());

  terrain.render(&window, this->camera, camera, shadows_uniform_on);
  skybox.render(&window, this->camera, camera, shadows_uniform_off); 
  water.render(&window, this->camera, camera, shadows_uniform_off);
  for(UW::GameObject& object : object_manager.objects) object.render(&window, this->camera, camera, shadows_uniform_on);

  
  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, 0);
};



void UW::Scene::renderSFD(UW::Camera& camera){
  for(UW::Meduse& meduse : meduses) meduse.render(&window, this->camera, camera, shadows_uniform_off);
};



void UW::Scene::postProcessing(){
  if(meshes_version != Resources::get().meshes.getLatestsVersion()){
    screen_quad_mesh_id = Resources::get().meshes.get_id("screen_quad");
    meshes_version = Resources::get().meshes.getLatestsVersion();
  };

  std::string shader_name = "PostProcessing";

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fbo.getColorTexture());
  post_uniform["u_SceneColorTexture"]->set<int>(0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fbo.getDepthTexture());
  post_uniform["u_SceneDepthTexture"]->set<int>(1);


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
  
  Resources::get().meshes[screen_quad_mesh_id].render();
  
  Resources::get().getShader(shader_name).unbind();
  Resources::get().getShader(shader_name).getUniforms().clear();


  glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
};
