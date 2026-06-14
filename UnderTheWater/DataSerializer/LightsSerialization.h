#pragma once

#include "Renderer.h"
#include "Resources/Lights/Lights.h"
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <filesystem>

#include "Utils/Logger.h"
#include "config.h"



namespace UW {
struct LightsRecord {
  std::string name = "";
  glm::vec3 position;
  glm::vec3 color;
  float strength;

  friend std::ostream& operator<<(std::ostream& os, const LightsRecord& record);
  friend std::istream& operator>>(std::istream& is, LightsRecord& record);
};



class LightsSerialization {
public:
  LightsSerialization() = default;
  ~LightsSerialization() = default;

  void save(const std::string& name, const UW::Light& light);
  void load(const std::string& name, UW::Light& light);

  void saveAll(std::unordered_map<std::string, UW::Lights>& lights);
  void loadAll(std::unordered_map<std::string, UW::Lights>& lights);

private:
  friend std::ostream& operator<<(std::ostream& os, const LightsRecord& record);
  friend std::istream& operator>>(std::istream& is, LightsRecord& record);
};
}; // namespace UW
