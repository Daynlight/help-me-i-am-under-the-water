// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <functional>

#include "Utils/Logger.h"

#include "UI/Settings.h"



namespace UW{
class UI_Log{
private:
  CW::Gui::Gui& gui;

public:
  UI_Log(CW::Gui::Gui& gui);
  ~UI_Log();
  void uiControl();

private:
  void guiLogs();
  std::function<void(CW::Renderer::iRenderer *window)> ui();

};
};

#endif
