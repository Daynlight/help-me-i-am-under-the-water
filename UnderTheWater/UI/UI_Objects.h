#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <functional>

#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "Objects/ObjectManager.h"

#include "UI/Settings.h"
#include "Scene.h"



namespace UW{
class UI_Objects{
private:
  CW::Renderer::Renderer& window;
  CW::Gui::Gui& gui;
  UW::Scene& scene;


public:
  UI_Objects(CW::Gui::Gui& gui, CW::Renderer::Renderer& window, UW::Scene& scene);
  ~UI_Objects();
  void uiControl();

private:
  void guiObjectList();
  std::function<void(CW::Renderer::iRenderer *window)> objectExplorerGui();
  void guiObjectEditor();
  std::function<void(CW::Renderer::iRenderer *window)> objectEditorGui();

};
};



#endif