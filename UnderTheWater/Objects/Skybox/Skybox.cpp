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

  Resources::get().getShader("Shaders/Skybox").getUniforms().emplace_back(&uniform);

  Resources::get().getTexture("Skybox/Skybox.png").bind(0); 
  Resources::get().getShader("Shaders/Skybox").bind();
  
  Resources::get().meshes["sky_box"].render();
  
  Resources::get().getShader("Shaders/Skybox").unbind();
  Resources::get().getTexture("Skybox/Skybox.png").unbind();
  
  Resources::get().getShader("Shaders/Skybox").getUniforms().clear();
  
  glDepthFunc(GL_LESS);
};