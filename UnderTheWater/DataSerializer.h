#pragma once
#include "Renderer.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>

#include <cmrc/cmrc.hpp>

#include "config.h"
#include "DataRecords.h"
#include "Objects/Object.h"
#include "Resources/Resources.h"
#include "Objects/GameObject.h"
#include "Resources/Materials/Materials.h"



namespace UW{
class DataSerializer{
private:
  void scanCmrcDirectory(const cmrc::embedded_filesystem& fs, const std::string& current_path, const std::string& pattern_str, std::vector<std::string>& out_mesh_files);

public:
  void saveAllObjects(std::vector<UW::GameObject> &objects);
  void loadAllObjects(std::vector<UW::GameObject> &objects);

  void saveAllMaterials(UW::Materials &materials);
  void loadAllMaterials(UW::Materials &materials);

  void saveAllLights(std::unordered_map<std::string, UW::Lights> &lights);
  void loadAllLights(std::unordered_map<std::string, UW::Lights> &lights);

  void saveMesh(const std::string& name, const CW::Renderer::Mesh& mesh);
  void loadMesh(const std::string& path_to_mesh, std::unordered_map<std::string, CW::Renderer::Mesh> &meshes);

  void saveAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh>& meshes);
  void loadAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh>& meshes);

  void saveShaders(const std::string& path_to_asset, GLuint type);
  // void loadShaders(std::unordered_map<std::string, CW::Renderer::Shader>& shaders);

  // void loadTextures(std::unordered_map<std::string, CW::Renderer::Texture>& textures);

  void saveAll(std::vector<UW::GameObject> &objects);

  void loadAll(std::vector<UW::GameObject> &objects);
};
}; // namespace UW
