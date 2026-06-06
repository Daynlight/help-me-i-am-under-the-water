#include "ObjectManager.h"
#include <iostream>



std::ostream& UW::operator<<(std::ostream& os, const UW::GameObjectRecord& record) {
  os << record.name << "\n" 
     << record.mesh << "\n" 
     << record.shader << "\n"
     << record.position.x << " " << record.position.y << " " << record.position.z << "\n"
     << record.rotation.x << " " << record.rotation.y << " " << record.rotation.z << "\n"
     << record.scale.x << " " << record.scale.y << " " << record.scale.z << "\n";

  os << record.textures.size() << "\n";
  for (const auto& tex : record.textures) os << tex << "\n";

  os << record.materials.size() << "\n";
  for (const auto& mat : record.materials) os << mat << "\n";

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::GameObjectRecord& record) {
  if (!std::getline(is, record.name) || 
      !std::getline(is, record.mesh) || 
      !std::getline(is, record.shader)) return is;

  if (!(is >> record.position.x >> record.position.y >> record.position.z >>
              record.rotation.x >> record.rotation.y >> record.rotation.z >>
              record.scale.x >> record.scale.y >> record.scale.z)) return is;

  size_t texSize = 0;
  if (!(is >> texSize)) return is;
  is.ignore();
  
  if (texSize > 10000) { 
    is.setstate(std::ios::failbit);
    return is;
  };

  record.textures.resize(texSize);
  for (size_t i = 0; i < texSize; ++i) std::getline(is, record.textures[i]);

  size_t matSize = 0;
  if (!(is >> matSize)) return is;
  is.ignore();
  
  if (matSize > 10000) {
    is.setstate(std::ios::failbit);
    return is;
  };

  record.materials.resize(matSize);
  for (size_t i = 0; i < matSize; ++i) std::getline(is, record.materials[i]);

  return is;
};



void UW::ObjectManager::save() {
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME << std::endl;
    return;
  };

  outFile << objects.size() << "\n";

  for (const auto& object : objects) {
    UW::GameObjectRecord record; 
    
    record.name = object.name;
    record.mesh = object.mesh;
    record.shader = object.shader;
    record.position = object.position;
    record.rotation = object.rotation;
    record.scale = object.scale;
    record.textures = object.textures;
    record.materials = object.materials;

    outFile << record;
  };
  
  outFile.close();
};



void UW::ObjectManager::load() {
  std::ifstream inFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
  if (!inFile.is_open()) {
    std::cerr << "Failed to open file for loading: " << UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME << std::endl;
    return;
  };

  objects.clear();

  size_t objectCount = 0;
  if (!(inFile >> objectCount)) return;
  inFile.ignore();

  for (size_t i = 0; i < objectCount; ++i) {
    UW::GameObjectRecord record;
    if (inFile >> record) {
      GameObject object(record.name, record.mesh, record.shader);
      object.position = record.position;
      object.rotation = record.rotation;
      object.scale = record.scale;
      object.textures = std::move(record.textures);
      object.materials = std::move(record.materials);

      objects.push_back(std::move(object));
    } else {
      std::cerr << "Error: File format corrupted at object index " << i << std::endl;
      break;
    };
  };

  inFile.close();
};
