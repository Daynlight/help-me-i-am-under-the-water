// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once
#include "Renderer.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>

#include <cmrc/cmrc.hpp>

#include "config.h"
#include "Utils/Logger.h"
#include "Objects/Object.h"
#include "Objects/GameObject.h"
#include "Resources/Materials/Materials.h"

#include "DataSerializer/MeshSerialization.h"
#include "DataSerializer/ObjectsSerialization.h"
#include "DataSerializer/MaterialsSerialization.h"
#include "DataSerializer/LightsSerialization.h"
#include "DataSerializer/ShaderSerialization.h"
#include "DataSerializer/ScriptSerialization.h"



namespace UW {
  class GameObject; 
};



namespace UW{
class DataSerializer{
private:
  DataSerializer() = default;
  ~DataSerializer() = default;

  MeshSerialization mesh_serializer;
  ObjectsSerialization objects_serializer;
  MaterialsSerialization materials_serializer;
  LightsSerialization lights_serializer;
  ShaderSerialization shader_serializer;
  ScriptSerialization script_serializer;

public:
  static DataSerializer& get();

  DataSerializer(const DataSerializer&) = delete;
  DataSerializer& operator=(const DataSerializer&) = delete;
  DataSerializer(DataSerializer&&) = delete;
  DataSerializer& operator=(DataSerializer&&) = delete;

public:
#ifndef PRODUCTION
  void saveAllObjects(std::vector<UW::GameObject> &objects);
#endif
  void loadAllObjects(std::vector<UW::GameObject> &objects);

#ifndef PRODUCTION
  void saveAllMaterials(UW::Materials &materials);
#endif
  void loadAllMaterials(UW::Materials &materials);

#ifndef PRODUCTION
  void saveAllLights(UW::Lights &lights);
#endif
  void loadAllLights(UW::Lights &lights);

#ifndef PRODUCTION
  void saveMesh(const std::string& name, const CW::Renderer::Mesh& mesh);
#endif
  void loadMesh(const std::string& path_to_mesh, UW::Meshes &meshes);

#ifndef PRODUCTION
  void saveAllMeshes(UW::Meshes& meshes);
#endif
  void loadAllMeshes(UW::Meshes& meshes);

#ifndef PRODUCTION
  void saveShaders(const std::string& shader_name, GLuint type);
#endif
  void loadShader(const std::string& shader_name);

#ifndef PRODUCTION
  void saveScript(const std::string& script_name, const std::string& source);
#endif
  std::string loadScript(const std::string& script_name);

  void loadAllTextures();

#ifndef PRODUCTION
  void saveAll(std::vector<UW::GameObject> &objects);
#endif

  void loadAll(std::vector<UW::GameObject> &objects);

};
}; // namespace UW
