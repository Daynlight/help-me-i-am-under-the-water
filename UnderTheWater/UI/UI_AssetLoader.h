// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#ifndef PRODUCTION

#include "Gui.h"
#include "imgui.h"
#include "Utils/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cfloat>

#include <string>
#include <vector>
#include <functional>

#include "Resources/Resources.h"
#include "Objects/ObjectManager.h"
#include "config.h"

#include "UI/Settings.h"
#include "Scene.h"



namespace UW {
struct TempAssetData {
  std::string original_name;
  char new_name[UW::Config::OBJECT_NAME_BUFFER_SIZE];
};



class UI_AssetLoader {
private:
  CW::Gui::Gui& gui;
  UW::Scene& scene;

  Assimp::Importer importer;
  const aiScene* current_scene = nullptr;
  
  char file_path_buffer[256] = {0};
  bool is_loaded = false;
  std::string load_error_msg = "";

  std::vector<bool> mesh_import_toggles;
  std::vector<int> mesh_mat_indices;
  std::vector<bool> material_import_toggles;

  std::vector<TempAssetData> temp_meshes;
  std::vector<TempAssetData> temp_materials;

public:
  UI_AssetLoader(CW::Gui::Gui& gui, UW::Scene& scene);
  ~UI_AssetLoader();

  void uiControl();
  void loadModelFromFile(const std::string& path);
  void guiAssetLoader();
  std::function<void(CW::Renderer::iRenderer *window)> assetLoaderGui();

private:
  void clearTemporaryData();
  void finalizeImport();
  std::vector<int> finalizeMaterials();
  void finalizeMesh(aiMesh* aMesh, const std::string& final_mesh_name, int custom_mat_id);
  void finalizeImportMerged(const std::string& final_merged_name);
};
}; // namespace UW

#endif
