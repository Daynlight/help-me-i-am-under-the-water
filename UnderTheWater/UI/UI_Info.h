// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#ifndef PRODUCTIN
#include "Renderer.h"
#include "Gui.h"

#include <functional>

#include "UI/Settings.h"
#include "Utils/Logger.h"
#include "Camera/Camera.h"
#include "Scene.h"


namespace UW{
class UI_Info{
private:
  float &fps;
  bool mesh_mode_is_updated = false;
  UW::Scene& scene;

  CW::Gui::Gui& gui;

public:
  UI_Info(CW::Gui::Gui& gui, float &fps, UW::Scene& scene);
  ~UI_Info();
  void uiControl();

private:
  void guiInfo(); 
  void guiControlsInfo();
  std::function<void(CW::Renderer::iRenderer *window)> ui();
};
};

#endif
