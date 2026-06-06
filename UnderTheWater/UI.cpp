#include "UI.h"



UW::UI::UI(CW::Renderer::Renderer &window,  float &fps, bool &debug_camera_on, UW::Camera &camera, UW::Camera &debug_camera, std::vector<UW::GameObject> &objects)
  :window(window), gui(&window), fps(fps), debug_camera_on(debug_camera_on), camera(camera), debug_camera(debug_camera), objects(objects){
  gui.setWorkspace(appWorkspace());
};



UW::UI::~UI(){

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

  uiControl();
  guiShaderLoad(guiSettings.shader_name, guiSettings.shader_type);
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
    if (sscanf(line, "Shader_Type=%d", &value) == 1) s->shader_type = value;
    if (sscanf(line, "Window_Width=%d", &value) == 1) s->window_width = value;
    if (sscanf(line, "Window_Height=%d", &value) == 1) s->window_height = value;
    
    char value_str[256];
    if (sscanf(line, "Shader_Name=%256s", &value_str) == 1) s->shader_name = std::string(value_str);
    if (sscanf(line, "Material_ID=%256s", &value_str) == 1) s->material_name = std::string(value_str);
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
    out_buf->appendf("Shader_Type=%d\n", guiSettings.shader_type);
    out_buf->appendf("Window_Width=%d\n", guiSettings.window_width);
    out_buf->appendf("Window_Height=%d\n", guiSettings.window_height);
    out_buf->appendf("Shader_Name=%s\n", guiSettings.shader_name.c_str());
    out_buf->appendf("Material_ID=%s\n", guiSettings.material_name.c_str());
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
  if(guiSettings.shaderEditorWindowOn){
    gui.addWindow("Shader Editor", shaderEditorGui());
  }
  else{
    gui.deleteWindow("Shader Editor");
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
      if(ImGui::MenuItem("Shader Editor")){
        guiSettings.shaderEditorWindowOn = !guiSettings.shaderEditorWindowOn;
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
// materials Explorer //
// ------------------ //
inline void UW::UI::guiMaterialList(){
  ImGui::SeparatorText("Materials List");

  for (std::pair<std::string, Material> el : Resources::get().materials.getMaterialReg()) {
    std::string button_label = "- " + el.first;
    if (ImGui::Button(button_label.c_str())) guiSettings.material_name = el.first;

    button_label = "Delete " + el.first;
    ImGui::SameLine();
    if (ImGui::Button(button_label.c_str())) Resources::get().materials.erase(el.first);
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
void UW::UI::guiShaderLoad(std::string name, GLenum type){
  guiSettings.shader_name = name;
  guiSettings.shader_type = type;
  memset(buffer, '\0', UW::Config::SHADER_EDITOR_BUFFER_SIZE);
  std::string source = Resources::get().shaders[name].getRegisterShader().at(type).getSource();
  memcpy(buffer, source.data(), source.size());
};



std::string UW::UI::getShaderTypeName(GLenum type){
  if(type == GL_VERTEX_SHADER) return "Vertex Shader";
  if(type == GL_FRAGMENT_SHADER) return "Fragment Shader";
  if(type == GL_GEOMETRY_SHADER) return "Geometry Shader";
  if(type == GL_TESS_CONTROL_SHADER) return "Tess Control Shader";
  if(type == GL_TESS_EVALUATION_SHADER) return "Tess Evaluation Shader";
  return std::to_string(type);
};



void UW::UI::guiShaderList(){
  ImGui::SeparatorText("Shader List");

  for (const auto& [ key, values ] : Resources::get().shaders) {
    for (const auto& [key_s, values_s] : values.getRegisterShader()){
      std::string button_label = "- " + key + ": " + getShaderTypeName(key_s);
      if (ImGui::Button(button_label.c_str())) guiShaderLoad(key, key_s);
    };
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI::shaderExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderList();
  };
};



// ------------- //
// Shader Editor //
// ------------- //
void UW::UI::guiShaderEditor(){
  float width = ImGui::GetContentRegionAvail().x;
  float height = ImGui::GetContentRegionAvail().y - 50.0f;
  
  ImGui::SeparatorText("Shader Editor");
  ImGui::Text("Shader: %s : %s", guiSettings.shader_name.c_str(), getShaderTypeName(guiSettings.shader_type).c_str());
  
  ImGui::InputTextMultiline("##Shader Content", buffer, UW::Config::SHADER_EDITOR_BUFFER_SIZE, ImVec2(width, height), ImGuiInputTextFlags_WordWrap);

  auto it = Resources::get().shaders.find(guiSettings.shader_name);
  if(it == Resources::get().shaders.end()) return;
  
  auto& reg = Resources::get().shaders[guiSettings.shader_name].getRegisterShader();
  auto it2 = reg.find(guiSettings.shader_type);
  if(it2 == reg.end()) return;

  if(strcmp(buffer, reg.at(guiSettings.shader_type).getSource().c_str()) != 0) shader_is_updated = true;

  if(shader_is_updated){
    shader_is_updated = false;
    
    Resources::get().shaders[guiSettings.shader_name].destroy();
    Resources::get().shaders[guiSettings.shader_name].removeShaders(guiSettings.shader_type);
    Resources::get().shaders[guiSettings.shader_name].setShader(buffer, guiSettings.shader_type);
    Resources::get().shaders[guiSettings.shader_name].compile();
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI::shaderEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderEditor();
  };
};



// --------------- //
// Object Explorer //
// --------------- //
void UW::UI::guiObjectList(){
  ImGui::SeparatorText("Object List");

  for(unsigned int id = 0; id < objects.size(); id++){
    std::string label = "- " + objects[id].name + " (" + std::to_string(id) + ")";
    if(ImGui::Button(label.c_str())) guiSettings.object_id = id;
    
    label = "Delete " + std::to_string(id);
    ImGui::SameLine();
    if(ImGui::Button(label.c_str())) objects.erase(objects.begin() + id);
  };

  if(ImGui::Button("Add new")) objects.emplace_back(UW::GameObject("new object", "testing", "testing", {}, {}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
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
  if(guiSettings.object_id >= objects.size()) return;

  UW::GameObject& object = objects[guiSettings.object_id];

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
    
    label = "Delete texture (" + std::to_string(i) + ")";
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
    
    label = "Delete material (" + std::to_string(i) + ")";
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
