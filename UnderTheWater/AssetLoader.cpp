#include "AssetLoader.h"



UW::AssetLoader::AssetLoader(CW::Gui::Gui& gui, UW::ObjectManager& object_manager)
  : gui(gui), object_manager(object_manager) {
  Logger::get().info("AssetLoader", "Initialized Asset Loader");
};



UW::AssetLoader::~AssetLoader() {
  clearTemporaryData();
}



void UW::AssetLoader::clearTemporaryData() {
  importer.FreeScene();
  current_scene = nullptr;
  temp_meshes.clear();
  temp_materials.clear();
  is_loaded = false;
  load_error_msg = "";
}



void UW::AssetLoader::loadModelFromFile(const std::string& path) {
  clearTemporaryData();

  current_scene = importer.ReadFile(path, 
    aiProcess_Triangulate | 
    aiProcess_FlipUVs | 
    aiProcess_CalcTangentSpace |
    aiProcess_GenNormals
  );

  if (!current_scene || current_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !current_scene->mRootNode) {
    load_error_msg = importer.GetErrorString();
    Logger::get().erro("AssetLoader", "Failed to load model: " + load_error_msg);
    return;
  };

  Logger::get().info("AssetLoader", "Successfully loaded model: " + path);

  for (unsigned int i = 0; i < current_scene->mNumMaterials; i++) {
    aiString matName;
    current_scene->mMaterials[i]->Get(AI_MATKEY_NAME, matName);
    
    TempAssetData matData;
    matData.original_name = matName.C_Str();
    if (matData.original_name.empty()) matData.original_name = "Material_" + std::to_string(i);
    
    strncpy(matData.new_name, matData.original_name.c_str(), UW::Config::OBJECT_NAME_BUFFER_SIZE);
    temp_materials.push_back(matData);
  };

  for (unsigned int i = 0; i < current_scene->mNumMeshes; i++) {
    TempAssetData meshData;
    meshData.original_name = current_scene->mMeshes[i]->mName.C_Str();
    if (meshData.original_name.empty()) meshData.original_name = "Mesh_" + std::to_string(i);
    
    strncpy(meshData.new_name, meshData.original_name.c_str(), UW::Config::OBJECT_NAME_BUFFER_SIZE);
    temp_meshes.push_back(meshData);
  };

  is_loaded = true;
};



void UW::AssetLoader::guiAssetLoader() {
  ImGui::SeparatorText("Load Model File");

  ImGui::InputText("File Path", file_path_buffer, sizeof(file_path_buffer));
  
  if (ImGui::Button("Load")) {
    loadModelFromFile(std::string(file_path_buffer));
  };

  if (!load_error_msg.empty()) {
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", load_error_msg.c_str());
  };

  if (is_loaded && current_scene) {
    ImGui::SeparatorText("Scene Data");
    
    ImGui::Text("Meshes found: %d", current_scene->mNumMeshes);
    ImGui::Text("Materials found: %d", current_scene->mNumMaterials);

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Rename Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (size_t i = 0; i < temp_materials.size(); i++) {
        ImGui::PushID(static_cast<int>(i) + 1000);
        ImGui::Text("Orig: %s", temp_materials[i].original_name.c_str());
        ImGui::SameLine();
        ImGui::InputText("##NewMatName", temp_materials[i].new_name, UW::Config::OBJECT_NAME_BUFFER_SIZE);
        ImGui::PopID();
      };
    };

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Rename Meshes & Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
      for (size_t i = 0; i < temp_meshes.size(); i++) {
        ImGui::PushID(static_cast<int>(i) + 2000);
        ImGui::Text("Orig: %s", temp_meshes[i].original_name.c_str());
        ImGui::SameLine();
        ImGui::InputText("##NewMeshName", temp_meshes[i].new_name, UW::Config::OBJECT_NAME_BUFFER_SIZE);
        ImGui::PopID();
      };
    };

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Import to Engine", ImVec2(-1, 40))) {
      finalizeImport();
    };
  };
};



void UW::AssetLoader::finalizeImport() {
  Logger::get().info("AssetLoader", "Finalizing Import...");

  finalizeMaterials();

  
  for (size_t i = 0; i < temp_meshes.size(); i++) {
    aiMesh* aMesh = current_scene->mMeshes[i];
    std::string final_mesh_name = temp_meshes[i].new_name;

    finalizeMesh(aMesh, final_mesh_name);

    std::string assigned_mat_name = temp_materials[aMesh->mMaterialIndex].new_name;

    UW::GameObject new_obj(
      final_mesh_name,
      final_mesh_name,
      UW::Config::DEFAULT_SHADER,
      { assigned_mat_name },
      {},
      glm::vec3(0.0f),
      glm::vec3(0.0f),
      glm::vec3(1.0f)
    );

    object_manager.objects.push_back(new_obj);
  };

  Logger::get().info("AssetLoader", "Import successful!");
  clearTemporaryData(); 
};



void UW::AssetLoader::finalizeMaterials(){
  Logger::get().info("AssetLoader", "Finalizing Materials...");

  for (size_t i = 0; i < temp_materials.size(); i++) {
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
  };

  Logger::get().info("AssetLoader", "Materials Import successful!");
};



void UW::AssetLoader::finalizeMesh(aiMesh* aMesh, const std::string& final_mesh_name){
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> uvs;
  std::vector<int> mat_ids;
  std::vector<unsigned int> indices;

  positions.reserve(aMesh->mNumVertices * 4);
  normals.reserve(aMesh->mNumVertices * 3);
  uvs.reserve(aMesh->mNumVertices * 2);
  mat_ids.reserve(aMesh->mNumVertices * 1);

  float mat_id = static_cast<float>(aMesh->mMaterialIndex);

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

  Resources::get().meshes[final_mesh_name] = std::move(temp_mesh);
};



std::function<void(CW::Renderer::iRenderer *window)> UW::AssetLoader::assetLoaderGui() {
return [this](CW::Renderer::iRenderer *window) {
  guiAssetLoader();
};
};
