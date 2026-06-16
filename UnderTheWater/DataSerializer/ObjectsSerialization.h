#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#include "Renderer.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Utils/Logger.h"



namespace UW {
  class GameObject; 
}



namespace UW {
struct GameObjectRecord {
  std::string name = "";
  std::string mesh = "";
  std::string shader = "";
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  std::vector<std::string> textures;
  std::vector<std::string> materials;

  friend std::ostream& operator<<(std::ostream& os, const GameObjectRecord& record);
  friend std::istream& operator>>(std::istream& is, GameObjectRecord& record);
};



class ObjectsSerialization {
public:
  ObjectsSerialization() = default;
  ~ObjectsSerialization() = default;

#ifndef PRODUCTION
  void save(const UW::GameObject& object);
#endif
  void load(UW::GameObject& object);

#ifndef PRODUCTION
  void saveAll(std::vector<UW::GameObject>& objects);
#endif
  void loadAll(std::vector<UW::GameObject>& objects);

private:
#ifndef PRODUCTION
  friend std::ostream& operator<<(std::ostream& os, const GameObjectRecord& record);
#endif
  friend std::istream& operator>>(std::istream& is, GameObjectRecord& record);
};
}; // namespace UW
