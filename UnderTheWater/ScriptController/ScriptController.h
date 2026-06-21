// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#include <string>
#include <filesystem>

#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "ScriptShared/GameObjectScriptInterface.h"
#include "Utils/Logger.h"
#include "config.h"



namespace UW{
class GameObjectData;



class GameObjectScriptRecord{
  std::filesystem::file_time_type lastWriteTime{};
  std::string path = "";
  std::string so_file = "";
  std::string cpp_file = "";
  bool log_observe_lock = 1;

  void* script_handler = nullptr;
  GameObjectScriptInterface* script = nullptr;

public:
  GameObjectScriptRecord(const std::string& path);
  ~GameObjectScriptRecord();

  void observe(GameObjectData* data);
  
  void onLoad(GameObjectData* data);
  void onUpdate(float delta_time);
  void onFixedUpdate(float fixed_delta_time);
  void onRender();
  void onDestroy();

  std::string getPath();

  int loadModule();
  void removeModule();

private:
  bool checkLastWrite();
  void updateScript(GameObjectData* data);

  int compile();

};
};
