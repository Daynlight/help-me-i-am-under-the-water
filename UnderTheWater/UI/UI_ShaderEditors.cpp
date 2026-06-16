#include "UI_ShaderEditors.h"
#ifndef PRODUCTION



UW::UI_ShaderEditor::UI_ShaderEditor(CW::Gui::Gui& gui, const std::string& name, GLenum type)
  :gui(gui), shader_name(name), shader_type(type){

  Logger::get().info("UI_ShaderEditor", "Opened { " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type] + " }");
  gui.addWindow("Shader Editor " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type], shaderEditorGui());
};



UW::UI_ShaderEditor::~UI_ShaderEditor(){
  gui.deleteWindow("Shader Editor " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type]);
  Logger::get().info("UI_ShaderEditor", "Closed { " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type] + " }");
};



void UW::UI_ShaderEditor::guiShaderLoad(const std::string& name, GLenum type){
  if(shader_is_loaded) return;

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


  Logger::get().info("UI_ShaderEditor", "Loaded { " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type] + " }");
  shader_is_loaded = true;
};



void UW::UI_ShaderEditor::guiShaderEditor(){
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
    DataSerializer::get().saveShaders(shader_name, shader_type);

    Logger::get().info("UI_ShaderEditor", "Saved { " + shader_name + " : " + UW::Config::SHADER_TYPE_TO_NAME[shader_type] + " }");
  };
};



inline std::function<void(CW::Renderer::iRenderer *window)> UW::UI_ShaderEditor::shaderEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiShaderLoad(shader_name, shader_type);
    guiShaderEditor();
  };
};



std::string UW::UI_ShaderEditor::getName(){
  return shader_name;
};


GLenum UW::UI_ShaderEditor::getType(){
  return shader_type;
};

#endif