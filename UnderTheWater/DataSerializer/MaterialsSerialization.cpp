// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "MaterialsSerialization.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



#ifndef PRODUCTION
void UW::MaterialsSerialization::save(const UW::Material& material) {
  
};
#endif



void UW::MaterialsSerialization::load(UW::Material& material) {
  
};



#ifndef PRODUCTION
void UW::MaterialsSerialization::saveAll(UW::Materials& materials) {
  Logger::get().info("MaterialsSerialization", "Saving all materials...");
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("MaterialsSerialization", "Filesystem error - " + std::string(e.what()));
    return;
  }

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("MaterialsSerialization", "Failed to open file for saving");
    return;
  };

  size_t mat_size = materials.size();
  outFile.write(reinterpret_cast<const char*>(&mat_size), sizeof(mat_size));

  for (auto& el : materials.getMaterialReg()) {
    UW::MaterialsRecord record;
    UW::Material material = el.second;

    record.name = el.first;
    record.albedo = material.albedo;
    record.metallic = material.metallic;
    record.roughness = material.roughness;
    record.emission_color = material.emission_color;
    record.emission_strength = material.emission_strength;
    record.ambient_occlusion = material.ambient_occlusion;

    outFile << record;
    Logger::get().info("MaterialsSerialization", "Material saved { " + el.first + " }");
  };

  outFile.close();
  Logger::get().info("MaterialsSerialization", "All Materials Had Been Saved");
};
#endif



void UW::MaterialsSerialization::loadAll(UW::Materials& materials) {
  Logger::get().info("MaterialsSerialization", "Loading all materials...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("MaterialsSerialization", "CMRC - File not found - " + resourcePath);
      return;
    }

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);

    materials.clear();

    size_t materialCount = 0;
    inFile.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

    for (size_t i = 0; i < materialCount; ++i) {
      UW::MaterialsRecord record;
      if (inFile >> record) {
        Material material;
        material.albedo = record.albedo;
        material.metallic = record.metallic;
        material.roughness = record.roughness;
        material.emission_color = record.emission_color;
        material.emission_strength = record.emission_strength;
        material.ambient_occlusion = record.ambient_occlusion;

        materials.emplace_back(record.name, std::move(material));
        Logger::get().info("MaterialsSerialization", "Material loaded { " + record.name + " }");
      } else {
        Logger::get().erro("MaterialsSerialization", "File format corrupted at index " + std::to_string(i));
        break;
      };
    };
    Logger::get().info("MaterialsSerialization", "All materials have been loaded");
  } catch(const std::exception& e) {
    Logger::get().erro("MaterialsSerialization", "Exception - " + std::string(e.what()));
  };
};



#ifndef PRODUCTION
std::ostream& UW::operator<<(std::ostream& os, const UW::MaterialsRecord& record) {
  size_t name_sz = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_sz), sizeof(size_t));
  if(name_sz > 0) os.write(reinterpret_cast<const char*>(record.name.data()), name_sz);

  os.write(reinterpret_cast<const char*>(&record.albedo), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.metallic), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.roughness), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.emission_color), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.emission_strength), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.ambient_occlusion), sizeof(float));

  return os;
};
#endif



std::istream& UW::operator>>(std::istream& is, UW::MaterialsRecord& record) {
  size_t name_sz = 0;
  if (!is.read(reinterpret_cast<char*>(&name_sz), sizeof(name_sz))) return is;
  record.name.resize(name_sz);
  if (name_sz > 0) is.read(&record.name[0], name_sz);

  is.read(reinterpret_cast<char*>(&record.albedo), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.metallic), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.roughness), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.emission_color), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.emission_strength), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.ambient_occlusion), sizeof(float));

  return is;
};
