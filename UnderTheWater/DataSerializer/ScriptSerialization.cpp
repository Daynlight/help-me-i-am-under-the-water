// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "ScriptSerialization.h"



namespace fs = std::filesystem;



#ifndef PRODUCTION
void UW::ScriptSerialization::save(const std::string& script_name, const std::string& source) {
  Logger::get().info("ScriptSerialization", "Saving script: " + UW::Config::SCRIPTS_FOLDER + script_name);
  
  std::string folder_path = UW::Config::SCRIPTS_FOLDER;
  std::string file_path = folder_path + script_name;

  try {
    if (!fs::exists(folder_path)) fs::create_directories(folder_path);

    std::ofstream outFile(file_path);
    if (!outFile.is_open()) {
      Logger::get().erro("ScriptSerialization", "Failed to open file: " + file_path);
      return;
    };

    outFile << source;
    outFile.close();

    Logger::get().info("ScriptSerialization", "Script saved: " + file_path);
  } catch (const fs::filesystem_error& e) {
    Logger::get().erro("ScriptSerialization", "Filesystem error: " + std::string(e.what()));
  };
};
#endif



std::string UW::ScriptSerialization::load(const std::string& script_name) {
  std::string file_path = UW::Config::SCRIPTS_FOLDER + script_name;

  if (!fs::exists(file_path)) {
    Logger::get().warn("ScriptSerialization", "Script file not found: " + file_path);
    return "";
  };

  std::ifstream inFile(file_path);
  if (!inFile.is_open()) {
    Logger::get().erro("ScriptSerialization", "Failed to open file: " + file_path);
    return "";
  };

  std::string source((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
  
  inFile.close();
  return source;
};
