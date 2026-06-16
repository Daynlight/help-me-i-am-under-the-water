#pragma once

#include "Renderer.h"
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <regex>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <cmrc/cmrc.hpp>

#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "config.h"
#include "Utils/utils.h"



namespace UW {
struct MeshRecord {
  std::string name = "";
  struct MeshDataRecord {
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



class MeshSerialization {
public:
  MeshSerialization() = default;
  ~MeshSerialization() = default;

#ifndef PRODUCTION
  void save(const std::string& name, const CW::Renderer::Mesh& mesh);
#endif
  void load(const std::string& path_to_mesh, UW::Meshes& meshes);

#ifndef PRODUCTION
  void saveAll(UW::Meshes& meshes);
#endif
  void loadAll(UW::Meshes& meshes);

private:
#ifndef PRODUCTION
  friend std::ostream& operator<<(std::ostream& os, const MeshRecord& record);
#endif
  friend std::istream& operator>>(std::istream& is, MeshRecord& record);
};

} // namespace UW
