#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window), debug_camera(&window), gui(&window){
  initWindow();
  gui.setWorkspace(appWorkspace());

  onLoad();
};



UW::App::~App(){
  onDestroy();
};



bool UW::App::isRunning(){
  return !window.getWindowData()->should_close;
};



void UW::App::run(){
  update();
  fixedUpdate();
  render();
};



// ===================================== //
// ========== Core Operations ========== //
// ===================================== //
void UW::App::onLoad(){
  camera.position = {1055, 797, 1331};
  debug_camera.position = {1157, 2048, 1310};
  debug_camera.direction = {-0.57, -0.76, -0.28};

  objects.emplace_back(GameObject("sky_box", "water", {}, 
    glm::vec3(0.0f, 2000.0f, 0.0f), glm::vec3(0.0f), glm::vec3(5.25f)));
};



void UW::App::onDestroy() {
  objects.clear();
  Resources::get().destroy();
};



void UW::App::render(){
  window.beginFrame();  

  Resources::get().lights["static"].bind(0);
  Resources::get().materials.bind(1);
  if(debug_camera_on){ 
    terrain.render(&window, camera, debug_camera);
    skybox.render(&window, camera, debug_camera); 
    water.render(&window, camera, debug_camera);
    for(UW::GameObject& object : objects) object.render(&window, camera, debug_camera);
  }
  else {
    terrain.render(&window, camera, camera);
    skybox.render(&window, camera, camera);
    water.render(&window, camera, camera);
    for(UW::GameObject& object : objects) object.render(&window, camera, camera);
  };
  
  Resources::get().materials.unbind();
  Resources::get().lights["static"].unbind();
  
  gui.render();

  window.windowEvents();
  window.swapBuffer();
};



void UW::App::update(){
  updateFps();
  swapCamera();

  camera.event(&window);

  for(UW::GameObject& el : objects) el.onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    fixed_update_time_acc -= 1.0f / UW::Config::FIXED_HZ;
    
    for(UW::GameObject& el : objects) el.onFixedUpdate();
  };
};



// ============================= //
// ========== Helpers ========== //
// ============================= //
void UW::App::initWindow(){
  window.setWindowTitle(UW::Config::WINDOW_TITLE);
  window.setCursorVisibility(UW::Config::DEFAULT_CURSOR_IS_VISIBLE);
  window.setVsync(UW::Config::VSYNC);
};



void UW::App::swapCamera(){
  if(window.getInputData()->is_key_down(UW::Config::SWAP_CAMERA_BTN) && camera_swap_cooldown_acc <= 0.0f) {
    debug_camera_on = !debug_camera_on;
    camera_swap_cooldown_acc = UW::Config::CAMERA_SWAP_COOLDOWN;
  };

  if(camera_swap_cooldown_acc >= 0.0f) camera_swap_cooldown_acc -= window.getWindowData()->delta_time;
};



void UW::App::updateFps(){
  if(fps_id > UW::Config::FPS_SAMPLES){
    fps = fps_id / fps_acc;
    fps_acc = 0.0f;
    fps_id = 0;
  }
  else{
    fps_acc += window.getWindowData()->delta_time;
    fps_id++;
  };
};



// ========================= //
// ========== GUI ========== //
// ========================= //
void UW::App::menuBarGui(){
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Window")) {
      if(ImGui::MenuItem("Info")){
        if(infoWindowOn){
          gui.deleteWindow("Info Gui");  
          infoWindowOn = false;
        }
        else{
          gui.addWindow("Info Gui", windowGui());
          infoWindowOn = true;
        }
      };
      if(ImGui::MenuItem("Material Explorer")){
        if(materialWindowOn){
          gui.deleteWindow("Material Explorer");
          materialWindowOn = false;
        }
        else{
          gui.addWindow("Material Explorer", materialExplorerGui());
          materialWindowOn = true;
        }
      };
      if(ImGui::MenuItem("Shader Explorer")){
        if(materialWindowOn){
          gui.deleteWindow("Shader Explorer");
          shaderExplorerWindowOn = false;
        }
        else{
          gui.addWindow("Shader Explorer", shaderExplorerGui());
          shaderExplorerWindowOn = true;
        }
      };
      if(ImGui::MenuItem("Shader Editor")){
        if(materialWindowOn){
          gui.deleteWindow("Shader Editor");
          shaderEditorWindowOn = false;
        }
        else{
          gui.addWindow("Shader Editor", shaderEditorGui());
          shaderEditorWindowOn = true;
        }
      };
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  };
};



std::function<void(std::function<void()> render_windows)> UW::App::appWorkspace() {
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
inline void UW::App::guiInfo(){
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



void UW::App::guiControlsInfo(){
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



inline std::function<void(CW::Renderer::iRenderer *window)> UW::App::windowGui(){
return [this](CW::Renderer::iRenderer *window){
  guiControlsInfo();
  guiInfo();
};
};



// ------------------ //
// materials Explorer //
// ------------------ //
inline void UW::App::guiMaterialParameters(){
  ImGui::SeparatorText("Materials Parameters");

  if(material_id >= Resources::get().materials.size()) return;

  Material temp_mat = Resources::get().materials.getMaterial(material_id);

  if(ImGui::ColorEdit3("Albedo: ", &temp_mat.albedo[0])) material_is_updated = true;
  if(ImGui::SliderFloat("Roughness: ", &temp_mat.roughness, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::SliderFloat("Metallic: ", &temp_mat.metallic, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::ColorEdit3("Emission Color: ", &temp_mat.emission_color[0])) material_is_updated = true;
  if(ImGui::SliderFloat("Emission Strength: ", &temp_mat.emission_strength, 0.0f, 1.0f)) material_is_updated = true;
  if(ImGui::SliderFloat("Ambient Occlusion: ", &temp_mat.ambient_occlusion, 0.0f, 1.0f)) material_is_updated = true;

  if(material_is_updated){
    material_is_updated = false;
    Resources::get().materials[material_id] = temp_mat;
    Resources::get().materials.compile();
  };
};



inline void UW::App::guiMaterialList(){
  ImGui::SeparatorText("Materials List");

  for (unsigned int id = 0; id < Resources::get().materials.size(); id++) {
    std::string button_label = "- " + std::to_string(id);
    if (ImGui::Button(button_label.c_str())) material_id = id;
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::App::materialExplorerGui(){
return [this](CW::Renderer::iRenderer *window){
  guiMaterialParameters();
  guiMaterialList();
};
};



// --------------- //
// Shader Explorer //
// --------------- //
void UW::App::guiShaderList(){
  ImGui::SeparatorText("Shader List");

  for (const auto& [ key, values ] : Resources::get().shaders) {
    for (const auto& [key_s, values_s] : values.getRegisterShader()){
      std::string button_label = "- " + key + ": " + std::to_string(key_s);
      
      if (ImGui::Button(button_label.c_str())) {
        shader_name = key;
        shader_type = key_s;
        std::string source = values_s.getSource();
        memcpy(buffer, source.data(), source.size());
      };
    };
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::App::shaderExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderList();
  };
};



// ------------- //
// Shader Editor //
// ------------- //
void UW::App::guiShaderEditor(){
  float width = ImGui::GetContentRegionAvail().x;
  float height = ImGui::GetContentRegionAvail().y - 50.0f;
  
  ImGui::SeparatorText("Shader Editor");
  
  ImGui::InputTextMultiline("##Shader Content", buffer, UW::Config::SHADER_EDITOR_BUFFER_SIZE, ImVec2(width, height), ImGuiInputTextFlags_WordWrap);

  auto it = Resources::get().shaders.find(shader_name);
  if(it == Resources::get().shaders.end()) return;
  auto& reg = Resources::get().shaders[shader_name].getRegisterShader();
  
  auto it2 = reg.find(shader_type);
  if(it2 == reg.end()) return;
  if(strcmp(buffer, reg.at(shader_type).getSource().c_str()) == 0) shader_is_updated = true;

  if(shader_is_updated){
    shader_is_updated = false;
    
    Resources::get().shaders[shader_name].destroy();
    Resources::get().shaders[shader_name].removeShaders(shader_type);
    Resources::get().shaders[shader_name].setShader(buffer, shader_type);
    Resources::get().shaders[shader_name].compile();
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::App::shaderEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderEditor();
  };
};