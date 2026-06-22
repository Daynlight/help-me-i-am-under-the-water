// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include <vector>
#include <string>

#include "config.h"



namespace UW{
struct GuiSettings{
  bool infoWindowOn = false;
  bool logWindowOn = false;
  bool materialExplorerOn = false;
  bool materialEditorOn = false;
  bool shaderExplorerWindowOn = false;
  bool scriptsExplorerWindowOn = false;
  bool shaderEditorWindowOn = false;
  bool scriptEditorWindowOn = false;
  bool objectExplorerWindowOn = false;
  bool objectEditorWindowOn = false;
  bool mesh_mode_on = false;
  bool assetLoaderWindowOn = false;
  bool lightsExplorerOn = false;
  std::string material_name = UW::Config::DEFAULT_GUI_MATERIAL;
  unsigned int object_id = UW::Config::DEFAULT_GUI_OBJECT;
  std::vector<std::pair<std::string, GLenum>> shader_editors_reg;
  std::vector<std::string> scripts_editors_reg;
  int window_width = 800;
  int window_height = 600;
};

inline GuiSettings guiSettings;

};
#endif
