// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#include "../../vendor/CWindow/CWindow/vendor/glm/glm/glm.hpp"
#include "../../vendor/CWindow/CWindow/vendor/glm/glm/gtc/quaternion.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>



namespace UW{
inline constexpr const char* gameObjectParameterTypeName[] = {"int", "float", "bool", "vec2", "vec3", "str"};
using GameObjectParameterType = std::variant<int, float, bool, glm::vec2, glm::vec3, std::string>;



struct GameObjectData{
  std::string name = "";
  std::string mesh = "";
  std::string shader = "";
  std::vector<std::string> textures;
  std::vector<std::string> materials;
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);

  std::unordered_map<std::string, GameObjectParameterType> parameters;
};
};
