// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "Meduse.h"



UW::Meduse::Meduse(){
  onLoad();
};



UW::Meduse::~Meduse(){
  onDestroy();
};



void UW::Meduse::Meduse::onLoad(){
  screen_quad_mesh_id = Resources::get().meshes.get_id("screen_quad");
  meshes_version = Resources::get().meshes.getLatestsVersion();
};



void UW::Meduse::Meduse::onDestroy(){

};



void UW::Meduse::Meduse::onUpdate(float delta_time){

};



void UW::Meduse::Meduse::onFixedUpdate(float fixed_delta_time) {
  if (path.size() < 3) return;

  glm::vec3 p0 = path[0];
  glm::vec3 p1 = path[1];
  glm::vec3 p2 = path[2];

  glm::vec3 raw_tangent = (4.0f * t - 3.0f) * p0 
    - (8.0f * t - 4.0f) * p1 
    + (4.0f * t - 1.0f) * p2;

  float tangent_len = glm::length(raw_tangent);
  if (tangent_len < 0.001f) tangent_len = 0.001f;

  float dt_pct = (speed * fixed_delta_time) / tangent_len;
  bool segment_swapped = false;

  if (t + dt_pct >= 0.5f) {
    float t_needed = 0.5f - t;
    float dt_used = (t_needed * tangent_len) / speed;
    float dt_remaining = glm::max(0.0f, fixed_delta_time - dt_used);

    t = 0.0f; 
    path.push_back(path.front());
    path.pop_front();

    if (path.size() < 3) return;
    p0 = path[0];
    p1 = path[1];
    p2 = path[2];

    raw_tangent = (4.0f * t - 3.0f) * p0 
      - (8.0f * t - 4.0f) * p1 
      + (4.0f * t - 1.0f) * p2;

    tangent_len = glm::length(raw_tangent);
    if (tangent_len < 0.001f) tangent_len = 0.001f;

    t += (speed * dt_remaining) / tangent_len;
    segment_swapped = true; 
  } else {
    t += dt_pct;
  }

  position = 2.0f * (t - 0.5f) * (t - 1.0f) * p0 
    - 4.0f * t * (t - 1.0f) * p1 
    + 2.0f * t * (t - 0.5f) * p2;

  if (glm::length(raw_tangent) > 0.001f) {
    glm::vec3 current_tangent = glm::normalize(raw_tangent);

    if (glm::length(last_tangent) < 0.1f || segment_swapped) {
      last_tangent = current_tangent;
    }

    glm::vec3 axis = glm::cross(last_tangent, current_tangent);
    float dot_prod = glm::clamp(glm::dot(last_tangent, current_tangent), -1.0f, 1.0f);
    
    if (glm::length(axis) > 0.0001f) {
      float angle = std::acos(dot_prod);
      glm::quat delta_rot = glm::angleAxis(angle, glm::normalize(axis));
      
      orientation = delta_rot * orientation;
      orientation = glm::normalize(orientation);
    }

    last_tangent = current_tangent;
  };
};



void UW::Meduse::Meduse::render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform){
  if(meshes_version != Resources::get().meshes.getLatestsVersion()){
    screen_quad_mesh_id = Resources::get().meshes.get_id("screen_quad");
    meshes_version = Resources::get().meshes.getLatestsVersion();
  };

  glm::vec3 pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 rotationMat = glm::mat4_cast(orientation);
  glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
  glm::mat4 preRotate = glm::translate(glm::mat4(1.0f), -pivotOffset);
  glm::mat4 postRotate = glm::translate(glm::mat4(1.0f), pivotOffset);

  glm::mat4 model = translationMat * postRotate * rotationMat * preRotate * scaleMat;
  sdf_uniform["model"]->set<glm::mat4>(model);

  
  sdf_uniform["material_id"]->set<int>(Resources::get().materials.translate_material("SDF"));
  sdf_uniform["lightCount"]->set<int>(Resources::get().lights.size());
  sdf_uniform["transformation"]->set<glm::mat4>(render_camera.transformation(renderer));
  sdf_uniform["cameraPosition"]->set<glm::vec3>(render_camera.position);
  

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Resources::get().getShader("SDF").getUniforms().emplace_back(&sdf_uniform);
  Resources::get().getShader("SDF").bind();
  
  Resources::get().meshes[screen_quad_mesh_id].render();
  
  Resources::get().getShader("SDF").unbind();
  Resources::get().getShader("SDF").getUniforms().clear();

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
};



void UW::Meduse::Meduse::setPosition(glm::vec3 position){
  this->position = position;
};



void UW::Meduse::Meduse::setOrientation(glm::vec3 rotation){
  this->orientation = glm::quat(rotation);
};



void UW::Meduse::Meduse::setScale(glm::vec3 scale){
  this->scale = scale;
};



void UW::Meduse::Meduse::setSpeed(float speed){
  this->speed = speed;
};



void UW::Meduse::Meduse::setPath(std::deque<glm::vec3> path){
  this->path = path;

  if (path.size() >= 3) {
    glm::vec3 p0 = path[0];
    glm::vec3 p1 = path[1];
    glm::vec3 p2 = path[2];

    glm::vec3 initial_tangent = (-3.0f * p0) - (-4.0f * p1) + (-1.0f * p2);
    
    if (glm::length(initial_tangent) > 0.001f) {
      initial_tangent = glm::normalize(initial_tangent);
      
      glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

      if (glm::abs(glm::dot(initial_tangent, world_up)) > 0.999f) world_up = glm::vec3(0.0f, 0.0f, 1.0f);

      orientation = glm::quatLookAt(initial_tangent, world_up);
      
      last_tangent = initial_tangent;
    };
  }
};



void UW::Meduse::Meduse::genRandom(int i, glm::vec3 position_min, glm::vec3 position_max, glm::vec3 center, glm::vec3 rotation_min, glm::vec3 rotation_max, float scale_min, float scale_max){
  std::mt19937 gen(UW::Utils::hash(UW::Config::SEED + i));

  std::uniform_real_distribution<float> distPos;
  std::uniform_real_distribution<float> distRot;
  std::uniform_real_distribution<float> distScale(scale_min, scale_max);

  setPosition(glm::vec3(
    std::uniform_real_distribution<float>(position_min.x, position_max.x)(gen),
    std::uniform_real_distribution<float>(position_min.y, position_max.y)(gen),
    std::uniform_real_distribution<float>(position_min.z, position_max.z)(gen)
  ) + center);

  setOrientation(glm::vec3(
    std::uniform_real_distribution<float>(rotation_min.x, rotation_max.x)(gen),
    std::uniform_real_distribution<float>(rotation_min.y, rotation_max.y)(gen),
    std::uniform_real_distribution<float>(rotation_min.z, rotation_max.z)(gen)
  ));

  float s = distScale(gen);
  setScale(glm::vec3(s, s, s));
};
