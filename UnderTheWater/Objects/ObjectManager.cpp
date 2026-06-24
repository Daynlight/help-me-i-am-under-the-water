// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "ObjectManager.h"



UW::ObjectManager &UW::ObjectManager::get(){
  static ObjectManager instance;
  return instance; 
};



void UW::ObjectManager::emplace_back(const std::string &name){
  objects.emplace_back(UW::GameObject(name, "empty", "Default"));
};



void UW::ObjectManager::erase(const std::string &name) {
  for (auto it = objects.begin(); it != objects.end(); ) {
    if (it->game_object_data.name == name) {
      it->onDestroy();
      it->scripts.clear(); 
      it->mesh_id = -1;
      it = objects.erase(it);
      
    } else {
      ++it; 
    };
  };
};



UW::GameObjectData *UW::ObjectManager::getGameObjectData(const std::string &name){
  for(auto& object : objects)
    if(object.game_object_data.name == name)
      return &object.game_object_data;

  return nullptr;
};
