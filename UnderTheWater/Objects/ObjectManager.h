// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include <vector>

#include "config.h"
#include "DataSerializer/DataSerializer.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "ScriptShared/IObjectManger.h"



namespace UW{
class GameObject;



class ObjectManager : public IObjectManager{
private:

public:
  static ObjectManager& get();

  ObjectManager(const ObjectManager&) = delete;
  ObjectManager& operator=(const ObjectManager&) = delete;
  ObjectManager(ObjectManager&&) = delete;
  ObjectManager& operator=(ObjectManager&&) = delete;

private:
  ObjectManager() = default;
  ~ObjectManager() = default;

public:
  std::vector<UW::GameObject> objects;

  void emplace_back(const std::string& name);
  void erase(const std::string& name);
  GameObjectData* getGameObjectData(const std::string& name);

  
};
}; // namespace UW
