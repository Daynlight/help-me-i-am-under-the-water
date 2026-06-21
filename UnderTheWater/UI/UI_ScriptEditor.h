// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <string>
#include <chrono>

#include "config.h"
#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "DataSerializer/DataSerializer.h"



namespace UW{
class UI_ScriptEditor{
private:
  CW::Gui::Gui& gui;
  bool script_is_loaded = false;
  bool script_is_updated = false;
  char buffer[UW::Config::SCRIPT_EDITOR_BUFFER_SIZE] = {0};
  std::string script_name = "";
  std::chrono::steady_clock::time_point last_save_time;
  std::chrono::steady_clock::duration save_cooldown_duration;

public:
  UI_ScriptEditor(CW::Gui::Gui& gui, const std::string& name);
  ~UI_ScriptEditor();

  void guiScriptLoad(const std::string& name);
  void guiScriptEditor();
  std::function<void(CW::Renderer::iRenderer *window)> ScriptEditorGui();

  std::string getName();
};
};

#endif
