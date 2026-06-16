#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <string>

#include "config.h"
#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "DataSerializer/DataSerializer.h"



namespace UW{
class UI_ShaderEditor{
private:
  CW::Gui::Gui& gui;
  bool shader_is_loaded = false;
  bool shader_is_updated = false;
  char buffer[UW::Config::SHADER_EDITOR_BUFFER_SIZE] = {0};
  std::string shader_name = "";
  GLenum shader_type = 0;

public:
  UI_ShaderEditor(CW::Gui::Gui& gui, const std::string& name, GLenum type);
  ~UI_ShaderEditor();

  void guiShaderLoad(const std::string& name, GLenum type);
  void guiShaderEditor();
  std::function<void(CW::Renderer::iRenderer *window)> shaderEditorGui();

  std::string getName();
  GLenum getType();
};
};

#endif