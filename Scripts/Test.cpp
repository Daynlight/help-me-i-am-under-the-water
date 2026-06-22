// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#define SCRIPTNAME Script_Test
#define BUILDING_SCRIPT_DLL

#include "../UnderTheWater/ScriptShared/GameObjectScriptInterface.h"
#include <cstdio>
#include <math.h>



namespace UW{
class SCRIPTNAME : public GameObjectScriptInterface {
private:
  glm::vec3 initial_position = glm::vec3(0.0f);
  glm::vec3 initial_size = glm::vec3(1.0f);
  std::string initial_mesh = "";
  float time = 0;

public:
  ~SCRIPTNAME() = default;
  
  void OnLoad(){
    logger->info("Test Script", "Loaded");
    initial_position = game_object_data->position;
    initial_size = game_object_data->scale;
    initial_mesh = game_object_data->mesh;
    game_object_data->position = glm::vec3(0.0f, 100.0f, 0.0f);
    game_object_data->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // game_object_data->mesh = "Statue";
  };
  
  void OnUpdate(float delta_time){
  };
  
  void OnFixedUpdate(float fixed_delta_time){
    time += fixed_delta_time;
    game_object_data->position += glm::vec3(sin(time) / 2.0f, cos(time)/ 2.0f, 0.0f);
  };
  
  void OnRender(){
  };
  
  void OnDestroy(){
    game_object_data->position = initial_position;
    game_object_data->scale = initial_size;
    game_object_data->mesh = initial_mesh;
    logger->info("Test Script", "Destroyed");
  };
};
};



extern "C" UW::GameObjectScriptInterface* SCRIPT_API GetScript() {
  UW::SCRIPTNAME* script = new UW::SCRIPTNAME();
  return (UW::GameObjectScriptInterface*)script;
};



extern "C" void SCRIPT_API DeleteScript(UW::GameObjectScriptInterface* script) {
  UW::SCRIPTNAME* temp_script = (UW::SCRIPTNAME*)script;
  delete temp_script;
};
