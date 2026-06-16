#include "UI.h"
#ifndef PRODUCTION

UW::UI::UI(CW::Renderer::Renderer &window, float &fps, bool &debug_camera_on, UW::Camera &camera, UW::Camera &debug_camera, UW::ObjectManager &object_manager)
  :window(window), gui(&window), object_manager(object_manager),
  info_ui(gui, camera, debug_camera, fps, debug_camera_on),
  log_ui(gui),
  materials_ui(gui),
  objects_ui(gui, window, object_manager),
  lights_ui(gui),
  shader_ui(gui),
  asset_loader_ui(gui, object_manager){
  Logger::get().info("UI", "Initializing UI");
  
  gui.setWorkspace(appWorkspace());
};


UW::UI::~UI(){
  Logger::get().info("UI", "Destroying UI");
  shader_ui.saveShaderEditors();
};


void UW::UI::onLoad(){
  Logger::get().info("UI", "Loading UI");

  uiLoad();
  window.setSize(guiSettings.window_width, guiSettings.window_height);
  Logger::get().info("UI", "Window Size Setted { "+ std::to_string(guiSettings.window_width) + " x " + std::to_string(guiSettings.window_height) +" }");
};


void UW::UI::render(){
  gui.render();
};


// ========================= //
// ========== GUI ========== //
// ========================= //
void UW::UI::uiLoad(){
  configControl();
  ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);
  Logger::get().info("UI", "Loading UI Data from disck");

  shader_ui.loadShaderEditors();
  uiControl();
};


void UW::UI::configControl(){
  ImGuiSettingsHandler handler;
  handler.TypeName = "GuiSettings";
  handler.TypeHash = ImHashStr("GuiSettings");

  handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler*, const char*){
    return (void*)&UW::guiSettings;
  };

  handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line){
    GuiSettings* s = (GuiSettings*)entry;

    int value;
    if (sscanf(line, "InfoWindowOn=%d", &value) == 1) s->infoWindowOn = value;
    if (sscanf(line, "LogWindowOn=%d", &value) == 1) s->logWindowOn = value;
    if (sscanf(line, "MaterialExplorerOn=%d", &value) == 1) s->materialExplorerOn = value;
    if (sscanf(line, "LightsExplorerOn=%d", &value) == 1) s->lightsExplorerOn = value;
    if (sscanf(line, "MaterialEditorOn=%d", &value) == 1) s->materialEditorOn = value;
    if (sscanf(line, "ShaderExplorerWindowOn=%d", &value) == 1) s->shaderExplorerWindowOn = value;
    if (sscanf(line, "ShaderEditorWindowOn=%d", &value) == 1) s->shaderEditorWindowOn = value;
    if (sscanf(line, "ObjectExplorerWindowOn=%d", &value) == 1) s->objectExplorerWindowOn = value;
    if (sscanf(line, "ObjectEditorWindowOn=%d", &value) == 1) s->objectEditorWindowOn = value;
    if (sscanf(line, "Object_ID=%d", &value) == 1) s->object_id = value;
    if (sscanf(line, "Mesh_Mode_On=%d", &value) == 1) s->mesh_mode_on = value;
    if (sscanf(line, "Window_Width=%d", &value) == 1) s->window_width = value;
    if (sscanf(line, "Window_Height=%d", &value) == 1) s->window_height = value;
    
    char value_str[256];
    if (sscanf(line, "Material_ID=%256s", &value_str) == 1) s->material_name = std::string(value_str);
    
    char name[256];
    unsigned int type;

    if (sscanf(line, "ShaderEditor=%255[^,],%u", name, &type) == 2){
      s->shader_editors_reg.emplace_back(name, type);
    };
  };

  handler.WriteAllFn = [](ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf){
    out_buf->appendf("[%s][Main]\n", handler->TypeName);
    out_buf->appendf("InfoWindowOn=%d\n", guiSettings.infoWindowOn);
    out_buf->appendf("LogWindowOn=%d\n", guiSettings.logWindowOn);
    out_buf->appendf("MaterialExplorerOn=%d\n", guiSettings.materialExplorerOn);
    out_buf->appendf("LightsExplorerOn=%d\n", guiSettings.lightsExplorerOn);
    out_buf->appendf("MaterialEditorOn=%d\n", guiSettings.materialEditorOn);
    out_buf->appendf("ShaderExplorerWindowOn=%d\n", guiSettings.shaderExplorerWindowOn);
    out_buf->appendf("ShaderEditorWindowOn=%d\n", guiSettings.shaderEditorWindowOn);
    out_buf->appendf("ObjectExplorerWindowOn=%d\n", guiSettings.objectExplorerWindowOn);
    out_buf->appendf("ObjectEditorWindowOn=%d\n", guiSettings.objectEditorWindowOn);
    out_buf->appendf("Object_ID=%d\n", guiSettings.object_id);
    out_buf->appendf("Mesh_Mode_On=%d\n", guiSettings.mesh_mode_on);
    out_buf->appendf("Window_Width=%d\n", guiSettings.window_width);
    out_buf->appendf("Window_Height=%d\n", guiSettings.window_height);
    out_buf->appendf("Material_ID=%s\n", guiSettings.material_name.c_str());
    
    out_buf->appendf("ShaderEditorCount=%zu\n", guiSettings.shader_editors_reg.size());

    for (size_t i = 0; i < guiSettings.shader_editors_reg.size(); ++i){
      out_buf->appendf(
        "ShaderEditor=%s,%u\n",
        guiSettings.shader_editors_reg[i].first.c_str(),
        guiSettings.shader_editors_reg[i].second
      );
    };

    out_buf->append("\n");
  };

  ImGui::GetCurrentContext()->SettingsHandlers.push_back(handler);
};


void UW::UI::uiControl(){
  info_ui.uiControl();
  log_ui.uiControl();
  materials_ui.uiControl();
  objects_ui.uiControl();
  lights_ui.uiControl();
  shader_ui.uiControl();
  asset_loader_ui.uiControl();
};



void UW::UI::menuBarGui(){
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Window")) {
      if(ImGui::MenuItem("Info")){
        guiSettings.infoWindowOn = !guiSettings.infoWindowOn;
        uiControl();
      };
      if(ImGui::MenuItem("Logs")){
        guiSettings.logWindowOn = !guiSettings.logWindowOn;
        uiControl();
      };
      if(ImGui::MenuItem("Material Explorer")){
        guiSettings.materialExplorerOn = !guiSettings.materialExplorerOn;
        uiControl();
      };
      if(ImGui::MenuItem("Material Editor")){
        guiSettings.materialEditorOn = !guiSettings.materialEditorOn;
        uiControl();
      };
      if(ImGui::MenuItem("Lights Explorer")){
        guiSettings.lightsExplorerOn = !guiSettings.lightsExplorerOn;
        uiControl();
      };
      if(ImGui::MenuItem("Shader Explorer")){
        guiSettings.shaderExplorerWindowOn = !guiSettings.shaderExplorerWindowOn;
        uiControl();
      };
      if(ImGui::MenuItem("Object Explorer")){
        guiSettings.objectExplorerWindowOn = !guiSettings.objectExplorerWindowOn;
        uiControl();
      };
      if(ImGui::MenuItem("Object Editor")){
        guiSettings.objectEditorWindowOn = !guiSettings.objectEditorWindowOn;
        uiControl();
      };
      ImGui::EndMenu();
    };

    if(ImGui::BeginMenu("Assets")){
      if(ImGui::MenuItem("Asset Loader")){
        guiSettings.assetLoaderWindowOn = !guiSettings.assetLoaderWindowOn;
        uiControl();
      };
      ImGui::EndMenu();
    };
    ImGui::EndMenuBar();
  };
};


std::function<void(std::function<void()> render_windows)> UW::UI::appWorkspace() {
  return [this](std::function<void()> render_windows){
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                        
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); 
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    ImGui::Begin("Window DockSpace", nullptr, window_flags);
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();

    menuBarGui();

    ImGuiID docspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(docspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    render_windows();

    ImGui::End();
  };
};



#endif
