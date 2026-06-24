// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "UI_AssetLoader.h"

#ifndef PRODUCTION



UW::UI_AssetLoader::UI_AssetLoader(CW::Gui::Gui& gui, UW::Scene& scene)
  : gui(gui), scene(scene) {
  Logger::get().info("UI_AssetLoader", "Initialized Asset Loader");
};



UW::UI_AssetLoader::~UI_AssetLoader() {
  clearTemporaryData();
};



void UW::UI_AssetLoader::uiControl(){
  if(guiSettings.assetLoaderWindowOn){
    Logger::get().info("UI", "Opening Asset Loader GUI");
    gui.addWindow("Asset Loader", assetLoaderGui());
  } else {
    gui.deleteWindow("Asset Loader");
  };
};



void UW::UI_AssetLoader::clearTemporaryData() {
  importer.FreeScene();
  current_scene = nullptr;
  temp_meshes.clear();
  temp_materials.clear();
  
  mesh_import_toggles.clear();
  mesh_mat_indices.clear();
  material_import_toggles.clear();
  
  is_loaded = false;
  load_error_msg = "";
};



void UW::UI_AssetLoader::loadModelFromFile(const std::string& path) {
  clearTemporaryData();

  current_scene = importer.ReadFile(path, 
    aiProcess_Triangulate | 
    aiProcess_CalcTangentSpace |
    aiProcess_GenNormals
  );

  if (!current_scene || current_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !current_scene->mRootNode) {
    load_error_msg = importer.GetErrorString();
    Logger::get().erro("UI_AssetLoader", "Failed to load model: " + load_error_msg);
    return;
  };

  Logger::get().info("UI_AssetLoader", "Successfully loaded model: " + path);

  for (unsigned int i = 0; i < current_scene->mNumMaterials; i++) {
    aiString matName;
    current_scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
    
    TempAssetData matData;
    matData.original_name = matName.C_Str();
    if (matData.original_name.empty()) matData.original_name = "Material_" + std::to_string(i);
    
    strncpy(matData.new_name, matData.original_name.c_str(), UW::Config::OBJECT_NAME_BUFFER_SIZE);
    temp_materials.push_back(matData);
    
    material_import_toggles.push_back(true);
  };

  for (unsigned int i = 0; i < current_scene->mNumMeshes; i++) {
    TempAssetData meshData;
    meshData.original_name = current_scene->mMeshes[i]->mName.C_Str();
    if (meshData.original_name.empty()) meshData.original_name = "Mesh_" + std::to_string(i);
    
    strncpy(meshData.new_name, meshData.original_name.c_str(), UW::Config::OBJECT_NAME_BUFFER_SIZE);
    temp_meshes.push_back(meshData);
    
    mesh_import_toggles.push_back(true);
    mesh_mat_indices.push_back(static_cast<int>(current_scene->mMeshes[i]->mMaterialIndex));
  };

  is_loaded = true;
};



void UW::UI_AssetLoader::guiAssetLoader() {
  ImGui::SeparatorText("Load Model File");

  ImGui::InputText("File Path", file_path_buffer, sizeof(file_path_buffer));
  ImGui::SameLine();
  if (ImGui::Button("Load", ImVec2(100, 0))) {
    loadModelFromFile(std::string(file_path_buffer));
  };

  if (!load_error_msg.empty()) {
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", load_error_msg.c_str());
  };

  if (is_loaded && current_scene) {
    ImGui::Spacing();
    ImGui::Separator();
    
    float panel_height = 400.0f;
    float left_width = ImGui::GetContentRegionAvail().x * 0.55f;

    ImGui::BeginChild("SettingsPanel", ImVec2(left_width, panel_height), true);
    
    ImGui::Text("Meshes found: %d | Materials found: %d", current_scene->mNumMeshes, current_scene->mNumMaterials);
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Materials Dictionary", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (size_t i = 0; i < temp_materials.size(); i++) {
        ImGui::PushID(static_cast<int>(i) + 1000);
        
        bool do_import_mat = material_import_toggles[i];
        if (ImGui::Checkbox("##ImportMat", &do_import_mat)) {
          material_import_toggles[i] = do_import_mat;
        }
        ImGui::SameLine();

        ImGui::Text("Orig: %s", temp_materials[i].original_name.c_str());
        ImGui::SameLine(180.0f);
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##NewMatName", temp_materials[i].new_name, UW::Config::OBJECT_NAME_BUFFER_SIZE);
        ImGui::PopID();
      };
    };

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Meshes Configuration", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (size_t i = 0; i < temp_meshes.size(); i++) {
        ImGui::PushID(static_cast<int>(i) + 2000);
        
        bool do_import_mesh = mesh_import_toggles[i];
        if (ImGui::Checkbox("##ImportMesh", &do_import_mesh)) {
          mesh_import_toggles[i] = do_import_mesh;
        }
        ImGui::SameLine();

        ImGui::SetNextItemWidth(120.0f);
        ImGui::InputText("##NewMeshName", temp_meshes[i].new_name, UW::Config::OBJECT_NAME_BUFFER_SIZE);
        ImGui::SameLine();

        if (!temp_materials.empty()) {
          int current_mat_idx = mesh_mat_indices[i];
          if (current_mat_idx >= temp_materials.size()) current_mat_idx = 0;
          
          ImGui::SetNextItemWidth(-1);
          if (ImGui::BeginCombo("##MatCombo", temp_materials[current_mat_idx].new_name)) {
            for (size_t m = 0; m < temp_materials.size(); m++) {
              bool is_selected = (current_mat_idx == static_cast<int>(m));
              if (ImGui::Selectable(temp_materials[m].new_name, is_selected)) {
                mesh_mat_indices[i] = static_cast<int>(m);
              }
              if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
          }
        }
        ImGui::PopID();
      };
    };
    ImGui::EndChild();

    
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Import as Separate Meshes", ImVec2(-1, 35))) {
      finalizeImport();
    };
    
    ImGui::Spacing();
    
    static char merged_obj_name[256] = "Merged_Object";
    
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Merged Mesh Name:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 210.0f);
    ImGui::InputText("##MergedNameInput", merged_obj_name, sizeof(merged_obj_name));
    
    ImGui::SameLine();
    if (ImGui::Button("Import as Single Merged Mesh", ImVec2(200.0f, 0))) {
      finalizeImportMerged(std::string(merged_obj_name));
    };
  };
};




void UW::UI_AssetLoader::finalizeImport() {
  Logger::get().info("UI_AssetLoader", "Finalizing Separate Import...");

  std::vector<int> oldToNew = finalizeMaterials();

  for (size_t i = 0; i < temp_meshes.size(); i++) {
    if (!mesh_import_toggles[i]) continue;

    aiMesh* aMesh = current_scene->mMeshes[i];
    std::string final_mesh_name = temp_meshes[i].new_name;
    
    int oldMatIdx = mesh_mat_indices[i];
    int finalMatIdx = oldToNew[oldMatIdx];
    if (finalMatIdx == -1) finalMatIdx = 0;
    
    finalizeMesh(aMesh, final_mesh_name, finalMatIdx);

    std::string assigned_mat_name = temp_materials[finalMatIdx].new_name;

    UW::GameObject new_obj(
      final_mesh_name,
      final_mesh_name,
      UW::Config::DEFAULT_SHADER,
      { assigned_mat_name },
      {},
      {},
      glm::vec3(0.0f),
      glm::vec3(0.0f),
      glm::vec3(1.0f)
    );

    UW::ObjectManager::get().objects.push_back(new_obj);
  };

  Logger::get().info("UI_AssetLoader", "Separate Import successful!");
  clearTemporaryData(); 
};



void UW::UI_AssetLoader::finalizeImportMerged(const std::string& merged_name) {
  Logger::get().info("UI_AssetLoader", "Finalizing Merged Import...");

  std::vector<int> oldToNew = finalizeMaterials();

  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<int> mat_ids;
  std::vector<unsigned int> indices;

  unsigned int vertexOffset = 0;
  
  std::vector<std::string> assigned_materials;
  for (size_t i = 0; i < temp_materials.size(); i++) {
    if (material_import_toggles[i]) {
      assigned_materials.push_back(temp_materials[i].new_name);
    }
  };

  for (size_t i = 0; i < temp_meshes.size(); i++) {
    if (!mesh_import_toggles[i]) continue;

    aiMesh* aMesh = current_scene->mMeshes[i];
    int oldMatIdx = mesh_mat_indices[i];
    int finalMatIdx = oldToNew[oldMatIdx];
    if (finalMatIdx == -1) finalMatIdx = 0;

    for (unsigned int v = 0; v < aMesh->mNumVertices; v++) {
      positions.push_back(aMesh->mVertices[v].x);
      positions.push_back(aMesh->mVertices[v].y);
      positions.push_back(aMesh->mVertices[v].z);
      positions.push_back(1.0f);

      if (aMesh->HasNormals()) {
        normals.push_back(aMesh->mNormals[v].x);
        normals.push_back(aMesh->mNormals[v].y);
        normals.push_back(aMesh->mNormals[v].z);
      } else {
        normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(0.0f);
      };

      if (aMesh->HasTextureCoords(0)) {
        uvs.push_back(aMesh->mTextureCoords[0][v].x);
        uvs.push_back(aMesh->mTextureCoords[0][v].y);
      } else {
        uvs.push_back(0.0f); uvs.push_back(0.0f);
      };

      mat_ids.push_back(finalMatIdx);
    };

    for (unsigned int f = 0; f < aMesh->mNumFaces; f++) {
      aiFace face = aMesh->mFaces[f];
      for (unsigned int ind = 0; ind < face.mNumIndices; ind++) {
        indices.push_back(face.mIndices[ind] + vertexOffset);
      };
    };
    
    vertexOffset += aMesh->mNumVertices;
  };

  std::string final_merged_name = merged_name.empty() ? "Merged_Object" : merged_name;

  CW::Renderer::Mesh temp_mesh;
  temp_mesh.addVertices(positions);
  temp_mesh.setData<float>(normals, 3, 1);
  temp_mesh.setData<float>(uvs, 2, 2);
  temp_mesh.setData<int>(mat_ids, 1, 3);
  temp_mesh.addIndices(indices);

  Resources::get().meshes.emplace_back(final_merged_name, std::move(temp_mesh));

  UW::GameObject new_obj(
    final_merged_name,
    final_merged_name,
    UW::Config::DEFAULT_SHADER,
    assigned_materials,
    {},
    {},
    glm::vec3(0.0f),
    glm::vec3(0.0f),
    glm::vec3(1.0f)
  );

  UW::ObjectManager::get().objects.push_back(new_obj);

  Logger::get().info("UI_AssetLoader", "Merged Import successful!");
  clearTemporaryData(); 
};



std::vector<int> UW::UI_AssetLoader::finalizeMaterials(){
  Logger::get().info("UI_AssetLoader", "Finalizing Materials...");

  std::vector<int> oldToNewMap(temp_materials.size(), -1);
  int newIndex = 0;

  for (size_t i = 0; i < temp_materials.size(); i++) {
    if (!material_import_toggles[i]) continue;

    std::string final_mat_name = temp_materials[i].new_name;
    UW::Material new_mat;
    
    aiMaterial* material = current_scene->mMaterials[i];
    
    aiColor3D color(1.f, 1.f, 1.f);
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color))
      new_mat.albedo = glm::vec3(color.r, color.g, color.b);
    
    float roughness = 0.5f;
    if (AI_SUCCESS == material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness))
      new_mat.roughness = roughness;

    float metallic = 0.0f;
    if (AI_SUCCESS == material->Get(AI_MATKEY_METALLIC_FACTOR, metallic))
      new_mat.metallic = metallic;

    aiColor3D emissive_color(0.f, 0.f, 0.f);
    if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive_color)) new_mat.emission_color = glm::vec3(emissive_color.r, emissive_color.g, emissive_color.b);
    else new_mat.emission_color = glm::vec3(0.0f);

    float emission_strength = 0.0f;
    if (AI_SUCCESS == material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emission_strength)) new_mat.emission_strength = emission_strength;
    else new_mat.emission_strength = (new_mat.emission_color != glm::vec3(0.0f)) ? 1.0f : 0.0f;

    float ambient_occlusion = 1.0f;
#ifdef AI_MATKEY_GLTF_TEXTURE_STRENGTH
    if (AI_SUCCESS != material->Get(AI_MATKEY_GLTF_TEXTURE_STRENGTH(aiTextureType_LIGHTMAP, 0), ambient_occlusion)) {
      ambient_occlusion = 1.0f;
    }
#endif
    new_mat.ambient_occlusion = ambient_occlusion;

    Resources::get().materials.emplace_back(final_mat_name, new_mat);
    oldToNewMap[i] = newIndex;
    newIndex++;
  };

  Logger::get().info("UI_AssetLoader", "Materials Import successful!");
  return oldToNewMap;
};



void UW::UI_AssetLoader::finalizeMesh(aiMesh* aMesh, const std::string& final_mesh_name, int custom_mat_id){
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<int> mat_ids;
  std::vector<unsigned int> indices;

  positions.reserve(aMesh->mNumVertices * 4);
  normals.reserve(aMesh->mNumVertices * 3);
  uvs.reserve(aMesh->mNumVertices * 2);
  mat_ids.reserve(aMesh->mNumVertices * 1);

  float mat_id = static_cast<float>(custom_mat_id);

  for (unsigned int v = 0; v < aMesh->mNumVertices; v++) {
    positions.push_back(aMesh->mVertices[v].x);
    positions.push_back(aMesh->mVertices[v].y);
    positions.push_back(aMesh->mVertices[v].z);
    positions.push_back(1.0f);

    if (aMesh->HasNormals()) {
      normals.push_back(aMesh->mNormals[v].x);
      normals.push_back(aMesh->mNormals[v].y);
      normals.push_back(aMesh->mNormals[v].z);
    } else {
      normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(0.0f);
    };

    if (aMesh->HasTextureCoords(0)) {
      uvs.push_back(aMesh->mTextureCoords[0][v].x);
      uvs.push_back(aMesh->mTextureCoords[0][v].y);
    } else {
      uvs.push_back(0.0f); uvs.push_back(0.0f);
    };

    mat_ids.push_back(mat_id);
  };

  for (unsigned int f = 0; f < aMesh->mNumFaces; f++) {
    aiFace face = aMesh->mFaces[f];
    for (unsigned int ind = 0; ind < face.mNumIndices; ind++) {
      indices.push_back(face.mIndices[ind]);
    };
  };

  CW::Renderer::Mesh temp_mesh;

  temp_mesh.addVertices(positions);
  temp_mesh.setData<float>(normals, 3, 1);
  temp_mesh.setData<float>(uvs, 2, 2);
  temp_mesh.setData<int>(mat_ids, 1, 3);
  temp_mesh.addIndices(indices);

  Resources::get().meshes.emplace_back(final_mesh_name, std::move(temp_mesh));
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_AssetLoader::assetLoaderGui() {
return [this](CW::Renderer::iRenderer *window) {
  guiAssetLoader();
};
};

#endif
