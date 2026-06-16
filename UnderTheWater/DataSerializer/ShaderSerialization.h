#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include <cmrc/cmrc.hpp>

#include "Renderer.h"
#include "config.h"
#include "Utils/Logger.h"



namespace UW {
class ShaderSerialization {
public:
  ShaderSerialization() = default;
  ~ShaderSerialization() = default;
  
#ifndef PRODUCTION
  void save(const std::string& shader_name, GLuint type);
#endif
  void load(const std::string& shader_name);

  void loadAll();
};
}; // namespace UW
