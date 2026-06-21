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
};



std::function<void(CW::Renderer::iRenderer *window)> UW::UI_Objects::objectEditorGui(){
  return [this](CW::Renderer::iRenderer *window){
    guiObjectEditor();
  };
};

#endif
