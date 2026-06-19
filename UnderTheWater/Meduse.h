#pragma once
#include "Renderer.h"

#include <random>

#include "Resources/Resources.h"
#include "SDF.h"
#include "config.h"


namespace UW{
class Meduse{
private:
  CW::Renderer::Uniform sdf_uniform;
  glm::vec3 position = {153.0f, 28.0f, -116.0f};
  glm::vec3 rotation = {0.2f, 0.707f, 0.0f};
  glm::vec3 scale = {0.5f, 0.5f, 0.5f};
  UW::SDF sdf;

public:
  Meduse(): sdf("SDF"){};
  ~Meduse(){};

  void render(CW::Renderer::Framebuffer& fbo, UW::Camera& camera, CW::Renderer::Renderer& window){
    glm::vec3 pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 rotationMat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
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
    this->rotation = rotation;
  };

  void setScale(glm::vec3 scale){
    this->scale = scale;
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

    // Use a unique seed per object, then let the generator run sequentially.
    // This provides perfect statistical independence without the overhead of 9 objects.
    std::mt19937 gen(hash(UW::Config::SEED + i));

    std::uniform_real_distribution<float> distPos;
    std::uniform_real_distribution<float> distRot;
    std::uniform_real_distribution<float> distScale(scale_min, scale_max);

    // Random Position
    setPos(glm::vec3(
      std::uniform_real_distribution<float>(position_min.x, position_max.x)(gen),
      std::uniform_real_distribution<float>(position_min.y, position_max.y)(gen),
      std::uniform_real_distribution<float>(position_min.z, position_max.z)(gen)
    ) + center);

    // Random Rotation
    setRot(glm::vec3(
      std::uniform_real_distribution<float>(rotation_min.x, rotation_max.x)(gen),
      std::uniform_real_distribution<float>(rotation_min.y, rotation_max.y)(gen),
      std::uniform_real_distribution<float>(rotation_min.z, rotation_max.z)(gen)
    ));

    // Random Scale (Uniform across axes)
    float s = distScale(gen);
    setScale(glm::vec3(s, s, s));
  };
};
};