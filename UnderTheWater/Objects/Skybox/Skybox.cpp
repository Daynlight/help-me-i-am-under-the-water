// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "Skybox.h"



UW::Skybox::Skybox(){
  mesh_id = Resources::get().meshes.get_id("sky_box");
  onLoad();
};



UW::Skybox::~Skybox(){
  onDestroy();
};



void UW::Skybox::onLoad(){

};



void UW::Skybox::onDestroy(){
  
};



void UW::Skybox::onUpdate(float delta_time){

};



void UW::Skybox::onFixedUpdate(float fixed_delta_time){

};



void UW::Skybox::render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform){
  if(Resources::get().meshes.validateVersion(mesh_version)){
    mesh_version = Resources::get().meshes.getLatestsVersion();
    mesh_id = Resources::get().meshes.get_id("sky_box");
  };

  glDepthFunc(GL_LEQUAL);

  uniform["projection"]->set<glm::mat4>(render_camera.projection(renderer));
  uniform["view"]->set<glm::mat4>(render_camera.view(renderer));
  uniform["skyboxTex"]->set<int>(0);

  Resources::get().getShader("Skybox").getUniforms().emplace_back(&uniform);
  Resources::get().getShader("Skybox").getUniforms().emplace_back(&shadows_uniform);

  Resources::get().getTexture("Skybox/Skybox.png").bind(0); 
  Resources::get().getShader("Skybox").bind();
  
  Resources::get().meshes[mesh_id].render();
  
  Resources::get().getShader("Skybox").unbind();
  Resources::get().getTexture("Skybox/Skybox.png").unbind();
  
  Resources::get().getShader("Skybox").getUniforms().clear();
  
  glDepthFunc(GL_LESS);
};
