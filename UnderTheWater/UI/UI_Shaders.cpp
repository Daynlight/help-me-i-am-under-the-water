// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "UI_Shaders.h"

#ifndef PRODUCTION



UW::UI_Shaders::UI_Shaders(CW::Gui::Gui& gui)
  :gui(gui){

  shader_editors.reserve(20);
};



UW::UI_Shaders::~UI_Shaders(){

};



void UW::UI_Shaders::uiControl(){
  if(guiSettings.shaderExplorerWindowOn){
    Logger::get().info("UI", "Opening Shader Explorer GUI");
    gui.addWindow("Shader Explorer", shaderExplorerGui());
  }
  else{
    Logger::get().info("UI", "Closing Shader Explorer GUI");
    gui.deleteWindow("Shader Explorer");
  };
};



void UW::UI_Shaders::loadShaderEditors(){
  Logger::get().info("UI", "Loading Shader Editors");

  shader_editors.clear();
  
  for(std::pair<std::string, GLenum> el : guiSettings.shader_editors_reg){
    shader_editors.emplace_back(std::make_unique<UI_ShaderEditor>(gui, el.first, el.second));
  };
};


void UW::UI_Shaders::saveShaderEditors(){
  Logger::get().info("UI", "Saving Shader Editors");

  guiSettings.shader_editors_reg.clear();
  for(const auto& el : shader_editors){
    guiSettings.shader_editors_reg.emplace_back(el->getName(), el->getType());
  };
};



void UW::UI_Shaders::guiShaderList(){
  ImGui::SeparatorText("Shader List");

  if(ImGui::Button("reset")) {
    Logger::get().info("UI", "Refreshing Shaders");
    Resources::get().shaders.clear();
  };

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
          else shader_editors.emplace_back(std::make_unique<UI_ShaderEditor>(gui, key, key_s));
        };
      };
    };
  };
};

inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Shaders::shaderExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderList();
  };
};

#endif
