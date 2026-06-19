#pragma once
#include "Renderer.h"
#include <glm/gtc/quaternion.hpp>

#include <random>
#include <vector>
#include <deque>

#include "Resources/Resources.h"
#include "SDF.h"
#include "config.h"


namespace UW{
class Meduse{
private:
  CW::Renderer::Uniform sdf_uniform;
  std::deque<glm::vec3> path;

  glm::vec3 position = {153.0f, 28.0f, -116.0f};
  glm::vec3 scale = {0.5f, 0.5f, 0.5f};
  
  glm::vec3 last_tangent = glm::vec3(0.0f);
  glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  UW::SDF sdf;
  float t = 0.0f;
  float speed = 10.0f;

public:
  Meduse(): sdf("SDF"){};
  ~Meduse(){};

  void render(CW::Renderer::Framebuffer& fbo, UW::Camera& camera, CW::Renderer::Renderer& window){
    glm::vec3 pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMat = glm::mat4_cast(orientation);

    // glm::mat4 tiltAdjust = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 preRotate = glm::translate(glm::mat4(1.0f), -pivotOffset);
    glm::mat4 postRotate = glm::translate(glm::mat4(1.0f), pivotOffset);

    glm::mat4 model = translationMat * postRotate * rotationMat * preRotate * scaleMat;

    sdf_uniform["model"]->set<glm::mat4>(model);

    sdf_uniform["material_id"]->set<int>(Resources::get().materials.translate_material("SDF"));
    sdf.render(fbo, camera, window, &sdf_uniform);
  };

  void setPos(glm::vec3 position){
    this->position = position;
  };

  void setRot(glm::vec3 rotation){
    this->orientation = glm::quat(rotation);
  };

  void setScale(glm::vec3 scale){
    this->scale = scale;
  };

  void setSpeed(float speed){
    this->speed = speed;
  };

  void genRandom(int i, 
    glm::vec3 position_min, glm::vec3 position_max, glm::vec3 center,
    glm::vec3 rotation_min, glm::vec3 rotation_max,
    float scale_min, float scale_max) {
      
    auto hash = [](uint32_t x) {
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      return x;
    };

    std::mt19937 gen(hash(UW::Config::SEED + i));

    std::uniform_real_distribution<float> distPos;
    std::uniform_real_distribution<float> distRot;
    std::uniform_real_distribution<float> distScale(scale_min, scale_max);

    setPos(glm::vec3(
      std::uniform_real_distribution<float>(position_min.x, position_max.x)(gen),
      std::uniform_real_distribution<float>(position_min.y, position_max.y)(gen),
      std::uniform_real_distribution<float>(position_min.z, position_max.z)(gen)
    ) + center);

    setRot(glm::vec3(
      std::uniform_real_distribution<float>(rotation_min.x, rotation_max.x)(gen),
      std::uniform_real_distribution<float>(rotation_min.y, rotation_max.y)(gen),
      std::uniform_real_distribution<float>(rotation_min.z, rotation_max.z)(gen)
    ));

    float s = distScale(gen);
    setScale(glm::vec3(s, s, s));
  };


  void setPath(std::deque<glm::vec3> path){
    this->path = path;

    if (path.size() >= 3) {
      glm::vec3 p0 = path[0];
      glm::vec3 p1 = path[1];
      glm::vec3 p2 = path[2];

      glm::vec3 initial_tangent = (-3.0f * p0) - (-4.0f * p1) + (-1.0f * p2);
      
      if (glm::length(initial_tangent) > 0.001f) {
        initial_tangent = glm::normalize(initial_tangent);
        
        glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);

        if (glm::abs(glm::dot(initial_tangent, world_up)) > 0.999f) {
          world_up = glm::vec3(0.0f, 0.0f, 1.0f); 
        }

        orientation = glm::quatLookAt(initial_tangent, world_up);
        
        last_tangent = initial_tangent;
      }
    }
  };


  
  void fixedUpdate(float delta_time) {
    if (path.size() < 3) return;

    glm::vec3 p0 = path[0];
    glm::vec3 p1 = path[1];
    glm::vec3 p2 = path[2];

    glm::vec3 raw_tangent = (4.0f * t - 3.0f) * p0 
      - (8.0f * t - 4.0f) * p1 
      + (4.0f * t - 1.0f) * p2;

    float tangent_len = glm::length(raw_tangent);
    if (tangent_len < 0.001f) tangent_len = 0.001f;
 
    float dt_pct = (speed * delta_time) / tangent_len;
    bool segment_swapped = false;

    if (t + dt_pct >= 0.5f) {
      float t_needed = 0.5f - t;
      float dt_used = (t_needed * tangent_len) / speed;
      float dt_remaining = glm::max(0.0f, delta_time - dt_used);

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
};
};
