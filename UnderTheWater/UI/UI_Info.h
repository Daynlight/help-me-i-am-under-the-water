#pragma once

#ifndef PRODUCTIN
#include "Renderer.h"
#include "Gui.h"

#include <functional>

#include "UI/Settings.h"
#include "Utils/Logger.h"
#include "Camera/Camera.h"



namespace UW{
class UI_Info{
private:
  float &fps;
  bool &debug_camera_on;
  bool mesh_mode_is_updated = false;
  UW::Camera &camera;
  UW::Camera &debug_camera;

  CW::Gui::Gui& gui;

public:
  UI_Info(CW::Gui::Gui& gui, UW::Camera &camera, UW::Camera &debug_camera, float &fps, bool &debug_camera_on);
  ~UI_Info();
  void uiControl();

private:
  void guiInfo(); 
  void guiControlsInfo();
  std::function<void(CW::Renderer::iRenderer *window)> ui();
};
};



#endif