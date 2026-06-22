// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "MeshSerialization.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



#ifndef PRODUCTION
void UW::MeshSerialization::save(const std::string& name, const CW::Renderer::Mesh& mesh) {
  Logger::get().info("MeshSerialization", "Saving mesh: " + name);
  std::string folder_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::MESHES_FOLDER;
  std::string file_path = folder_path + name + UW::Config::MESH_EXTENSION;

  std::filesystem::create_directories(folder_path);

  std::ofstream outFile(file_path, std::ios::binary);
  if (!outFile.is_open()) return;

  UW::MeshRecord record;
  record.name = name;
  record.indices = mesh.getIndices();

  auto reg = mesh.getDataRegister();
  record.mesh_data.clear();
  record.mesh_data.reserve(reg.size());

  for (const auto& [location, mesh_data_instance] : reg) {
    UW::MeshRecord::MeshDataRecord e;
    e.key = location;
    e.dimension = mesh_data_instance.getDimension();
    e.size_of_element = mesh_data_instance.getSizeOfElement();
    e.size = mesh_data_instance.getSize();
    e.type = mesh_data_instance.getType();

    size_t byte_count = e.size;
    const uint8_t* raw = reinterpret_cast<const uint8_t*>(mesh_data_instance.getRawData());
    e.data.assign(raw, raw + byte_count);

    record.mesh_data.emplace_back(std::move(e));
  };

  outFile << record;
  Logger::get().info("MeshSerialization", "Mesh saved: " + name);
};
#endif



void UW::MeshSerialization::load(const std::string& path_to_mesh, UW::Meshes& meshes) {
  Logger::get().info("MeshSerialization", "Loading mesh: " + path_to_mesh);
  try {
    auto fs = cmrc::GameData::get_filesystem();
    auto embedded_file = fs.open(path_to_mesh);
    std::string data_str(embedded_file.begin(), embedded_file.end());
    std::stringstream inFile(data_str);

    UW::MeshRecord record;
    if (!(inFile >> record)) return;

    CW::Renderer::Mesh engine_mesh;
    engine_mesh.addIndices(record.indices);

    std::unordered_set<unsigned int> loaded_layouts;

    for (const auto& e : record.mesh_data) {
      if (e.data.empty()) continue;
      if (loaded_layouts.count(e.key)) continue;
      loaded_layouts.insert(e.key);

      if (e.key == 0 && e.type == GL_FLOAT) {
        std::vector<float> vertices(e.data.size() / sizeof(float));
        std::memcpy(vertices.data(), e.data.data(), e.data.size());
        engine_mesh.addVertices(vertices, e.dimension, e.key);
      } else {
        UW::Utils::uploadBufferByType(engine_mesh, e.type, e.data, e.dimension, e.key);
      };
    };

    meshes.emplace_back(record.name, std::move(engine_mesh));
    
    Logger::get().info("MeshSerialization", "Mesh loaded: " + record.name);
  } catch (const std::exception& e) {
    Logger::get().erro("MeshSerialization", "CMRC EXCEPTION: " + std::string(e.what()));
  };
};



#ifndef PRODUCTION
void UW::MeshSerialization::saveAll(UW::Meshes& meshes) {
  Logger::get().info("MeshSerialization", "Saving all meshes...");
  
  std::vector<std::pair<std::string, unsigned int>> meshes_to_save;
  
  for (const auto& pair : meshes.getMeshIDs())
    meshes_to_save.push_back(pair);

  for (const auto& [mesh_name, mesh_id] : meshes_to_save)
    save(mesh_name, meshes[mesh_id]);
  
  Logger::get().info("MeshSerialization", "All meshes have been saved");
};
#endif



void UW::MeshSerialization::loadAll(UW::Meshes& meshes) {
  Logger::get().info("MeshSerialization", "Loading all meshes...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string meshes_root = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::MESHES_FOLDER;

    if (!fs.exists(meshes_root)) return;

    std::vector<std::string> mesh_files;
    UW::Utils::scanCmrcDirectory(fs, meshes_root, "\\.msh$", mesh_files);

    for (const auto& file_path : mesh_files) load(file_path, meshes);

    meshes.compileAll();

    Logger::get().info("MeshSerialization", "All meshes have been loaded");
  } catch (const std::exception& e) {
    Logger::get().erro("MeshSerialization", "CMRC EXCEPTION: " + std::string(e.what()));
  };
};



#ifndef PRODUCTION
std::ostream& UW::operator<<(std::ostream& os, const UW::MeshRecord& record){
  size_t name_size = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
  if (name_size > 0) os.write(record.name.data(), name_size);

  size_t index_count = record.indices.size();
  os.write(reinterpret_cast<const char*>(&index_count), sizeof(index_count));
  if (index_count > 0) os.write(reinterpret_cast<const char*>(record.indices.data()), index_count * sizeof(record.indices[0]));

  size_t buffer_count = record.mesh_data.size();
  os.write(reinterpret_cast<const char*>(&buffer_count), sizeof(buffer_count));

  for (const auto& e : record.mesh_data){
    os.write(reinterpret_cast<const char*>(&e.key), sizeof(e.key));
    os.write(reinterpret_cast<const char*>(&e.dimension), sizeof(e.dimension));
    os.write(reinterpret_cast<const char*>(&e.size_of_element), sizeof(e.size_of_element));
    os.write(reinterpret_cast<const char*>(&e.size), sizeof(e.size));
    os.write(reinterpret_cast<const char*>(&e.type), sizeof(e.type));

    size_t byte_count = e.data.size();
    os.write(reinterpret_cast<const char*>(&byte_count), sizeof(byte_count));
    if (byte_count > 0) os.write(reinterpret_cast<const char*>(e.data.data()), byte_count);
  };

  return os;
};
#endif



std::istream& UW::operator>>(std::istream& is, UW::MeshRecord& record){
  size_t name_size = 0;
  if (!is.read(reinterpret_cast<char*>(&name_size), sizeof(name_size))) return is;
  
  record.name.resize(name_size);
  if (name_size > 0) is.read(&record.name[0], name_size);

  size_t index_count = 0;
  is.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));
  
  record.indices.resize(index_count);
  if (index_count > 0) is.read(reinterpret_cast<char*>(record.indices.data()), index_count * sizeof(record.indices[0]));

  size_t buffer_count = 0;
  is.read(reinterpret_cast<char*>(&buffer_count), sizeof(buffer_count));
  
  record.mesh_data.resize(buffer_count);

  for (auto& e : record.mesh_data){
    is.read(reinterpret_cast<char*>(&e.key), sizeof(e.key));
    is.read(reinterpret_cast<char*>(&e.dimension), sizeof(e.dimension));
    is.read(reinterpret_cast<char*>(&e.size_of_element), sizeof(e.size_of_element));
    is.read(reinterpret_cast<char*>(&e.size), sizeof(e.size));
    is.read(reinterpret_cast<char*>(&e.type), sizeof(e.type));

    size_t byte_count = 0;
    is.read(reinterpret_cast<char*>(&byte_count), sizeof(byte_count));
    
    e.data.resize(byte_count);
    if (byte_count > 0) is.read(reinterpret_cast<char*>(e.data.data()), byte_count);
  };

  return is;
};
