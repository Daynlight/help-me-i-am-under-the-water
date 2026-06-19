#pragma once

#ifndef PRODUCTION
#include <memory>
#include <vector>
#include <algorithm>

#include "Gui.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "config.h"
#include "Utils/Logger.h"
#include "Objects/ObjectManager.h"
#include "DataSerializer/DataSerializer.h"
#include "Objects/GameObject.h"
#include "Camera/Camera.h"
#include "Scene.h"

#include "UI/Settings.h"
#include "UI/UI_AssetLoader.h"
#include "UI/UI_ShaderEditors.h"
#include "UI/UI_Info.h"
#include "UI/UI_Log.h"
#include "UI/UI_Materials.h"
#include "UI/UI_Objects.h"
#include "UI/UI_Lights.h"
#include "UI/UI_Shaders.h"



namespace UW{
class UI{
private:
  CW::Gui::Gui gui;
  CW::Renderer::Renderer& window;

  UW::Scene& scene;

  UW::UI_AssetLoader asset_loader_ui;
  UW::UI_Info info_ui;
  UW::UI_Log log_ui;
  UW::UI_Materials materials_ui;
  UW::UI_Objects objects_ui;
  UW::UI_Lights lights_ui;
  UW::UI_Shaders shader_ui;

public:
  UI(CW::Renderer::Renderer &window, float &fps, UW::Scene& scene);
  ~UI();
  void onLoad();
  void render();


private:
// gui
  void uiLoad();
  void configControl();
  void uiControl();
  void menuBarGui();

  std::function<void(std::function<void()> render_windows)> appWorkspace();
};
};

#endif