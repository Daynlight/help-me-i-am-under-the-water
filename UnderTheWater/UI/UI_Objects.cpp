// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "UI_Objects.h"

#ifndef PRODUCTION



UW::UI_Objects::UI_Objects(CW::Gui::Gui& gui, CW::Renderer::Renderer& window, UW::Scene& scene)
  :gui(gui), window(window), scene(scene) {};



UW::UI_Objects::~UI_Objects(){
};



void UW::UI_Objects::uiControl(){
  if(guiSettings.objectExplorerWindowOn){
    Logger::get().info("UI", "Opening Object Explorer GUI");
    gui.addWindow("Object Explorer", objectExplorerGui());
  }
  else{
    Logger::get().info("UI", "Closing Object Explorer GUI");
    gui.deleteWindow("Object Explorer");
  };

  if(guiSettings.objectEditorWindowOn){
    Logger::get().info("UI", "Opening Object Editor GUI");
    gui.addWindow("Object Editor", objectEditorGui());
  }
  else{
    Logger::get().info("UI", "Closing Object Explorer GUI");
    gui.deleteWindow("Object Editor");
  };
};



void UW::UI_Objects::guiObjectList(){
  ImGui::SeparatorText("Object List");

  for(unsigned int id = 0; id < scene.object_manager.objects.size(); id++){
    std::string label = "- " + scene.object_manager.objects[id].game_object_data.name + "##(" + std::to_string(id) + ")";
    if(ImGui::Button(label.c_str())) guiSettings.object_id = id;
    
    label = "Delete##" + std::to_string(id);
    ImGui::SameLine();
    if(ImGui::Button(label.c_str())) {
      scene.object_manager.objects.erase(scene.object_manager.objects.begin() + id);
      Logger::get().warn("UI", "Deleted Object { " + scene.object_manager.objects[id].game_object_data.name + " }");
    };

    label = "Duplicate##" + std::to_string(id);
    ImGui::SameLine();
    if(ImGui::Button(label.c_str())) {
      scene.object_manager.objects.emplace_back(GameObject(scene.object_manager.objects[id].game_object_data.name + "_copy", scene.object_manager.objects[id]));
      Logger::get().warn("UI", "Duplicated Object { " + scene.object_manager.objects[id].game_object_data.name + " }");
    };
  };

  if(ImGui::Button("Add new")) {
    scene.object_manager.objects.emplace_back(UW::GameObject("new object", "testing", "testing", {}, {}, {}, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f)));
    Logger::get().info("UI", "Added New Object { new object }");
  };
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Objects::objectExplorerGui(){
return [this](CW::Renderer::iRenderer *window){
  guiObjectList();
};
};



void UW::UI_Objects::guiObjectEditor(){
  ImGui::SeparatorText("Object Editor");
  if(guiSettings.object_id >= scene.object_manager.objects.size()) return;
  
  UW::GameObject& object = scene.object_manager.objects[guiSettings.object_id];

  char name_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(name_buffer, object.game_object_data.name.data(), object.game_object_data.name.size());
  name_buffer[object.game_object_data.name.size()] = '\0';
  ImGui::InputText("name", name_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  object.game_object_data.name = std::string(name_buffer + '\0');
  
  char mesh_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(mesh_buffer, object.game_object_data.mesh.data(), object.game_object_data.mesh.size());
  mesh_buffer[object.game_object_data.mesh.size()] = '\0';
  ImGui::InputText("mesh", mesh_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  if(!Resources::get().meshes.exists(mesh_buffer)) return;
  object.game_object_data.mesh = std::string(mesh_buffer + '\0');

  char shader_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
  memcpy(shader_buffer, object.game_object_data.shader.data(), object.game_object_data.shader.size());
  shader_buffer[object.game_object_data.shader.size()] = '\0';
  ImGui::InputText("shader", shader_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
  auto its = Resources::get().shaders.find(shader_buffer);
  if(its == Resources::get().shaders.end()) return;
  object.game_object_data.shader = std::string(shader_buffer + '\0');


  ImGui::InputFloat3("position: ", &object.game_object_data.position[0]);
  glm::vec3 position_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("position slider: ", &position_offset[0], -100.0f, 100.0f);
  object.game_object_data.position += position_offset * window.getWindowData()->delta_time;

  ImGui::InputFloat3("rotate: ", &object.game_object_data.rotation[0]);
  glm::vec3 rotate_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("rotate slider: ", &rotate_offset[0], -1.0f, 1.0f);
  object.game_object_data.rotation += rotate_offset * window.getWindowData()->delta_time;

  ImGui::InputFloat3("scale: ", &object.game_object_data.scale[0]);
  glm::vec3 scale_offset = glm::vec3(0.0f);
  ImGui::SliderFloat3("scale slider: ", &scale_offset[0], -100.0f, 100.0f);
  object.game_object_data.scale += scale_offset * window.getWindowData()->delta_time;


  ImGui::SeparatorText("Textures: ");
  for(int i = 0; i < object.game_object_data.textures.size(); i++){
    std::string label = "- texture (" + std::to_string(i) + ")";
    char texture_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
    memcpy(texture_buffer, object.game_object_data.textures[i].data(), object.game_object_data.textures[i].size());
    texture_buffer[object.game_object_data.textures[i].size()] = '\0';
    ImGui::InputText(label.c_str(), texture_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
    object.game_object_data.textures[i] = std::string(texture_buffer + '\0');
    
    ImGui::SameLine();
    label = "Delete texture##(" + std::to_string(i) + ")";
    if(ImGui::Button(label.c_str())) object.game_object_data.textures.erase(object.game_object_data.textures.begin() + i);
  };

  std::string label = "Add Texture (" + std::to_string(object.game_object_data.textures.size()) + ")";
  if(ImGui::Button(label.c_str())) object.game_object_data.textures.emplace_back("");


  ImGui::SeparatorText("Materials: ");
  for(int i = 0; i < object.game_object_data.materials.size(); i++){
    std::string label = "- material (" + std::to_string(i) + ")";
    
    char material_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
    memcpy(material_buffer, object.game_object_data.materials[i].data(), object.game_object_data.materials[i].size());
    material_buffer[object.game_object_data.materials[i].size()] = '\0';
    ImGui::InputText(label.c_str(), material_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE);
    if(!Resources::get().materials.find(material_buffer)) return;
    object.game_object_data.materials[i] = std::string(material_buffer + '\0');
    
    ImGui::SameLine();
    label = "Delete material##(" + std::to_string(i) + ")";
    if(ImGui::Button(label.c_str())) object.game_object_data.materials.erase(object.game_object_data.materials.begin() + i);
  };

  label = "Add material (" + std::to_string(object.game_object_data.materials.size()) + ")";
  if(ImGui::Button(label.c_str())) object.game_object_data.materials.emplace_back("new material");


  ImGui::SeparatorText("Scripts: ");
  for(int i = 0; i < object.scripts.size(); i++){
    std::string label = "- script (" + std::to_string(i) + ")";
    
    char script_buffer[UW::Config::OBJECT_NAME_BUFFER_SIZE];
    memcpy(script_buffer, object.scripts[i].getPath().data(), object.scripts[i].getPath().size());
    script_buffer[object.scripts[i].getPath().size()] = '\0';
    if(ImGui::InputText(label.c_str(), script_buffer, UW::Config::OBJECT_NAME_BUFFER_SIZE))
      object.scripts[i] = UW::GameObjectScriptRecord(std::string(script_buffer + '\0'));
    
    ImGui::SameLine();
    label = "Delete scripts##(" + std::to_string(i) + ")";
    if(ImGui::Button(label.c_str())) object.scripts.erase(object.scripts.begin() + i);
  };

  label = "Add script (" + std::to_string(object.scripts.size()) + ")";
  if(ImGui::Button(label.c_str())) {
    object.scripts.emplace_back(GameObjectScriptRecord("new script"));
  };


  ImGui::SeparatorText("Parameters");

  auto& params = object.game_object_data.parameters;

  for (auto it = params.begin(); it != params.end();) {
    const std::string& current_name = it->first;
    auto& param_value = it->second;

    ImGui::PushID(current_name.c_str());

    bool delete_triggered = false;
    bool rename_triggered = false;
    char name_buffer[128];
    strncpy(name_buffer, current_name.c_str(), sizeof(name_buffer) - 1);
    name_buffer[sizeof(name_buffer) - 1] = '\0';

    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::InputText("##ParamName", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
      rename_triggered = true;
    }
    if (ImGui::IsItemDeactivatedAfterEdit()) {
      rename_triggered = true;
    }

    ImGui::SameLine();

    int current_type_idx = static_cast<int>(param_value.index());
    ImGui::SetNextItemWidth(70.0f);
    if (ImGui::Combo("##ParamType", &current_type_idx, UW::gameObjectParameterTypeName, IM_ARRAYSIZE(UW::gameObjectParameterTypeName))) {
      switch (current_type_idx) {
        case 0: param_value = 0; break;
        case 1: param_value = 0.0f; break;
        case 2: param_value = false; break;
        case 3: param_value = glm::vec2(0.0f); break;
        case 4: param_value = glm::vec3(0.0f); break;
        case 5: param_value = std::string(""); break;
      }
    }

    ImGui::SameLine();

    std::visit([](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      ImGui::SetNextItemWidth(150.0f);
      
      if constexpr (std::is_same_v<T, int>) {
        ImGui::InputInt("##val", &arg);
      }
      else if constexpr (std::is_same_v<T, float>) {
        ImGui::DragFloat("##val", &arg, 0.05f);
      }
      else if constexpr (std::is_same_v<T, bool>) {
        ImGui::Checkbox("##val", &arg);
      }
      else if constexpr (std::is_same_v<T, glm::vec2>) {
        ImGui::DragFloat2("##val", &arg.x, 0.05f);
      }
      else if constexpr (std::is_same_v<T, glm::vec3>) {
        ImGui::DragFloat3("##val", &arg.x, 0.05f);
      }
      else if constexpr (std::is_same_v<T, std::string>) {
        char str_buffer[256];
        strncpy(str_buffer, arg.c_str(), sizeof(str_buffer) - 1);
        str_buffer[sizeof(str_buffer) - 1] = '\0';
        if (ImGui::InputText("##val", str_buffer, sizeof(str_buffer))) {
          arg = std::string(str_buffer);
        }
      }
    }, param_value);

    ImGui::SameLine();

    if (ImGui::Button("Delete")) {
      delete_triggered = true;
    }

    ImGui::PopID();

    if (delete_triggered) {
      it = params.erase(it);
    } 
    else if (rename_triggered && std::string(name_buffer) != current_name && !std::string(name_buffer).empty()) {
      std::string new_key = name_buffer;
      
      if (params.find(new_key) == params.end()) {
        params[new_key] = std::move(param_value);
        it = params.erase(it);
      } else {
        ++it;
      }
    } 
    else {
      ++it;
    }
  }

  ImGui::Spacing();

  std::string add_label = "Add Parameter (" + std::to_string(params.size()) + ")";
  if (ImGui::Button(add_label.c_str())) {
    std::string unique_new_name = "NewParameter_" + std::to_string(params.size());
    
    int safety_counter = 0;
    while(params.find(unique_new_name) != params.end()) {
      unique_new_name = "NewParameter_" + std::to_string(params.size() + (++safety_counter));
    }
    
    params[unique_new_name] = 0;
  }
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Objects::objectEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiObjectEditor();
  };
};

#endif
