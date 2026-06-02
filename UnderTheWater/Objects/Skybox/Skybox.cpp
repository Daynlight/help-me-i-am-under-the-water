#include "Skybox.h"



UW::Skybox::Skybox(){

};



UW::Skybox::~Skybox(){

};



void UW::Skybox::onUpdate(float delta_time){

};



void UW::Skybox::onFixedUpdate(){

};



void UW::Skybox::render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera){
  glDepthFunc(GL_LEQUAL);

  uniform["projection"]->set<glm::mat4>(render_camera.projection(renderer));
  uniform["view"]->set<glm::mat4>(render_camera.view(renderer));
  uniform["skyboxTex"]->set<int>(0);

  Resources::get().shaders["sky_box"].getUniforms().emplace_back(&uniform);

  Resources::get().textures["sky_box"].bind(0); 
  Resources::get().shaders["sky_box"].bind();
  
  Resources::get().meshes["sky_box"].render();
  
  Resources::get().shaders["sky_box"].unbind();
  Resources::get().textures["sky_box"].unbind();
  
  Resources::get().shaders["sky_box"].getUniforms().clear();
  
  glDepthFunc(GL_LESS);
};