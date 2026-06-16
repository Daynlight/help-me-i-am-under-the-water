#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include "Resources/Resources.h"
#include "Utils/Logger.h"

#include "UI/Settings.h"



namespace UW{
class UI_Materials{
private:
  CW::Gui::Gui& gui;
  bool material_is_updated = false;

public:
  UI_Materials(CW::Gui::Gui& gui);
  ~UI_Materials();
  void uiControl();

private:
  void guiMaterialList();
  inline std::function<void(CW::Renderer::iRenderer *window)> materialExplorerGui();
  
  inline void guiMaterialParameters();
  std::function<void(CW::Renderer::iRenderer *window)> materialEditorGui();
};
};



#endif