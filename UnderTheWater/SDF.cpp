#include "SDF.h"



UW::SDF::SDF(const std::string& shader_name)
  :shader_name(shader_name){
};



UW::SDF::~SDF(){

};



void UW::SDF::render(CW::Renderer::Framebuffer& fbo, UW::Camera& camera, CW::Renderer::Renderer& window, CW::Renderer::Uniform* uniform){
  fbo.bind();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  // glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  CW::Renderer::Uniform sdf_uniform; 
  unsigned int quad_mesh_id = Resources::get().meshes.get_id("screen_quad");

  sdf_uniform["transformation"]->set<glm::mat4>(camera.transformation(&window));
  sdf_uniform["lightCount"]->set<int>(Resources::get().lights.size());
  sdf_uniform["cameraPosition"]->set<glm::vec3>(camera.position);
  
  Resources::get().getShader(shader_name).getUniforms().emplace_back(&sdf_uniform);
  if(uniform != nullptr)
    Resources::get().getShader(shader_name).getUniforms().emplace_back(uniform);

  Resources::get().lights.bind(0);
  Resources::get().materials.bind(1);

  Resources::get().getShader(shader_name).bind();
  
  Resources::get().meshes[quad_mesh_id].render();
  
  Resources::get().getShader(shader_name).unbind();

  Resources::get().materials.unbind();
  Resources::get().lights.unbind();

  Resources::get().getShader(shader_name).getUniforms().clear();

  fbo.unbind();

  glDisable(GL_BLEND);
};
