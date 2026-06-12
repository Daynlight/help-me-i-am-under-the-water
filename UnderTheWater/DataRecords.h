#pragma once 
#include "Renderer.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>



namespace UW{
struct GameObjectRecord{
  std::string name = "";
  std::string mesh = "";
  std::string shader = "";
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  std::vector<std::string> textures;
  std::vector<std::string> materials;

  friend std::ostream& operator<<(std::ostream& os, const GameObjectRecord& record);
  friend std::istream& operator>>(std::istream& is, GameObjectRecord& record);
};

struct MaterialsRecord{
  std::string name = "";
  glm::vec3 albedo = glm::vec3(1.0f);
  float metallic = 0.0f;
  float roughness = 1.0f;
  glm::vec3 emission_color = glm::vec3(0.0f);
  float emission_strength = 0.0f;
  float ambient_occlusion = 1.0f;

  friend std::ostream& operator<<(std::ostream& os, const MaterialsRecord& record);
  friend std::istream& operator>>(std::istream& is, MaterialsRecord& record);
};

struct LightsRecord{
  std::string name = "";
  glm::vec3 position;
  glm::vec3 color;
  float strength;

  friend std::ostream& operator<<(std::ostream& os, const LightsRecord& record);
  friend std::istream& operator>>(std::istream& is, LightsRecord& record);
};

struct MeshRecord{
  std::string name = "";
  struct MeshDataRecord{
    unsigned int key = 0;
    std::vector<std::uint8_t> data;
    unsigned int dimension = 0;
    unsigned int size_of_element = 0;
    unsigned int size = 0;
    GLenum type = GL_FLOAT;
  };

  std::vector<GLuint> indices;
  std::vector<MeshDataRecord> mesh_data;

  friend std::ostream& operator<<(std::ostream& os, const MeshRecord& record);
  friend std::istream& operator>>(std::istream& is, MeshRecord& record);
};
};