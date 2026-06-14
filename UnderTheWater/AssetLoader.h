#pragma once

#include "Gui.h"
#include "imgui.h"
#include "Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <functional>

#include "Resources/Resources.h"
#include "ObjectManager.h"
#include "config.h"



namespace UW {
struct TempAssetData {
  std::string original_name;
  char new_name[UW::Config::OBJECT_NAME_BUFFER_SIZE];
};



class AssetLoader {
private:
  CW::Gui::Gui& gui;
  UW::ObjectManager& object_manager;

  Assimp::Importer importer;
  const aiScene* current_scene = nullptr;
  
  char file_path_buffer[256] = {0};
  bool is_loaded = false;
  std::string load_error_msg = "";

  std::vector<TempAssetData> temp_meshes;
  std::vector<TempAssetData> temp_materials;

public:
  AssetLoader(CW::Gui::Gui& gui, UW::ObjectManager& object_manager);
  ~AssetLoader();

  void loadModelFromFile(const std::string& path);
  void guiAssetLoader();
  std::function<void(CW::Renderer::iRenderer *window)> assetLoaderGui();

private:
  void clearTemporaryData();
  void finalizeImport();
  void finalizeMaterials();
  void finalizeMesh(aiMesh* aMesh, const std::string& final_mesh_name);

};
}; // namespace UW