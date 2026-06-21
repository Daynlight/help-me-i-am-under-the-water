// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#define SCRIPTNAME Script_Test
#define BUILDING_SCRIPT_DLL

#include "GameObjectScriptInterface.h"
#include "../UnderTheWater/Objects/GameObjectData.h"
#include <cstdio>
#include <math.h>



namespace UW{
class SCRIPTNAME : public GameObjectScriptInterface {
private:
  glm::vec3 initial_position = glm::vec3(0.0f);
  float time = 0;

public:
  ~SCRIPTNAME() = default;
  
  void OnLoad(){
    initial_position = game_object_data->position;
    // printf("OnLoad\n");
  };
  
  void OnUpdate(float delta_time){
    // printf("OnUpdate\n");
  };
  
  void OnFixedUpdate(float fixed_delta_time){
    time += fixed_delta_time;
    game_object_data->position += glm::vec3(sin(time), cos(time), sin(time));
    // printf("OnFixedUpdate\n");
  };
  
  void OnRender(){
    // printf("OnRender\n");
  };
  
  void OnDestroy(){
    // printf("OnDestroy\n");
    game_object_data->position = initial_position;
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
