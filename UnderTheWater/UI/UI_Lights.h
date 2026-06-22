// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#pragma once

#ifndef PRODUCTION
#include "Renderer.h"
#include "Gui.h"

#include <functional>

#include "config.h"
#include "Utils/Logger.h"
#include "Resources/Resources.h"
#include "DataSerializer/DataSerializer.h"

#include "UI/Settings.h"



namespace UW{
class UI_Lights{
private:
  CW::Gui::Gui& gui;

public:
  UI_Lights(CW::Gui::Gui& gui);
  ~UI_Lights();
  void uiControl();

private:
  void guiLights();
  std::function<void(CW::Renderer::iRenderer *window)> ui();
};
};

#endif
