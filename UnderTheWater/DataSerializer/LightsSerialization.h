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

#ifndef PRODUCTION
  void save(const std::string& name, const UW::Light& light);
#endif
  void load(const std::string& name, UW::Light& light);

#ifndef PRODUCTION
  void saveAll(UW::Lights& lights);
#endif
  void loadAll(UW::Lights& lights);

private:
#ifndef PRODUCTION
  friend std::ostream& operator<<(std::ostream& os, const LightsRecord& record);
#endif
  friend std::istream& operator>>(std::istream& is, LightsRecord& record);
};
}; // namespace UW
