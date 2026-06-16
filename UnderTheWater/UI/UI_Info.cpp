#include "UI_Info.h"

#ifndef PRODUCTION



UW::UI_Info::UI_Info(CW::Gui::Gui& gui, UW::Camera &camera, UW::Camera &debug_camera, float &fps, bool &debug_camera_on, bool &post_processing_on)
  :gui(gui), camera(camera), debug_camera(debug_camera), fps(fps), debug_camera_on(debug_camera_on), post_processing_on(post_processing_on){};



UW::UI_Info::~UI_Info(){};



void UW::UI_Info::uiControl(){
  if(guiSettings.infoWindowOn){
    Logger::get().info("UI", "Opening Info Gui");
    gui.addWindow("Info Gui", ui());
  }
  else{
    Logger::get().info("UI", "Closing Info GUI");
    gui.deleteWindow("Info Gui");  
  };
};



inline void UW::UI_Info::guiInfo(){
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
      Logger::get().info("UI", "Changed Draw Mode To Mesh");
    }
    else{
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      Logger::get().info("UI", "Changed Draw Mode To Normal");
    };
  };

  if(ImGui::Checkbox("Post Processing", &post_processing_on));
};



void UW::UI_Info::guiControlsInfo(){
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



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Info::ui(){
return [this](CW::Renderer::iRenderer *window){
  guiControlsInfo();
  guiInfo();
};
};



#endif