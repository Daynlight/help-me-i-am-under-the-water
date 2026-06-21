#include "UI_ScriptEditor.h"
#ifndef PRODUCTION



UW::UI_ScriptEditor::UI_ScriptEditor(CW::Gui::Gui& gui, const std::string& name)
  :gui(gui), script_name(name){

  Logger::get().info("UI_ScriptEditor", "Opened { " + script_name + " }");
  
  save_cooldown_duration = std::chrono::milliseconds(static_cast<long long>(UW::Config::SCRIPT_SAVE_COOLDOWN * 1000.0f));
  last_save_time = std::chrono::steady_clock::now();
  
  gui.addWindow("Script Editor " + script_name, ScriptEditorGui());
};



UW::UI_ScriptEditor::~UI_ScriptEditor(){
  if (script_is_updated) {
    DataSerializer::get().saveScript(script_name, buffer);
    Logger::get().info("UI_ScriptEditor", "Force saved on close: { " + script_name + " }");
  };

  gui.deleteWindow("Script Editor " + script_name);
  Logger::get().info("UI_ScriptEditor", "Closed { " + script_name  + " }");
};



void UW::UI_ScriptEditor::guiScriptLoad(const std::string& name){
  if(script_is_loaded) return;

  script_name = name;
  memset(buffer, '\0', UW::Config::SCRIPT_EDITOR_BUFFER_SIZE);
  
  std::string source = DataSerializer::get().loadScript(name);
  size_t copy_size = std::min(source.size(), static_cast<size_t>(UW::Config::SCRIPT_EDITOR_BUFFER_SIZE) - 1);
  memcpy(buffer, source.data(), copy_size);
  buffer[copy_size] = '\0';


  Logger::get().info("UI_ScriptEditor", "Loaded { " + script_name + " }");
  script_is_loaded = true;
};



void UW::UI_ScriptEditor::guiScriptEditor(){
  float width = ImGui::GetContentRegionAvail().x;
  float height = ImGui::GetContentRegionAvail().y - 50.0f;
  
  ImGui::SeparatorText("Script Editor");
  ImGui::Text("Script: %s", script_name.c_str());
  
  if(ImGui::InputTextMultiline("##Script Content", buffer, UW::Config::SCRIPT_EDITOR_BUFFER_SIZE, ImVec2(width, height), ImGuiInputTextFlags_WordWrap)){
    script_is_updated = true;
    last_save_time = std::chrono::steady_clock::now();
  };

  if(script_is_updated){
    auto now = std::chrono::steady_clock::now();
    
    if (now - last_save_time >= save_cooldown_duration) {
      
      DataSerializer::get().saveScript(script_name, buffer);
      Logger::get().info("UI_ScriptEditor", "Auto-Saved { " + script_name + " }");
      last_save_time = now;
      script_is_updated = false; 
    }
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_ScriptEditor::ScriptEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiScriptLoad(script_name);
    guiScriptEditor();
  };
};



std::string UW::UI_ScriptEditor::getName(){
  return script_name;
};



#endif
