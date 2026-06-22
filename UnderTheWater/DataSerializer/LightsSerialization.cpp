// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "LightsSerialization.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



#ifndef PRODUCTION
void UW::LightsSerialization::save(const std::string& name, const UW::Light& light) {

};
#endif



void UW::LightsSerialization::load(const std::string& name, UW::Light& light) {

};



#ifndef PRODUCTION
void UW::LightsSerialization::saveAll(UW::Lights& lights) {
  Logger::get().info("LightsSerialization", "Saving all lights...");
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("LightsSerialization", "Filesystem error - " + std::string(e.what()));
    return;
  }

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("LightsSerialization", "Failed to open file for saving");
    return;
  }

  unsigned int size = lights.size();

  outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

  for(int i = 0; i < lights.size(); i++){
    UW::Light light = lights.get(i);
    UW::LightsRecord record;
      
    record.position = light.position;
    record.color = light.color;
    record.strength = light.strength;

    outFile << record;
  };

  outFile.close();
  Logger::get().info("LightsSerialization", "All lights have been saved");
};
#endif



void UW::LightsSerialization::loadAll(UW::Lights& lights) {
  Logger::get().info("LightsSerialization", "Loading all lights...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("LightsSerialization", "CMRC - File not found - " + resourcePath);
      return;
    }

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);

    unsigned int lightCount = 0;
    inFile.read(reinterpret_cast<char*>(&lightCount), sizeof(lightCount));

    lights.clear();

    for (unsigned int i = 0; i < lightCount; ++i) {
      UW::LightsRecord record;
      if (inFile >> record) {
        UW::Light light(record.position, record.color, record.strength);
        lights.emplace_back(light);
      } else {
        Logger::get().erro("LightsSerialization", "File format corrupted at index " + std::to_string(i));
        break;
      };
    };

    lights.compile();

    Logger::get().info("LightsSerialization", "All lights have been loaded");
  } catch(const std::exception& e) {
    Logger::get().erro("LightsSerialization", "Exception - " + std::string(e.what()));
  };
};



#ifndef PRODUCTION
std::ostream& UW::operator<<(std::ostream& os, const UW::LightsRecord& record) {
  os.write(reinterpret_cast<const char*>(&record.position), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.color), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.strength), sizeof(float));

  return os;
};
#endif



std::istream& UW::operator>>(std::istream& is, UW::LightsRecord& record) {
  is.read(reinterpret_cast<char*>(&record.position), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.color), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.strength), sizeof(float));

  return is;
};
