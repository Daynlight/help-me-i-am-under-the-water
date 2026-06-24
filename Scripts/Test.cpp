// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#define SCRIPT_NAME Script_Test
#define SCRIPT_FILE_NAME "Test"
#define BUILDING_SCRIPT_DLL

#include "../UnderTheWater/ScriptShared/GameObjectScriptInterface.h"
#include <cstdio>
#include <math.h>
#include <deque>



namespace UW{
class SCRIPT_NAME : public GameObjectScriptInterface {
private:
  GameObjectData initial_game_data = GameObjectData();

  std::deque<glm::vec3> path;

  glm::vec3 position = {153.0f, 28.0f, -116.0f};
  glm::vec3 scale = {0.5f, 0.5f, 0.5f};
  
  glm::vec3 last_tangent = glm::vec3(0.0f);
  glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  float t = 0.0f;
  float speed = 10.0f;

  unsigned int screen_quad_mesh_id = 0;
  unsigned int meshes_version = -1;

public:
  ~SCRIPT_NAME() = default;
  
  void OnLoad(){
    logger->info("Test Script", "Loaded");
    initial_game_data = *game_object_data;

    int interpolate_points_size = 0;
    auto it1 = game_object_data->parameters.find("interpolate_position_points");
    if (it1 != game_object_data->parameters.end()) {
      if (auto* point_size = std::get_if<int>(&it1->second)) {
        interpolate_points_size = *point_size;
      };
    };
    
    glm::vec3 interpolate_value = glm::vec3(218.0f, 53, -31);
    auto it2 = game_object_data->parameters.find("interpolate_position_0");
    if (it2 != game_object_data->parameters.end()) {
      if (auto* str_ptr = std::get_if<glm::vec3>(&it2->second)) {
        glm::vec3 value = *str_ptr;
          logger->info(SCRIPT_FILE_NAME, "Position[0] = [" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + "]");
      }
    }
    position = interpolate_value;
    path.push_back(interpolate_value);


    for(int i = 1; i <= interpolate_points_size; i++){
      auto it3 = game_object_data->parameters.find("interpolate_position_" + std::to_string(i));
      if (it3 != game_object_data->parameters.end()) {
        if (auto* str_ptr = std::get_if<glm::vec3>(&it3->second)) {
          glm::vec3 value = *str_ptr;
          path.push_back(value);
          logger->info(SCRIPT_FILE_NAME, "Position[" + std::to_string(i) + "] = [" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + "]");
        };
      };
    };
  };
  
  void OnUpdate(float delta_time){
    
  };
  
  void OnFixedUpdate(float fixed_delta_time){
    if (path.size() < 3) return;

    glm::vec3 p0 = path[0];
    glm::vec3 p1 = path[1];
    glm::vec3 p2 = path[2];

    
    glm::vec3 raw_tangent = (4.0f * t - 3.0f) * p0 
      - (8.0f * t - 4.0f) * p1 
      + (4.0f * t - 1.0f) * p2;
    
    glm::vec3 forward = glm::normalize(raw_tangent);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float tangent_len = glm::length(raw_tangent);
    if (tangent_len < 0.001f) tangent_len = 0.001f;

    float dt_pct = (speed * fixed_delta_time) / tangent_len;
    bool segment_swapped = false;

    if (t + dt_pct >= 0.5f) {
      float t_needed = 0.5f - t;
      float dt_used = (t_needed * tangent_len) / speed;
      float dt_remaining = glm::max(0.0f, fixed_delta_time - dt_used);

      t = 0.0f; 
      path.push_back(path.front());
      path.pop_front();

      if (path.size() < 3) return;
      p0 = path[0];
      p1 = path[1];
      p2 = path[2];

      raw_tangent = (4.0f * t - 3.0f) * p0 
        - (8.0f * t - 4.0f) * p1 
        + (4.0f * t - 1.0f) * p2;

      tangent_len = glm::length(raw_tangent);
      if (tangent_len < 0.001f) tangent_len = 0.001f;

      t += (speed * dt_remaining) / tangent_len;
      segment_swapped = true; 
    } else {
      t += dt_pct;
    }

    position = 2.0f * (t - 0.5f) * (t - 1.0f) * p0 
      - 4.0f * t * (t - 1.0f) * p1 
      + 2.0f * t * (t - 0.5f) * p2;

    if (glm::length(raw_tangent) > 0.001f) {
      glm::vec3 current_tangent = glm::normalize(raw_tangent);

      if (glm::length(last_tangent) < 0.1f || segment_swapped) {
        last_tangent = current_tangent;
      }

      glm::vec3 axis = glm::cross(last_tangent, current_tangent);
      float dot_prod = glm::clamp(glm::dot(last_tangent, current_tangent), -1.0f, 1.0f);
      
      if (glm::length(axis) > 0.0001f) {
        float angle = std::acos(dot_prod);
        glm::quat delta_rot = glm::angleAxis(angle, glm::normalize(axis));
        
        orientation = delta_rot * orientation;
        orientation = glm::normalize(orientation);
      }

      last_tangent = current_tangent;
    };

    game_object_data->rotation = glm::eulerAngles(orientation);;
    
    game_object_data->position = position;
  };
  
  void OnRender(){
  };
  
  void OnDestroy(){
    *game_object_data = initial_game_data;
    logger->info("Test Script", "Destroyed");
  };
};
};



#ifndef PRODUCTION

extern "C" UW::GameObjectScriptInterface* SCRIPT_API GetScript() {
  UW::SCRIPT_NAME* script = new UW::SCRIPT_NAME();
  return (UW::GameObjectScriptInterface*)script;
};



extern "C" void SCRIPT_API DeleteScript(UW::GameObjectScriptInterface* script) {
  UW::SCRIPT_NAME* temp_script = (UW::SCRIPT_NAME*)script;
  delete temp_script;
};

#else

REGISTER_SCRIPT(SCRIPT_FILE_NAME, SCRIPT_NAME)

#endif

