#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);

#include "Resources/Resources.h"



UW::DataSerializer &UW::DataSerializer::get(){
  static DataSerializer instance;
  return instance;
};



void UW::DataSerializer::scanCmrcDirectory(
  const cmrc::embedded_filesystem& fs,
  const std::string& current_path,
  const std::string& pattern_str,
  std::vector<std::string>& out_mesh_files){
  std::regex pattern(pattern_str);

  for (const auto& entry : fs.iterate_directory(current_path)) {
    std::string entry_path = current_path + (current_path.back() == '/' ? "" : "/") + entry.filename();
    
    if (entry.is_directory()) scanCmrcDirectory(fs, entry_path, pattern_str, out_mesh_files);
    else if (entry.is_file() && std::regex_search(entry.filename(), pattern)) out_mesh_files.emplace_back(entry_path);
  };
};



void UW::DataSerializer::saveAllObjects(std::vector<UW::GameObject> &objects) {
  objects_serializer.saveAll(objects);
};



void UW::DataSerializer::loadAllObjects(std::vector<UW::GameObject> &objects) {
  objects_serializer.loadAll(objects);
};



void UW::DataSerializer::saveAllMaterials(UW::Materials &materials) {
  materials_serializer.saveAll(materials);
};



void UW::DataSerializer::loadAllMaterials(UW::Materials &materials) {
  materials_serializer.loadAll(materials);
};



void UW::DataSerializer::saveAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  lights_serializer.saveAll(lights);
};



void UW::DataSerializer::loadAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  lights_serializer.loadAll(lights);
};



void UW::DataSerializer::saveMesh(const std::string &name, const CW::Renderer::Mesh &mesh) {
  mesh_serializer.save(name, mesh);
};



void UW::DataSerializer::loadMesh(const std::string& path_to_mesh, std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.load(path_to_mesh, meshes);
};



void UW::DataSerializer::saveAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.saveAll(meshes);
};



void UW::DataSerializer::loadAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.loadAll(meshes);
};



void UW::DataSerializer::saveShaders(const std::string &shader_name, GLuint type){
  shader_serializer.save(shader_name, type);
};



void UW::DataSerializer::loadShader(const std::string& shader_name){
  shader_serializer.load(shader_name);
};



void UW::DataSerializer::saveAll(std::vector<UW::GameObject> &objects) {
  Logger::get().info("DataSerializer", "Saving all game data...");
  objects_serializer.saveAll(objects);
  materials_serializer.saveAll(Resources::get().materials);
  lights_serializer.saveAll(Resources::get().lights);
  mesh_serializer.saveAll(Resources::get().meshes);
  Logger::get().info("DataSerializer", "All game data has been saved");
};



void UW::DataSerializer::loadAll(std::vector<UW::GameObject> &objects) {
  Logger::get().info("DataSerializer", "Loading all game data...");
  mesh_serializer.loadAll(Resources::get().meshes);
  lights_serializer.loadAll(Resources::get().lights);
  materials_serializer.loadAll(Resources::get().materials);
  objects_serializer.loadAll(objects);
  Logger::get().info("DataSerializer", "All game data has been loaded");
};
