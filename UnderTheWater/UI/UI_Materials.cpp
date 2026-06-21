// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "UI_Materials.h"

#ifndef PRODUCTION



UW::UI_Materials::UI_Materials(CW::Gui::Gui& gui)
  :gui(gui){};



UW::UI_Materials::~UI_Materials(){
};



void UW::UI_Materials::uiControl(){
  if(guiSettings.materialExplorerOn){
    Logger::get().info("UI", "Opening Materials Explorer GUI");
    gui.addWindow("Material Explorer", materialExplorerGui());
  }
  else{
    Logger::get().info("UI", "Closing Materials Explorer GUI");
    gui.deleteWindow("Material Explorer");
  };
  
  if(guiSettings.materialEditorOn){
    Logger::get().info("UI", "Opening Materials Editor GUI");
    gui.addWindow("Material Editor", materialEditorGui());
  }
  else{
    Logger::get().info("UI", "Closing Materials Editor GUI");
    gui.deleteWindow("Material Editor");
  };
};



inline void UW::UI_Materials::guiMaterialList(){
  ImGui::SeparatorText("Materials List");

  for (std::pair<std::string, Material> el : Resources::get().materials.getMaterialReg()) {
    std::string button_label = "- " + el.first;
    if (ImGui::Button(button_label.c_str())) guiSettings.material_name = el.first;

    button_label = "Delete ##" + el.first;
    ImGui::SameLine();
    if (ImGui::Button(button_label.c_str())) {
      Resources::get().materials.erase(el.first);
      Logger::get().warn("UI", "Deleted Material { " + el.first + " }");
      break;
    };
  };

  std::string button_label = "Add " + std::to_string(Resources::get().materials.size());
  if (ImGui::Button(button_label.c_str())) {
    Resources::get().materials.emplace_back("new material", UW::Material());
    Logger::get().info("UI", "Added new Material { new material }");
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Materials::materialExplorerGui(){
return [this](CW::Renderer::iRenderer *window){
  guiMaterialList();
};
};



inline void UW::UI_Materials::guiMaterialParameters(){
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
    Logger::get().info("UI", "Updating Material { " +  guiSettings.material_name + " }");
    material_is_updated = false;
    Resources::get().materials[guiSettings.material_name] = temp_mat;
    Resources::get().materials.compile();
  };
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Materials::materialEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiMaterialParameters();
  };
};

#endif
