// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "ScriptController.h"



UW::GameObjectScriptRecord::GameObjectScriptRecord(const std::string &path)
  : path(path), 
    cpp_file(UW::Config::SCRIPTS_SRC_FOLDER + path + ".cpp"),
    so_file(UW::Config::SCRIPTS_DLL_FOLDER + path + ".so") {
};



UW::GameObjectScriptRecord::~GameObjectScriptRecord(){
  removeModule();
};



bool UW::GameObjectScriptRecord::checkLastWrite() {
  bool file_exist = std::filesystem::exists(cpp_file);

  bool changed = 0;

  if(cant_find_file_print && !file_exist){
    UW::Logger::get().erro("Script Controller", "No file named: " + cpp_file);
    cant_find_file_print = 0;
  };
  
  if(!file_exist)
    return 0;
  
  cant_find_file_print = 1;
  std::filesystem::file_time_type currentWriteTime{};

  try{
    currentWriteTime = std::filesystem::last_write_time(cpp_file);
  } catch(const std::filesystem::filesystem_error& e){
    UW::Logger::get().erro("Script Controller", "Filesystem error - " + std::string(e.what()));
    return false;
  };

  if(currentWriteTime != lastWriteTime){
    changed = 1;
    lastWriteTime = currentWriteTime;
    version++;
    Logger::get().info("Script Controller", "Script changed lastWriteTime != currentWriteTime");
  };

  return changed;
};



void UW::GameObjectScriptRecord::updateScript() {
  removeModule();

  if(!compile())
    if(!loadModule())
      init();
};



int UW::GameObjectScriptRecord::compile() {
  const char* command = "g++";
  const char* argv[] = {
    "g++",
    "-rdynamic",
    "-shared", 
    "-fPIC",
    "-o", so_file.c_str(), 
    cpp_file.c_str(), 
    nullptr
  };


  pid_t pid = fork();
  if(pid == 0){
    execvp(command, const_cast<char* const*>(argv)); 
    Logger::get().erro("Script Controller", "Failed to exec g++");
    return -1;
  }
  else if(pid > 0){
    int status = 0; 
    waitpid(pid, &status, 0);
    version++;

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) return 0; 
    else { 
      UW::Logger::get().erro("Script Controller", "Compilation failed!");
      return -1; 
    };
  };

  UW::Logger::get().erro("Script Controller", "Failed to fork()");
  return -1;
};



int UW::GameObjectScriptRecord::loadModule() {
  removeModule();

  bool file_exist = std::filesystem::exists(so_file);
  if(!file_exist) return -1;

  script_handler = dlopen((so_file).c_str(), RTLD_NOW);

  if (!script_handler) {
    Logger::get().erro("Script Controller", "Failed to load script - " + std::string(dlerror()));
    return -1;
  };

  dlerror();

  typedef GameObjectScriptInterface* (*GetScriptFunc)();
  GetScriptFunc getScript = (GetScriptFunc)dlsym(script_handler, "GetScript");
  const char* dlsym_error = dlerror();
  
  if (dlsym_error || !getScript) {
    Logger::get().erro("Script Controller", "Cannot load symbol 'GetScript' - " + std::string(dlsym_error));
    removeModule();
    return -1;
  };

  script = getScript();

  if(!script){
    removeModule();
    return -1;
  }

  Logger::get().info("Script Controller", "Script Loaded");
  return 0;
};



void UW::GameObjectScriptRecord::removeModule(){
  if (!script_handler) {
    script = nullptr;
    return;
  };

  if(script){
    destroy();

    dlerror();
    using DeleteScriptFunc = void (*)(GameObjectScriptInterface*);
    DeleteScriptFunc deleteScript = (DeleteScriptFunc)dlsym(script_handler, "DeleteScript");
    const char* dlsym_error = dlerror();

    if (dlsym_error || !deleteScript) {
    Logger::get().warn("Script Controller", "DeleteScript not found or invalid - " + std::string(dlsym_error ? dlsym_error : "null"));
      script = nullptr;
    }
    else{
      deleteScript(script);
      script = nullptr;
    };
  };

  
  if(script_handler) dlclose(script_handler);
  script_handler = nullptr;
  script = nullptr;
};



void UW::GameObjectScriptRecord::init() {
  if(script){

#ifndef PRODUCTION
#ifdef SANDBOX_SCRIPTS
    pid_t pid = fork();
    if(pid == 0){
      script->OnLoad();

      _exit(0);
    }
    else if(pid > 0){
      int status = 0; 
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { 
        UW::Logger::get().erro("Script Controller", std::to_string(status) + " - Init failed!");
        return; 
      };
    };
#endif
#endif

    script->OnLoad();
  };
};



void UW::GameObjectScriptRecord::update(float delta_time) {
  if(script){

#ifndef PRODUCTION
#ifdef SANDBOX_SCRIPTS
    pid_t pid = fork();
    if(pid == 0){
      script->OnUpdate(delta_time);

      _exit(0);
    }
    else if(pid > 0){
      int status = 0; 
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { 
        UW::Logger::get().erro("Script Controller", std::to_string(status) + " - Update failed!");
        return; 
      };
    };
#endif
#endif

    script->OnUpdate(delta_time);
  };
};



void UW::GameObjectScriptRecord::fixedUpdate(float fixed_delta_time) {
  if(script){

#ifndef PRODUCTION
#ifdef SANDBOX_SCRIPTS
    pid_t pid = fork();
    if(pid == 0){
      script->OnFixedUpdate(fixed_delta_time);

      _exit(0);
    }
    else if(pid > 0){
      int status = 0; 
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { 
        UW::Logger::get().erro("Script Controller", std::to_string(status) + " - FixedUpdate failed!");
        return; 
      };
    };
#endif
#endif

    script->OnFixedUpdate(fixed_delta_time);
  };
};



void UW::GameObjectScriptRecord::render() {
  if(script){

#ifndef PRODUCTION
#ifdef SANDBOX_SCRIPTS
    pid_t pid = fork();
    if(pid == 0){
      script->OnRender();

      _exit(0);
    }
    else if(pid > 0){
      int status = 0; 
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { 
        UW::Logger::get().erro("Script Controller", std::to_string(status) + " - Render failed!");
        return; 
      };
    };
#endif
#endif

    script->OnRender();
  };
};



void UW::GameObjectScriptRecord::destroy() {
  if(script){

#ifndef PRODUCTION
#ifdef SANDBOX_SCRIPTS
    pid_t pid = fork();
    if(pid == 0){
      script->OnDestroy();

      _exit(0);
    }
    else if(pid > 0){
      int status = 0; 
      waitpid(pid, &status, 0);

      if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) { 
        UW::Logger::get().erro("Script Controller", std::to_string(status) + " - Destroy failed!");
        return; 
      };
    };
#endif
#endif

    script->OnDestroy();
  };
};



std::string UW::GameObjectScriptRecord::getPath(){
  return path;
};



unsigned int UW::GameObjectScriptRecord::getVersion(){
  return version;
};
