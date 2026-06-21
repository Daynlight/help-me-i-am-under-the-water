// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "UI_Scripts.h"

#ifndef PRODUCTION



UW::UI_Scripts::UI_Scripts(CW::Gui::Gui& gui)
  :gui(gui){

  script_editors.reserve(20);
};



UW::UI_Scripts::~UI_Scripts(){

};



void UW::UI_Scripts::uiControl(){
  if(guiSettings.scriptsExplorerWindowOn){
    Logger::get().info("UI_Scripts", "Opening Script Explorer GUI");
    gui.addWindow("Script Explorer", scriptExplorerGui());
  }
  else{
    Logger::get().info("UI_Scripts", "Closing Script Explorer GUI");
    gui.deleteWindow("Script Explorer");
  };
};



void UW::UI_Scripts::loadScriptEditors(){
  Logger::get().info("UI_Scripts", "Loading Scripts Editors");

  script_editors.clear();
  
  for(std::string el : guiSettings.scripts_editors_reg){
    script_editors.emplace_back(std::make_unique<UI_ScriptEditor>(gui, el));
  };
};



void UW::UI_Scripts::saveScriptEditors(){
  Logger::get().info("UI_Scripts", "Saving Scripts Editors");

  guiSettings.scripts_editors_reg.clear();
  for(const auto& el : script_editors){
    if(el) guiSettings.scripts_editors_reg.emplace_back(el->getName());
  };
};



std::vector<std::string> UW::UI_Scripts::getAvailableScripts() {
  std::vector<std::string> script_files;
  
  if (fs::exists(UW::Config::SCRIPTS_FOLDER) && fs::is_directory(UW::Config::SCRIPTS_FOLDER)) {
    for (const auto& entry : fs::directory_iterator(UW::Config::SCRIPTS_FOLDER)) {
      if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
        script_files.push_back(entry.path().filename().string());
      };
    };
  };
  
  return script_files;
};



void UW::UI_Scripts::guiScriptList() {
  ImGui::SeparatorText("Scripts List");

  auto available_scripts = getAvailableScripts();
  std::vector<std::string> scripts_to_close;

  for (const auto& script_name : available_scripts) {
    if (ImGui::Button(script_name.c_str())) {
      bool is_open = std::any_of(
        script_editors.begin(),
        script_editors.end(),
        [&](const auto& editor) { return editor->getName() == script_name; }
      );

      if (is_open) {
        scripts_to_close.push_back(script_name);
      } else {
        script_editors.emplace_back(std::make_unique<UI_ScriptEditor>(gui, script_name));
      };
    };
  };


  for (const auto& name_to_close : scripts_to_close) {
    script_editors.erase(
      std::remove_if(script_editors.begin(), script_editors.end(),
        [&](const auto& editor) { return editor->getName() == name_to_close; }
      ),
      script_editors.end()
    );
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Scripts::scriptExplorerGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiScriptList();
  };
};

#endif
