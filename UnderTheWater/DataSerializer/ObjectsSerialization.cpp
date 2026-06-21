#include "ObjectsSerialization.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



#ifndef PRODUCTION
void UW::ObjectsSerialization::save(const UW::GameObject& object) {
  Logger::get().info("ObjectsSerialization", "Saving object: " + object.name);
  
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("ObjectsSerialization", "Filesystem error - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME, std::ios::binary | std::ios::app);
  if (!outFile.is_open()) {
    Logger::get().erro("ObjectsSerialization", "Failed to open file for saving");
    return;
  };

  UW::GameObjectRecord record;
  record.name = object.name;
  record.mesh = object.mesh;
  record.shader = object.shader;
  record.position = object.game_object_data.position;
  record.rotation = object.game_object_data.rotation;
  record.scale = object.game_object_data.scale;
  record.textures = object.textures;
  record.materials = object.materials;
  for(auto script : object.scripts) record.scripts.emplace_back(script.getPath());

  outFile << record;
  outFile.close();

  Logger::get().info("ObjectsSerialization", "Object saved { " + object.name + " }");
};
#endif



void UW::ObjectsSerialization::load(UW::GameObject& object) {
  // Single object load would need file path parameter
};



#ifndef PRODUCTION
void UW::ObjectsSerialization::saveAll(std::vector<UW::GameObject>& objects) {
  Logger::get().info("ObjectsSerialization", "Saving all objects...");
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("ObjectsSerialization", "Filesystem error - " + std::string(e.what()));
    return;
  }

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("ObjectsSerialization", "Failed to open file for saving");
    return;
  };

  size_t obj_size = objects.size();
  outFile.write(reinterpret_cast<const char*>(&obj_size), sizeof(obj_size));

  for (const auto& object : objects) {
    UW::GameObjectRecord record;
    record.name = object.name;
    record.mesh = object.mesh;
    record.shader = object.shader;
    record.position = object.game_object_data.position;
    record.rotation = object.game_object_data.rotation;
    record.scale = object.game_object_data.scale;
    record.textures = object.textures;
    record.materials = object.materials;
    for(auto script : object.scripts) record.scripts.emplace_back(script.getPath());

    outFile << record;
    Logger::get().info("ObjectsSerialization", "Object saved { " + object.name + " }");
  };

  outFile.close();
  Logger::get().info("ObjectsSerialization", "All Objects Had Been Saved");
};
#endif



void UW::ObjectsSerialization::loadAll(std::vector<UW::GameObject>& objects) {
  Logger::get().info("ObjectsSerialization", "Loading all objects...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("ObjectsSerialization", "CMRC - File not found - " + resourcePath);
      return;
    };

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);
    
    objects.clear();

    size_t objectCount = 0;
    inFile.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    for (size_t i = 0; i < objectCount; ++i) {
      UW::GameObjectRecord record;
      if (inFile >> record) {
        GameObject object(record.name, record.mesh, record.shader);
        object.game_object_data.position = record.position;
        object.game_object_data.rotation = record.rotation;
        object.game_object_data.scale = record.scale;
        object.textures = std::move(record.textures);
        object.materials = std::move(record.materials);
        for(auto& script : record.scripts) object.scripts.emplace_back(script);

        objects.push_back(std::move(object));
        Logger::get().info("ObjectsSerialization", "Object loaded { " + record.name + " }");
      } else {
        Logger::get().erro("ObjectsSerialization", "File format corrupted at index " + std::to_string(i));
        break;
      };
    };
    Logger::get().info("ObjectsSerialization", "All objects have been loaded");
  } catch(const std::exception& e) {
    Logger::get().erro("ObjectsSerialization", "Exception - " + std::string(e.what()));
  };
};



#ifndef PRODUCTION
std::ostream& UW::operator<<(std::ostream& os, const UW::GameObjectRecord& record) {
  size_t name_sz = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_sz), sizeof(name_sz));
  if (name_sz > 0) os.write(record.name.data(), name_sz);

  size_t mesh_sz = record.mesh.size();
  os.write(reinterpret_cast<const char*>(&mesh_sz), sizeof(mesh_sz));
  if (mesh_sz > 0) os.write(record.mesh.data(), mesh_sz);

  size_t shader_sz = record.shader.size();
  os.write(reinterpret_cast<const char*>(&shader_sz), sizeof(shader_sz));
  if (shader_sz > 0) os.write(record.shader.data(), shader_sz);

  os.write(reinterpret_cast<const char*>(&record.position), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.rotation), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.scale), sizeof(glm::vec3));

  size_t tex_count = record.textures.size();
  os.write(reinterpret_cast<const char*>(&tex_count), sizeof(tex_count));
  for (const auto& tex : record.textures) {
    size_t tex_sz = tex.size();
    os.write(reinterpret_cast<const char*>(&tex_sz), sizeof(tex_sz));
    if (tex_sz > 0) os.write(tex.data(), tex_sz);
  };

  size_t mat_count = record.materials.size();
  os.write(reinterpret_cast<const char*>(&mat_count), sizeof(mat_count));
  for (const auto& mat : record.materials) {
    size_t mat_sz = mat.size();
    os.write(reinterpret_cast<const char*>(&mat_sz), sizeof(mat_sz));
    if (mat_sz > 0) os.write(mat.data(), mat_sz);
  };

  size_t scr_count = record.scripts.size();
  os.write(reinterpret_cast<const char*>(&scr_count), sizeof(scr_count));
  for (const auto& scr : record.scripts) {
    size_t scr_sz = scr.size();
    os.write(reinterpret_cast<const char*>(&scr_sz), sizeof(scr_sz));
    if (scr_sz > 0) os.write(scr.data(), scr_sz);
  };

  return os;
};
#endif



std::istream& UW::operator>>(std::istream& is, UW::GameObjectRecord& record) {
  size_t name_sz = 0;
  if (!is.read(reinterpret_cast<char*>(&name_sz), sizeof(name_sz))) return is;
  record.name.resize(name_sz);
  if (name_sz > 0) is.read(&record.name[0], name_sz);

  size_t mesh_sz = 0;
  is.read(reinterpret_cast<char*>(&mesh_sz), sizeof(mesh_sz));
  record.mesh.resize(mesh_sz);
  if (mesh_sz > 0) is.read(&record.mesh[0], mesh_sz);

  size_t shader_sz = 0;
  is.read(reinterpret_cast<char*>(&shader_sz), sizeof(shader_sz));
  record.shader.resize(shader_sz);
  if (shader_sz > 0) is.read(&record.shader[0], shader_sz);

  is.read(reinterpret_cast<char*>(&record.position), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.rotation), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.scale), sizeof(glm::vec3));

  size_t tex_count = 0;
  is.read(reinterpret_cast<char*>(&tex_count), sizeof(tex_count));
  
  if (tex_count > 10000) { 
    is.setstate(std::ios::failbit);
    return is;
  };

  record.textures.resize(tex_count);
  for (auto& tex : record.textures) {
    size_t tex_sz = 0;
    is.read(reinterpret_cast<char*>(&tex_sz), sizeof(tex_sz));
    tex.resize(tex_sz);
    if (tex_sz > 0) is.read(&tex[0], tex_sz);
  }

  size_t mat_count = 0;
  is.read(reinterpret_cast<char*>(&mat_count), sizeof(mat_count));
  
  if (mat_count > 10000) {
    is.setstate(std::ios::failbit);
    return is;
  };

  record.materials.resize(mat_count);
  for (auto& mat : record.materials) {
    size_t mat_sz = 0;
    is.read(reinterpret_cast<char*>(&mat_sz), sizeof(mat_sz));
    mat.resize(mat_sz);
    if (mat_sz > 0) is.read(&mat[0], mat_sz);
  };

  size_t scr_count = 0;
  is.read(reinterpret_cast<char*>(&scr_count), sizeof(scr_count));
  
  if (scr_count > 10000) {
    is.setstate(std::ios::failbit);
    return is;
  };

  record.scripts.resize(scr_count);
  for (auto& scr : record.scripts) {
    size_t scr_sz = 0;
    is.read(reinterpret_cast<char*>(&scr_sz), sizeof(scr_sz));
    scr.resize(scr_sz);
    if (scr_sz > 0) is.read(&scr[0], scr_sz);
  };

  return is;
};
