#pragma once
#include "Renderer.h"

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "config.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"



namespace UW{
struct GameObjectRecord{
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



class ObjectManager{
private:

public:
  std::vector<UW::GameObject> objects;

  void save();
  void load();
};
}; // namespace UW
