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

  void save(const std::string& shader_name, GLuint type);
  void load(const std::string& shader_name);
};
}; // namespace UW
