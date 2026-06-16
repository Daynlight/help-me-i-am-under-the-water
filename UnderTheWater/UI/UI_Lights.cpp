#include "UI_Lights.h"

#ifndef PRODUCTION



UW::UI_Lights::UI_Lights(CW::Gui::Gui& gui)
  :gui(gui){};



UW::UI_Lights::~UI_Lights(){
};



void UW::UI_Lights::uiControl(){
  if(guiSettings.lightsExplorerOn){
    Logger::get().info("UI", "Opening Lights Explorer GUI");
    gui.addWindow("Lights Editor", ui());
  }
  else{
    Logger::get().info("UI", "Closing Lights Explorer GUI");
    gui.deleteWindow("Lights Editor");
  };
};



void UW::UI_Lights::guiLights(){
  ImGui::SeparatorText("Lights");
  bool lights_updated = false;

  for(unsigned int i = 0; i < Resources::get().lights.size(); i++){
    UW::Light& light = Resources::get().lights[i];
    std::string label = "light: (" + std::to_string(i) + ")"; 
    ImGui::Text(label.c_str());
    if(ImGui::InputFloat3(("position: [%f, %f, %f] ##" + std::to_string(i)).c_str(), &light.position[0])) lights_updated = true;
    if(ImGui::ColorEdit3(("color:  ##" + std::to_string(i)).c_str(), &light.color[0])) lights_updated = true;
    if(ImGui::InputFloat(("strength: %f ##" + std::to_string(i)).c_str(), &light.strength)) lights_updated = true;

    std::string delete_light_label = "Delete ##(" + std::to_string(i) + ")";
    if(ImGui::Button(delete_light_label.c_str())) {
      Resources::get().lights.erase(i);
      Logger::get().info("UI", "Deleted Light at {" + std::to_string(i) + "}");
      lights_updated = true;
    };

    ImGui::Separator();
  };

  std::string add_light_label = "Add Light ##(" + std::to_string(Resources::get().lights.size()) + ")";
  if(ImGui::Button(add_light_label.c_str())) {
    Resources::get().lights.emplace_back(UW::Light({0, 0, 0}, {1, 1, 1}, 1));
    Logger::get().info("UI", "Added Light at {" + std::to_string(Resources::get().lights.size()) + "}");
    lights_updated = true;
  };

  if(lights_updated){
    DataSerializer::get().saveAllLights(Resources::get().lights);
    Logger::get().info("UI", "Lights saved");
  };
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Lights::ui(){
  return [this](CW::Renderer::iRenderer *window){
    guiLights();
  };
};



#endif
