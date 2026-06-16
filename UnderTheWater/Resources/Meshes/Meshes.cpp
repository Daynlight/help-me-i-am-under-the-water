#include "Meshes.h"



UW::Meshes::Meshes() {
};



UW::Meshes::~Meshes() {
};



CW::Renderer::Mesh& UW::Meshes::operator[](unsigned int index) {
  return mesh_data[index];
};



const CW::Renderer::Mesh& UW::Meshes::operator[](unsigned int index) const {
  return mesh_data[index];
};



unsigned int UW::Meshes::get_id(const std::string& name) {
  auto it = mesh_id.find(name);
  if (it == mesh_id.end()) {
    return INVALID_ID;
  };
  return it->second;
};



bool UW::Meshes::exists(const std::string& name) const {
  return mesh_id.find(name) != mesh_id.end();
};



void UW::Meshes::emplace_back(const std::string& name, CW::Renderer::Mesh&& mesh) {
  version += 1;

  auto it = mesh_id.find(name);
  if (it != mesh_id.end()) {
    mesh_data[it->second] = std::move(mesh);
  } else {  
    unsigned int new_id = static_cast<unsigned int>(mesh_data.size());
    
    mesh_data.emplace_back(std::move(mesh));
    mesh_id[name] = new_id;
    id_to_name.push_back(name);
  };
};



void UW::Meshes::erase(const std::string& name) {
  if (!exists(name)) return;
  version += 1;

  unsigned int index_to_remove = mesh_id[name];
  unsigned int last_index = static_cast<unsigned int>(mesh_data.size() - 1);

  if (index_to_remove != last_index) {
    std::swap(mesh_data[index_to_remove], mesh_data[last_index]);

    const std::string& moved_element_name = id_to_name[last_index];

    mesh_id[moved_element_name] = index_to_remove;
    id_to_name[index_to_remove] = moved_element_name;
  };

  mesh_data.pop_back();
  id_to_name.pop_back();
  mesh_id.erase(name);
};



void UW::Meshes::clear(){
  version += 1;
  mesh_data.clear();
  mesh_id.clear();
  id_to_name.clear();
};



std::unordered_map<std::string, unsigned int>& UW::Meshes::getMeshIDs(){
  return mesh_id;
};



bool UW::Meshes::validateVersion(unsigned int version){
  return version == this->version;
};



unsigned int UW::Meshes::getLatestsVersion(){
  return version;
};



void UW::Meshes::compileAll(){
  for(CW::Renderer::Mesh& mesh : mesh_data)
    mesh.compile();
};