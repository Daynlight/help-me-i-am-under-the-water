// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#include <string>
#include <filesystem>

#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../Scripts/GameObjectScriptInterface.h"
#include "Utils/Logger.h"
#include "config.h"



namespace UW{
class GameObjectScriptRecord{
  std::filesystem::file_time_type lastWriteTime{};
  std::string path = "";
  std::string so_file = "";
  std::string cpp_file = "";
  bool cant_find_file_print = 1;
  void* script_handler = nullptr; 
  unsigned int version = -1;
  GameObjectScriptInterface* script = nullptr;

public:
  GameObjectScriptRecord(const std::string& path);
  ~GameObjectScriptRecord();

  bool checkLastWrite();
  void updateScript();
  
  int compile();
  int loadModule();
  void removeModule();
  
  void init();
  void update(float delta_time);
  void fixedUpdate(float fixed_delta_time);
  void render();
  void destroy();

  std::string getPath();
  unsigned int getVersion();

};
};
