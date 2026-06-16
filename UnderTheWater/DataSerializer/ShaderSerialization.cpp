#include "ShaderSerialization.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);

#include "Resources/Resources.h"



#ifndef PRODUCTION
void UW::ShaderSerialization::save(const std::string &shader_name, GLuint type){
  Logger::get().info("ShaderSerialization", "Saving shader: " + shader_name + " type=" + std::to_string(type));
  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::SHADERS_FOLDER + shader_name + "/" + UW::Config::SHADER_TYPE_TO_NAME[type];
  std::string source = Resources::get().getShader(shader_name).getRegisterShader().at(type).getSource();
  
  try {
    std::filesystem::path p(local_path);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("ShaderSerialization", "Filesystem error while creating directories - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(local_path);
  if (!outFile.is_open()) {
    Logger::get().erro("ShaderSerialization", "Failed to open file for saving - " + local_path);
    return;
  };

  outFile << source << "\n";
  
  outFile.close();
  Logger::get().info("ShaderSerialization", "Shader saved: " + shader_name);
};
#endif



void UW::ShaderSerialization::load(const std::string& shader_name){
  Logger::get().info("ShaderSerialization", "Loading shader: " + shader_name);
  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::SHADERS_FOLDER + shader_name;
  CW::Renderer::Shader shader;

  for(auto& shader_name : UW::Config::SHADER_NAME_TO_TYPE){
    try {
      auto fs = cmrc::GameData::get_filesystem();
      auto file = fs.open(local_path + "/" + shader_name.first); 
      
      const char* data_ptr = reinterpret_cast<const char*>(file.begin());
      const GLuint type = shader_name.second;
      shader.setShader(std::string(data_ptr), type);
      continue;
    } catch (const std::runtime_error& e) {
      Logger::get().warn("ShaderSerialization", "[LoadShader] CMRC Exception: " + std::string(e.what()));
    };
  };

  if(shader.getRegisterShader().size() != 0){
    Resources::get().shaders[shader_name] = std::move(shader);
    Resources::get().shaders[shader_name].compile();
    Logger::get().info("ShaderSerialization", "Shader loaded: " + shader_name);
  } else {
    Logger::get().info("ShaderSerialization", "No shader source found for: " + shader_name);
  };
};



void UW::ShaderSerialization::loadAll() {
  Logger::get().info("ShaderSerialization", "Scanning and loading all shaders...");
  
  std::string root_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::SHADERS_FOLDER;
  
  if (!root_path.empty() && root_path.back() == '/') {
    root_path.pop_back();
  }

  try {
    auto fs = cmrc::GameData::get_filesystem();
    
    for (auto&& entry : fs.iterate_directory(root_path))
      if (entry.is_directory())
        load(entry.filename());
    
    Logger::get().info("ShaderSerialization", "Finished loading all shaders.");
  } catch (const std::exception& e) {
    Logger::get().erro("ShaderSerialization", "[LoadAll] CMRC Exception: " + std::string(e.what()));
  }
}