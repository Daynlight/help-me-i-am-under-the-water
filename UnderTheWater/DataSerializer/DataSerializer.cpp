#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);

#include "Resources/Resources.h"



UW::DataSerializer &UW::DataSerializer::get(){
  static DataSerializer instance;
  return instance;
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllObjects(std::vector<UW::GameObject> &objects) {
  objects_serializer.saveAll(objects);
};
#endif



void UW::DataSerializer::loadAllObjects(std::vector<UW::GameObject> &objects) {
  objects_serializer.loadAll(objects);
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
  mesh_serializer.save(name, mesh);
};
#endif



void UW::DataSerializer::loadMesh(const std::string& path_to_mesh, std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.load(path_to_mesh, meshes);
};



#ifndef PRODUCTION
void UW::DataSerializer::saveAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.saveAll(meshes);
};
#endif



void UW::DataSerializer::loadAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes) {
  mesh_serializer.loadAll(meshes);
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
void UW::DataSerializer::saveAll(std::vector<UW::GameObject> &objects) {
  Logger::get().info("DataSerializer", "Saving all game data...");
  objects_serializer.saveAll(objects);
  materials_serializer.saveAll(Resources::get().materials);
  lights_serializer.saveAll(Resources::get().lights);
  mesh_serializer.saveAll(Resources::get().meshes);
  Logger::get().info("DataSerializer", "All game data has been saved");
};
#endif



void UW::DataSerializer::loadAll(std::vector<UW::GameObject> &objects) {
  Logger::get().info("DataSerializer", "Loading all game data...");
  mesh_serializer.loadAll(Resources::get().meshes);
  lights_serializer.loadAll(Resources::get().lights);
  materials_serializer.loadAll(Resources::get().materials);
  objects_serializer.loadAll(objects);
  Logger::get().info("DataSerializer", "All game data has been loaded");
};
