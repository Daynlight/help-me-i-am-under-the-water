// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



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
class ScriptSerialization {
public:
  ScriptSerialization() = default;
  ~ScriptSerialization() = default;
  
#ifndef PRODUCTION
  void save(const std::string& script_name, const std::string& source);
#endif
  std::string load(const std::string& shader_name);
};
}; // namespace UW
