#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <functional>
#include <memory>
#include <filesystem>
#include <vector>
#include <string>

#include "Utils/Logger.h"

#include "UI/Settings.h"
#include "UI/UI_ScriptEditor.h"



namespace fs = std::filesystem;



namespace UW{
class UI_Scripts{
private:
  CW::Gui::Gui& gui;
  std::vector<std::unique_ptr<UI_ScriptEditor>> script_editors;

public:
  UI_Scripts(CW::Gui::Gui& gui);
  ~UI_Scripts();
  void uiControl();
  void loadScriptEditors();
  void saveScriptEditors();

private:
  std::vector<std::string> getAvailableScripts();
  void guiScriptList();
  inline std::function<void(CW::Renderer::iRenderer *window)> scriptExplorerGui();

};
};



#endif