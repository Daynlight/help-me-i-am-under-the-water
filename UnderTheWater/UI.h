#pragma once
#include "Gui.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "config.h"
#include "ObjectManager.h"
#include "Objects/GameObject.h"
#include "Camera/Camera.h"



namespace UW{
struct GuiSettings{
  bool infoWindowOn = false;
  bool materialExplorerOn = false;
  bool materialEditorOn = false;
  bool shaderExplorerWindowOn = false;
  bool shaderEditorWindowOn = false;
  bool objectExplorerWindowOn = false;
  bool objectEditorWindowOn = false;
  std::string material_name = UW::Config::DEFAULT_GUI_MATERIAL;
  unsigned int object_id = UW::Config::DEFAULT_GUI_OBJECT;
  GLenum shader_type = UW::Config::DEFAULT_GUI_SHADER_TYPE;
  std::string shader_name = UW::Config::DEFAULT_GUI_SHADER;
  int window_width = 800;
  int window_height = 600;
};

static GuiSettings guiSettings;



class UI{
private:
  CW::Gui::Gui gui;
  CW::Renderer::Renderer& window;

  float &fps;
  bool &debug_camera_on;
  UW::Camera &camera;
  UW::Camera &debug_camera;
  UW::ObjectManager& object_manager;

  bool material_is_updated = false;
  bool shader_is_updated = false;
  char buffer[UW::Config::SHADER_EDITOR_BUFFER_SIZE] = {0};

public:
  UI(CW::Renderer::Renderer &window, float &fps, bool &debug_camera_on, UW::Camera &camera, UW::Camera &debug_camera, UW::ObjectManager &object_manager);
  ~UI();
  void onLoad();
  void render();


private:
// gui
  void uiLoad();
  void configControl();
  void uiControl();

  void guiInfo();
  void guiControlsInfo();
  void guiMaterialParameters();
  void guiMaterialList();
  void guiShaderLoad(std::string name, GLenum type);
  std::string getShaderTypeName(GLenum type);
  void guiShaderEditor();
  void guiShaderList();
  void guiObjectEditor();
  void guiObjectList();

  void menuBarGui();

  std::function<void(std::function<void()> render_windows)> appWorkspace();

  std::function<void(CW::Renderer::iRenderer *window)> windowGui();
  std::function<void(CW::Renderer::iRenderer *window)> materialExplorerGui();
  std::function<void(CW::Renderer::iRenderer *window)> materialEditorGui();
  std::function<void(CW::Renderer::iRenderer *window)> shaderExplorerGui();
  std::function<void(CW::Renderer::iRenderer *window)> shaderEditorGui();
  std::function<void(CW::Renderer::iRenderer *window)> objectExplorerGui();
  std::function<void(CW::Renderer::iRenderer *window)> objectEditorGui();
};
};