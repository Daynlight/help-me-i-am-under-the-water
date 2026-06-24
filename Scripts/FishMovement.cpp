// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#define SCRIPT_NAME Script_FishMovement
#define SCRIPT_FILE_NAME "FishMovement"
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
  
  glm::vec3 last_tangent = glm::vec3(0.0f);
  glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  glm::vec3 up_vector = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 forward_vector = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 rotation_adjustment = glm::vec3(0.0f);
  
  float rotation_speed = 5.0f;
  float speed = 10.0f;
  float t = 0.0f;

  std::vector<std::string> child_objects;


public:
  ~SCRIPT_NAME() = default;
  


  void OnLoad(){
    logger->info("Test Script", "Loaded");
    initial_game_data = *game_object_data;



    // std::string child_name = game_object_data->name + "_child_1";
    // child_objects.emplace_back(child_name);
    // object_manager->emplace_back(child_name);
    // GameObjectData* child_data = object_manager->getGameObjectData(child_name);
    // child_data->mesh = "Pufferfish";
    // child_data->position = {163, 26, -21};


    
    auto itr = game_object_data->parameters.find("rotation_adjustment");
    if (itr != game_object_data->parameters.end()) {
      if (auto* new_rotation = std::get_if<glm::vec3>(&itr->second)) {
        rotation_adjustment = *new_rotation; 
        logger->info(SCRIPT_FILE_NAME, "Adjust rotation = [" + std::to_string(rotation_adjustment.x) + ", " + std::to_string(rotation_adjustment.y) + ", " + std::to_string(rotation_adjustment.z) + "]");
      };
    };



    int interpolate_points_size = 0;
    auto it1 = game_object_data->parameters.find("p_size");
    if (it1 != game_object_data->parameters.end()) {
      if (auto* point_size = std::get_if<int>(&it1->second)) {
        interpolate_points_size = *point_size;
        logger->info(SCRIPT_FILE_NAME, "Position size = " + std::to_string(*point_size));
      };
    };
    
    glm::vec3 interpolate_value = glm::vec3(218.0f, 53, -31);
    auto it2 = game_object_data->parameters.find("p_0");
    if (it2 != game_object_data->parameters.end()) {
      if (auto* str_ptr = std::get_if<glm::vec3>(&it2->second)) {
        glm::vec3 value = *str_ptr;
          logger->info(SCRIPT_FILE_NAME, "Position[0] = [" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + "]");
      };
    };
    game_object_data->position = interpolate_value;
    path.push_back(interpolate_value);

    for(int i = 1; i <= interpolate_points_size; i++){
      auto it3 = game_object_data->parameters.find("p_" + std::to_string(i));
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
    movement(fixed_delta_time);
  };
  


  void OnRender(){
  };
  


  void OnDestroy(){
    *game_object_data = initial_game_data;
    for(const auto& child : child_objects)
      object_manager->erase(child);

    logger->info("Test Script", "Destroyed");
  };



  // ================= //
  // ==== Helpers ==== //
  // ================= //
  void movement(float fixed_delta_time){
    if (path.size() < 3) return;

    glm::vec3 p0 = path[0];
    glm::vec3 p1 = path[1];
    glm::vec3 p2 = path[2];

    glm::vec3 raw_tangent = (4.0f * t - 3.0f) * p0 - (8.0f * t - 4.0f) * p1 + (4.0f * t - 1.0f) * p2;
    
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

    game_object_data->position = 2.0f * (t - 0.5f) * (t - 1.0f) * p0 - 4.0f * t * (t - 1.0f) * p1 + 2.0f * t * (t - 0.5f) * p2;

    applyRotation(raw_tangent, fixed_delta_time);
  };



  void applyRotation(glm::vec3 raw_tangent, float fixed_delta_time){
    if (glm::length(last_tangent) > 0.001f && glm::length(raw_tangent) > 0.001f) raw_tangent = glm::mix(last_tangent, raw_tangent, 0.25f);

    last_tangent = raw_tangent;

    if (glm::length(raw_tangent) > 0.001f) {
      glm::vec3 current_tangent = glm::normalize(raw_tangent);

      glm::vec3 reference_up = up_vector;
      if (glm::abs(glm::dot(current_tangent, reference_up)) > 0.999f) reference_up = glm::vec3(1.0f, 0.0f, 0.0f);

      glm::vec3 target_forward = current_tangent;
      glm::vec3 target_right = glm::normalize(glm::cross(reference_up, target_forward));
      glm::vec3 target_up = glm::cross(target_forward, target_right);
      glm::mat3 target_mat(target_right, target_up, target_forward);
      
      glm::vec3 mesh_forward = glm::normalize(forward_vector);
      glm::vec3 mesh_right = glm::normalize(glm::cross(up_vector, mesh_forward));
      if (glm::length(mesh_right) < 0.001f) mesh_right = glm::vec3(1.0f, 0.0f, 0.0f);
      
      glm::vec3 mesh_up = glm::cross(mesh_forward, mesh_right);
      glm::mat3 mesh_mat(mesh_right, mesh_up, mesh_forward);

      glm::mat3 rot_mat = target_mat * glm::transpose(mesh_mat);
      glm::quat target_orientation = glm::quat_cast(rot_mat);

      float slerp_factor = glm::clamp(fixed_delta_time * rotation_speed, 0.0f, 1.0f);
      orientation = glm::slerp(orientation, target_orientation, slerp_factor);
      orientation = glm::normalize(orientation);
    };

    if (glm::length(raw_tangent) > 0.001f) {
      glm::vec3 smooth_forward = orientation * glm::normalize(forward_vector);
      
      float yaw = atan2(smooth_forward.x, smooth_forward.z);

      game_object_data->rotation = glm::vec3(0.0f, yaw, 0.0f) + rotation_adjustment;
    };
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
