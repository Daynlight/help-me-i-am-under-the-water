// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);

#include "Resources/Resources.h"



UW::DataSerializer &UW::DataSerializer::get(){
  static DataSerializer instance;
  return instance;
};



UW::DataSerializer::DataSerializer()
  : mesh_serializer(std::make_unique<MeshSerialization>()),
    objects_serializer(std::make_unique<ObjectsSerialization>()){};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllObjects(std::vector<GameObject>& objects) {
  objects_serializer->saveAll(objects);
};
#endif



void UW::DataSerializer::loadAllObjects(std::vector<GameObject>& objects) {
  objects_serializer->loadAll(objects);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllMaterials(UW::Materials &materials) {
  materials_serializer.saveAll(materials);
};
#endif



void UW::DataSerializer::loadAllMaterials(UW::Materials &materials) {
  materials_serializer.loadAll(materials);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllLights(UW::Lights &lights) {
  lights_serializer.saveAll(lights);
};
#endif



void UW::DataSerializer::loadAllLights(UW::Lights &lights) {
  lights_serializer.loadAll(lights);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveMesh(const std::string &name, const CW::Renderer::Mesh &mesh) {
  mesh_serializer->save(name, mesh);
};
#endif



void UW::DataSerializer::loadMesh(const std::string& path_to_mesh, UW::Meshes &meshes) {
  mesh_serializer->load(path_to_mesh, meshes);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllMeshes(UW::Meshes &meshes) {
  mesh_serializer->saveAll(meshes);
};
#endif



void UW::DataSerializer::loadAllMeshes(UW::Meshes &meshes) {
  mesh_serializer->loadAll(meshes);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveShaders(const std::string &shader_name, GLuint type){
  shader_serializer.save(shader_name, type);
};
#endif



void UW::DataSerializer::loadShader(const std::string& shader_name){
  shader_serializer.load(shader_name);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveScript(const std::string &script_name, const std::string& source){
  script_serializer.save(script_name, source);
};
#endif



std::string UW::DataSerializer::loadScript(const std::string& script_name){
  return script_serializer.load(script_name);
};




void UW::DataSerializer::loadAllTextures() {
  Logger::get().info("DataSerializer", "Scanning and loading all textures...");

  std::string root_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::TEXTURES_FOLDER;

  if (!root_path.empty() && root_path.back() == '/') root_path.pop_back();

  try {
    auto fs = cmrc::GameData::get_filesystem();
    
    for (auto&& entry : fs.iterate_directory(root_path)) {
      if (entry.is_file()) {
        std::string file_name = entry.filename();
        std::string full_cmrc_path = root_path + "/" + file_name;

        auto file = fs.open(full_cmrc_path); 
        const unsigned char* data_ptr = reinterpret_cast<const unsigned char*>(file.begin());
        
        CW::Renderer::TextureLoader loader(data_ptr, file.size());

        auto it = Resources::get().textures.emplace(file_name, CW::Renderer::Texture()).first;
        it->second.compile(loader.data);
        
        Logger::get().info("DataSerializer", "Loaded texture from CMRC: " + file_name);
      };
    };
  } catch (const std::exception& e) {
    Logger::get().warn("DataSerializer", "[CMRC] Could not scan textures folder: " + std::string(e.what()));
  };

  try {
    std::string local_disk_path = root_path + "/"; 

    if (std::filesystem::exists(local_disk_path) && std::filesystem::is_directory(local_disk_path)) {
      
      for (const auto& entry : std::filesystem::directory_iterator(local_disk_path)) {
        if (entry.is_regular_file()) {
          std::string file_name = entry.path().filename().string();

          if (Resources::get().textures.find(file_name) != Resources::get().textures.end()) continue; 

          std::ifstream file(entry.path(), std::ios::binary | std::ios::ate);
          if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<unsigned char> buffer(size);
            if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
              CW::Renderer::TextureLoader loader(buffer.data(), size);
              
              auto it = Resources::get().textures.emplace(file_name, CW::Renderer::Texture()).first;
              it->second.compile(loader.data);
              
              Logger::get().info("DataSerializer", "Loaded texture from Disk: " + file_name);
            };
          };
        };
      };
    }
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().warn("DataSerializer", "[Filesystem] Could not scan local textures folder: " + std::string(e.what()));
  };
  
  Logger::get().info("DataSerializer", "Finished loading all textures.");
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAll() {
  Logger::get().info("DataSerializer", "Saving all game data...");
  objects_serializer->saveAll(ObjectManager::get().objects);
  materials_serializer.saveAll(Resources::get().materials);
  lights_serializer.saveAll(Resources::get().lights);
  mesh_serializer->saveAll(Resources::get().meshes);
  Logger::get().info("DataSerializer", "All game data has been saved");
};
#endif



void UW::DataSerializer::loadAll() {
  Logger::get().info("DataSerializer", "Loading all game data...");
  mesh_serializer->loadAll(Resources::get().meshes);
  lights_serializer.loadAll(Resources::get().lights);
  materials_serializer.loadAll(Resources::get().materials);
  objects_serializer->loadAll(ObjectManager::get().objects);
  shader_serializer.loadAll();
  loadAllTextures();
  Logger::get().info("DataSerializer", "All game data has been loaded");
};
