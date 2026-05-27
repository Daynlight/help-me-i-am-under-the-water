#include "App.h"



// ========================= //
// ========== APP ========== //
// ========================= //
UW::App::App()
  :camera(&window), debug_camera(&window), gui(&window){
  initWindow();
  gui.addWindow("Gui", windowGui());

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

  objects["terrain"] = std::make_shared<UW::Terrain>();
  objects["water"] = std::make_shared<UW::Water>();
  objects["sky_box"] = std::make_shared<UW::Skybox>();
};



void UW::App::onDestroy() {
  objects.clear();
  Resources::get().destroy();
};



void UW::App::render(){
  window.beginFrame();  

  Resources::get().lights["static"].bind(0);
  if(debug_camera_on){ 
    objects["terrain"]->render(&window, camera, debug_camera);
    objects["sky_box"]->render(&window, camera, debug_camera); 
    objects["water"]->render(&window, camera, debug_camera);
  }
  else {
    objects["terrain"]->render(&window, camera, camera);
    objects["sky_box"]->render(&window, camera, camera);
    objects["water"]->render(&window, camera, camera);
  };
  
  Resources::get().lights["static"].unbind();
  
  gui.render();

  window.windowEvents();
  window.swapBuffer();
};



void UW::App::update(){
  updateFps();
  swapCamera();

  camera.event(&window);

  for(const std::pair<std::string, std::shared_ptr<Object>>& el : objects) el.second->onUpdate(window.getWindowData()->delta_time);
};



void UW::App::fixedUpdate(){
  fixed_update_time_acc += window.getWindowData()->delta_time;

  if(fixed_update_time_acc >= 1.0f / UW::Config::FIXED_HZ){
    fixed_update_time_acc -= 1.0f / UW::Config::FIXED_HZ;
    
    for(const std::pair<std::string, std::shared_ptr<Object>>& el : objects) el.second->onFixedUpdate();
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



inline void UW::App::guiMaterialParameters(){
  ImGui::SeparatorText("Materials Parameters");

  auto it = Resources::get().materials.find(material_name);
  if (it != Resources::get().materials.end()){
    ImGui::ColorEdit3("Albedo: ", &Resources::get().materials[material_name].albedo[0]);
    ImGui::SliderFloat("Roughness: ", &Resources::get().materials[material_name].roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("Metallic: ", &Resources::get().materials[material_name].metallic, 0.0f, 1.0f);
    ImGui::ColorEdit3("Emission Color: ", &Resources::get().materials[material_name].emission_color[0]);
    ImGui::SliderFloat("Emission Strength: ", &Resources::get().materials[material_name].emission_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("Ambient Occlusion: ", &Resources::get().materials[material_name].ambient_occlusion, 0.0f, 1.0f);
  };
};


inline void UW::App::guiMaterialList(){
  ImGui::SeparatorText("Materials List");

  for (const auto& [key, material] : Resources::get().materials) {
    std::string button_label = "- " + key;
    if (ImGui::Button(button_label.c_str())) material_name = key;
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::App::windowGui(){
return [this](CW::Renderer::iRenderer *window){
  guiControlsInfo();
  guiInfo();
  guiMaterialParameters();
  guiMaterialList();
};
};