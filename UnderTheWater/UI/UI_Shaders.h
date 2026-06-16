#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <functional>
#include <memory>

#include "Utils/Logger.h"
#include "Resources/Resources.h"

#include "UI/Settings.h"
#include "UI/UI_ShaderEditors.h"



namespace UW{
class UI_Shaders{
private:
  CW::Gui::Gui& gui;
  std::vector<std::unique_ptr<UI_ShaderEditor>> shader_editors;

public:
  UI_Shaders(CW::Gui::Gui& gui);
  ~UI_Shaders();
  void uiControl();
  void loadShaderEditors();
  void saveShaderEditors();

private:
  void guiShaderList();
  inline std::function<void(CW::Renderer::iRenderer *window)> shaderExplorerGui();

};
};



#endif