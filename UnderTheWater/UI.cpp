#include "UI.h"



UW::ShaderEditor::ShaderEditor(CW::Gui::Gui& gui, UW::DataSerializer& serializer, std::string name, GLenum type)
  :gui(gui), serializer(serializer), shader_name(name), shader_type(type){
  gui.addWindow("Shader Editor " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type], shaderEditorGui());
};



UW::ShaderEditor::~ShaderEditor(){
  gui.deleteWindow("Shader Editor " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type]);
};



void UW::ShaderEditor::guiShaderLoad(std::string name, GLenum type){
  shader_name = name;
  shader_type = type;
  memset(buffer, '\0', UW::Config::SHADER_EDITOR_BUFFER_SIZE);
  
  auto it = Resources::get().shaders.find(name);
  if(it == Resources::get().shaders.end()) return;

  const std::unordered_map<GLenum, CW::Renderer::ShaderData>& reg = Resources::get().getShader(name).getRegisterShader();
  auto ita = reg.find(type);
  if(ita == reg.end()) return;

  std::string source = reg.at(type).getSource();
  memcpy(buffer, source.data(), source.size());
};



void UW::ShaderEditor::guiShaderEditor(){
  float width = ImGui::GetContentRegionAvail().x;
  float height = ImGui::GetContentRegionAvail().y - 50.0f;
  
  ImGui::SeparatorText("Shader Editor");
  ImGui::Text("Shader: %s : %s", shader_name.c_str(), UW::Config::SHADER_TYPE_TO_NAME[shader_type].c_str());
  
  ImGui::InputTextMultiline("##Shader Content", buffer, UW::Config::SHADER_EDITOR_BUFFER_SIZE, ImVec2(width, height), ImGuiInputTextFlags_WordWrap);

  auto it = Resources::get().shaders.find(shader_name);
  if(it == Resources::get().shaders.end()) return;
  
  auto& reg = Resources::get().getShader(shader_name).getRegisterShader();
  auto it2 = reg.find(shader_type);
  if(it2 == reg.end()) return;

  if(strcmp(buffer, reg.at(shader_type).getSource().c_str()) != 0) shader_is_updated = true;

  if(shader_is_updated){
    shader_is_updated = false;
    
    Resources::get().getShader(shader_name).destroy();
    Resources::get().getShader(shader_name).removeShaders(shader_type);
    Resources::get().getShader(shader_name).setShader(buffer, shader_type);
    Resources::get().getShader(shader_name).compile();
    serializer.saveShaders(shader_name, shader_type);
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::ShaderEditor::shaderEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderLoad(shader_name, shader_type);
    guiShaderEditor();
  };
};



std::string UW::ShaderEditor::getName(){
  return shader_name;
};


GLenum UW::ShaderEditor::getType(){
  return shader_type;
};






UW::UI::UI(CW::Renderer::Renderer &window,  float &fps, bool &debug_camera_on, UW::Camera &camera, UW::Camera &debug_camera, UW::ObjectManager &object_manager, UW::DataSerializer& serializer)
  :window(window), gui(&window), fps(fps), debug_camera_on(debug_camera_on), camera(camera), debug_camera(debug_camera), object_manager(object_manager), serializer(serializer){
  shader_editors.reserve(20);
  gui.setWorkspace(appWorkspace());
};



UW::UI::~UI(){
  saveShaderEditors();
};


void UW::UI::onLoad(){
  uiLoad();
  window.setSize(guiSettings.window_width, guiSettings.window_height);
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
  loadShaderEditors();
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
    if (sscanf(line, "MaterialExplorerOn=%d", &value) == 1) s->materialExplorerOn = value;
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
    out_buf->appendf("MaterialExplorerOn=%d\n", guiSettings.materialExplorerOn);
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
  if(guiSettings.infoWindowOn){
    gui.addWindow("Info Gui", windowGui());
  }
  else{
    gui.deleteWindow("Info Gui");  
  };
  if(guiSettings.materialExplorerOn){
    gui.addWindow("Material Explorer", materialExplorerGui());
  }
  else{
    gui.deleteWindow("Material Explorer");
  };
  if(guiSettings.materialEditorOn){
    gui.addWindow("Material Editor", materialEditorGui());
  }
  else{
    gui.deleteWindow("Material Editor");
  };
  if(guiSettings.shaderExplorerWindowOn){
    gui.addWindow("Shader Explorer", shaderExplorerGui());
  }
  else{
    gui.deleteWindow("Shader Explorer");
  };
  if(guiSettings.objectExplorerWindowOn){
    gui.addWindow("Object Explorer", objectExplorerGui());
  }
  else{
    gui.deleteWindow("Object Explorer");
  };
  if(guiSettings.objectEditorWindowOn){
    gui.addWindow("Object Editor", objectEditorGui());
  }
  else{
    gui.deleteWindow("Object Editor");
  };
};



void UW::UI::loadShaderEditors(){
  shader_editors.clear();
  
  for(std::pair<std::string, GLenum> el : guiSettings.shader_editors_reg){
    shader_editors.emplace_back(std::make_unique<ShaderEditor>(gui, serializer, el.first, el.second));
  };
};



void UW::UI::saveShaderEditors(){
  guiSettings.shader_editors_reg.clear();
  for(const auto& el : shader_editors){
    guiSettings.shader_editors_reg.emplace_back(el->getName(), el->getType());
  };
};



void UW::UI::menuBarGui(){
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Window")) {
      if(ImGui::MenuItem("Info")){
        guiSettings.infoWindowOn = !guiSettings.infoWindowOn;
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
    }
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



// -------- //
// Info Gui //
// -------- //
inline void UW::UI::guiInfo(){
  ImGui::SeparatorText("Info");
  ImGui::Text("FPS: %f", fps);

  ImGui::Text("Current camera: %s", debug_camera_on ? "Debug" : "Normal");

  ImGui::Text("Camera:");
  ImGui::InputFloat3("Camera POS: [%f, %f, %f]", &camera.position[0]);
  ImGui::SliderFloat3("Camera DIR: [%f, %f, %f]", &camera.direction[0], -1, 1);

  ImGui::Text("Debug Camera:");
  ImGui::InputFloat3("Debug POS: [%f, %f, %f]", &debug_camera.position[0]);
  ImGui::SliderFloat3("Debug DIR: [%f, %f, %f]", &debug_camera.direction[0], -1, 1);

  if(ImGui::Checkbox("Mesh mode", &guiSettings.mesh_mode_on)) mesh_mode_is_updated = false;
  if(!mesh_mode_is_updated){
    mesh_mode_is_updated = true;
    if(guiSettings.mesh_mode_on){
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else{
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };
  };
};



void UW::UI::guiControlsInfo(){
  ImGui::SeparatorText("Controls Info");

  ImGui::Text("- Swap Camera: %s", UW::Config::SWAP_CAMERA_BTN.c_str());
  ImGui::Text("- Swap Camera Mode: %s", UW::Config::CAMERA_SWAP_MODE_BTN.c_str());
  ImGui::Text("- Camera Accelerate: %s", UW::Config::CAMERA_ACCELERATE.c_str());
  ImGui::Text("- Camera Decelerate: %s", UW::Config::CAMERA_DECELERATE.c_str());
  ImGui::Text("- Move Forward: %s", UW::Config::CAMERA_MOVE_FORWARD.c_str());
  ImGui::Text("- Move Back: %s", UW::Config::CAMERA_MOVE_BACK.c_str());
  ImGui::Text("- Move Right: %s", UW::Config::CAMERA_MOVE_RIGHT.c_str());
  ImGui::Text("- Move Left: %s", UW::Config::CAMERA_MOVE_LEFT.c_str());
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI::windowGui(){
return [this](CW::Renderer::iRenderer *window){
  guiControlsInfo();
  guiInfo();
};
};



// ------------------ //
// Materials Explorer //
// ------------------ //
inline void UW::UI::guiMaterialList(){
  ImGui::SeparatorText("Materials List");

  for (std::pair<std::string, Material> el : Resources::get().materials.getMaterialReg()) {
    std::string button_label = "- " + el.first;
    if (ImGui::Button(button_label.c_str())) guiSettings.material_name = el.first;

    button_label = "Delete ##" + el.first;
    ImGui::SameLine();
    if (ImGui::Button(button_label.c_str())) {
      Resources::get().materials.erase(el.first);
      break;
    }
  };

  std::string button_label = "Add " + std::to_string(Resources::get().materials.size());
  if (ImGui::Button(button_label.c_str())) Resources::get().materials.emplace_back("new material", UW::Material());
  
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI::materialExplorerGui(){
return [this](CW::Renderer::iRenderer *window){
  guiMaterialList();
};
};



// --------------- //
// Material Editor //
// --------------- //
inline void UW::UI::guiMaterialParameters(){
  ImGui::SeparatorText("Materials Parameters");
  ImGui::Text("Material id: %s", guiSettings.material_name.c_str());

  if(!Resources::get().materials.find(guiSettings.material_name)) return;

  Material temp_mat = Resources::get().materials.getMaterial(guiSettings.material_name);

  char name_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(name_buffer, guiSettings.material_name.data(), guiSettings.material_name.size());
  name_buffer[guiSettings.material_name.size()] = '\0';
  if(ImGui::InputText("name", name_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE)){
    Resources::get().materials.erase(guiSettings.material_name);
    guiSettings.material_name = std::string(name_buffer + '\0');
    Resources::get().materials.emplace_back(guiSettings.material_name, temp_mat);
  };
  
  if(ImGui::ColorEdit3("Albedo: ", &temp_mat.albedo[0])) material_is_updated = true;
  if(ImGui::SliderFloat("Roughness: ", &temp_mat.roughness, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::SliderFloat("Metallic: ", &temp_mat.metallic, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::ColorEdit3("Emission Color: ", &temp_mat.emission_color[0])) material_is_updated = true;
  if(ImGui::SliderFloat("Emission Strength: ", &temp_mat.emission_strength, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::SliderFloat("Ambient Occlusion: ", &temp_mat.ambient_occlusion, 0.0f, 1.0f)) material_is_updated = true;

  if(material_is_updated){
    material_is_updated = false;
    Resources::get().materials[guiSettings.material_name] = temp_mat;
    Resources::get().materials.compile();
  };
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI::materialEditorGui(){
return [this](CW::Renderer::iRenderer *window){
  guiMaterialParameters();
};
};



// --------------- //
// Shader Explorer //
// --------------- //
void UW::UI::guiShaderList(){
  ImGui::SeparatorText("Shader List");

  if(ImGui::Button("reset")) Resources::get().shaders.clear();

  for (const auto& [ key, values ] : Resources::get().shaders) {
    if(ImGui::CollapsingHeader(key.c_str())){
      for (const auto& [key_s, values_s] : values.getRegisterShader()){
        std::string button_label = UW::Config::SHADER_TYPE_TO_NAME[key_s] +  "##-" + key;
        if (ImGui::Button(button_label.c_str())){
          bool exists = std::any_of(
            shader_editors.begin(),
            shader_editors.end(),
            [&](const auto& editor) { return editor->getName() == key && editor->getType() == key_s; }
          );

          if (exists){
            shader_editors.erase(
              std::remove_if(
                shader_editors.begin(),
                shader_editors.end(),
                [&](const auto& editor) { return editor->getName() == key && editor->getType() == key_s; }
              ),
              shader_editors.end()
            );
          }
          else shader_editors.emplace_back(std::make_unique<ShaderEditor>(gui, serializer, key, key_s));
        };
      };
    };
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI::shaderExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderList();
  };
};



// --------------- //
// Object Explorer //
// --------------- //
void UW::UI::guiObjectList(){
  ImGui::SeparatorText("Object List");

  for(unsigned int id = 0; id < object_manager.objects.size(); id++){
    std::string label = "- " + object_manager.objects[id].name + "##(" + std::to_string(id) + ")";
    if(ImGui::Button(label.c_str())) guiSettings.object_id = id;
    
    label = "Delete##" + std::to_string(id);
    ImGui::SameLine();
    if(ImGui::Button(label.c_str())) object_manager.objects.erase(object_manager.objects.begin() + id);
  };

  if(ImGui::Button("Add new")) object_manager.objects.emplace_back(UW::GameObject("new object", "testing", "testing", {}, {}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI::objectExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiObjectList();
  };
};



// ------------- //
// Object Editor //
// ------------- //
void UW::UI::guiObjectEditor(){
  ImGui::SeparatorText("Object Editor");
  if(guiSettings.object_id >= object_manager.objects.size()) return;
  
  UW::GameObject& object = object_manager.objects[guiSettings.object_id];

  char name_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(name_buffer, object.name.data(), object.name.size());
  name_buffer[object.name.size()] = '\0';
  ImGui::InputText("name", name_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  object.name = std::string(name_buffer + '\0');

  char mesh_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(mesh_buffer, object.mesh.data(), object.mesh.size());
  mesh_buffer[object.mesh.size()] = '\0';
  ImGui::InputText("mesh", mesh_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  object.mesh = std::string(mesh_buffer + '\0');

  char shader_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(shader_buffer, object.shader.data(), object.shader.size());
  shader_buffer[object.shader.size()] = '\0';
  ImGui::InputText("shader", shader_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  object.shader = std::string(shader_buffer + '\0');


  ImGui::InputFloat3("position: ", &object.position[0]);
  glm::vec3 position_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("position slider: ", &position_offset[0], -100.0f, 100.0f);
  object.position += position_offset * window.getWindowData()->delta_time;

  ImGui::InputFloat3("rotate: ", &object.rotation[0]);
  glm::vec3 rotate_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("rotate slider: ", &rotate_offset[0], -1.0f, 1.0f);
  object.rotation += rotate_offset * window.getWindowData()->delta_time;

  ImGui::InputFloat3("scale: ", &object.scale[0]);
  glm::vec3 scale_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("scale slider: ", &scale_offset[0], -100.0f, 100.0f);
  object.scale += scale_offset * window.getWindowData()->delta_time;


  ImGui::SeparatorText("Textures: ");
  for(int i = 0; i < object.textures.size(); i++){
    std::string label = "- texture (" + std::to_string(i) + ")";
    char texture_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
    memcpy(texture_buffer, object.textures[i].data(), object.textures[i].size());
    texture_buffer[object.textures[i].size()] = '\0';
    ImGui::InputText(label.c_str(), texture_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
    object.textures[i] = std::string(texture_buffer + '\0');
    
    ImGui::SameLine();
    label = "Delete texture##(" + std::to_string(i) + ")";
    if(ImGui::Button(label.c_str())) object.textures.erase(object.textures.begin() + i);
  };

  std::string label = "Add Texture (" + std::to_string(object.textures.size()) + ")";
  if(ImGui::Button(label.c_str())) object.textures.emplace_back("");


  ImGui::SeparatorText("Materials: ");
  for(int i = 0; i < object.materials.size(); i++){
    std::string label = "- material (" + std::to_string(i) + ")";
    
    char material_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
    memcpy(material_buffer, object.materials[i].data(), object.materials[i].size());
    material_buffer[object.materials[i].size()] = '\0';
    ImGui::InputText(label.c_str(), material_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
    object.materials[i] = std::string(material_buffer + '\0');
    
    ImGui::SameLine();
    label = "Delete material##(" + std::to_string(i) + ")";
    if(ImGui::Button(label.c_str())) object.materials.erase(object.materials.begin() + i);
  };

  label = "Add material (" + std::to_string(object.materials.size()) + ")";
  if(ImGui::Button(label.c_str())) object.materials.emplace_back("new material");
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI::objectEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiObjectEditor();
  };
};
