#include "Resources.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);



UW::Resources& UW::Resources::get(){
  static Resources instance; 
  return instance;
};



UW::Resources::Resources(){
  initMeshes();
};



UW::Resources::~Resources(){
  destroy();
};



void UW::Resources::destroy(){
  meshes.clear();
  textures.clear();
  shaders.clear();
  materials.destroy();
  lights.clear();
};



void UW::Resources::initMeshes(){
  // ======================= //
  // ======= Terrain ======= //
  // ======================= //
  std::vector<float> vertices = {
    -1.0f, 0.0f, 1.0f,
    -1.0f, 0.0f, -1.0f,
    1.0f, 0.0f, -1.0f,
    1.0f, 0.0f, 1.0f
  };
  meshes["terrain_chunk"].addVertices(vertices, 3);
  meshes["terrain_chunk"].addIndices({0,1,2,3});



  // ====================== //
  // ======= SkyBox ======= //
  // ====================== //
  vertices = {         
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f
  };
  std::vector<unsigned int> indices = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3
  };
  meshes["sky_box"].addVertices(vertices, 3, 0);
  meshes["sky_box"].addIndices(indices);



  // ============================ //
  // ======= Testing Cube ======= //
  // ============================ //
  vertices = {         
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f
  };
  indices = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    5, 4, 7, 7, 6, 5,
    4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3
  };
  std::vector<GLfloat> normals = {
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
  };
  std::vector<GLfloat> uvs = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
  };
  std::vector<GLint> mat_id = {
    0, 0, 0, 0, 1, 1, 1, 1
  };
  meshes["testing"].addVertices(vertices, 3, 0);
  meshes["testing"].setData<GLfloat>(normals, 3, 1);
  meshes["testing"].setData<GLfloat>(uvs, 2, 2);
  meshes["testing"].setData<GLint>(mat_id, 1, 3);
  meshes["testing"].addIndices(indices);
};



CW::Renderer::Texture &UW::Resources::getTexture(const std::string &path_to_asset){
  auto it = textures.find(path_to_asset);
  
  if (it != textures.end()) {
    return it->second;
  }

  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + path_to_asset;

  try {
    auto fs = cmrc::assets::get_filesystem();
    auto file = fs.open(local_path); 
    
    const unsigned char* data_ptr = reinterpret_cast<const unsigned char*>(file.begin());
    CW::Renderer::TextureLoader loader(data_ptr, file.size());

    it = textures.emplace(path_to_asset, CW::Renderer::Texture()).first;
    it->second.compile(loader.data);
    
    return it->second;
  } catch (const std::runtime_error& e) {
  }

  if (std::filesystem::exists(local_path) && !std::filesystem::is_directory(local_path)) {
    std::ifstream file(local_path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
      std::streamsize size = file.tellg();
      file.seekg(0, std::ios::beg);

      std::vector<unsigned char> buffer(size);
      if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        CW::Renderer::TextureLoader loader(buffer.data(), size);
        
        it = textures.emplace(path_to_asset, CW::Renderer::Texture()).first;
        it->second.compile(loader.data);
        return it->second;
      }
    }
  }

  return textures["default_fallback"]; 
};



CW::Renderer::Shader &UW::Resources::getShader(const std::string &path_to_asset){
  auto it = shaders.find(path_to_asset);
  
  if (it != shaders.end()) {
    return it->second;
  }

  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + path_to_asset;
  CW::Renderer::Shader shader;

  for(auto& shader_name : UW::Config::SHADER_NAME_TO_TYPE){
    try {
      auto fs = cmrc::assets::get_filesystem();
      auto file = fs.open(local_path + "/" + shader_name.first); 
      
      const char* data_ptr = reinterpret_cast<const char*>(file.begin());
      const GLuint type = shader_name.second;
      shader.setShader(std::string(data_ptr), type);
      continue;
    } catch (const std::runtime_error& e) {};

    if (std::filesystem::exists(local_path + "/" + shader_name.first) && !std::filesystem::is_directory(local_path + "/" + shader_name.first)) {
      std::ifstream file(local_path + "/" + shader_name.first, std::ios::binary | std::ios::ate);
      if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<char> buffer(size);
        if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
          const GLuint type = shader_name.second;
          std::string data_ptr = "";
          for(const char el : buffer) data_ptr += el;
          shader.setShader(data_ptr, type);
        };
      };
    };
  };
  if(shader.getRegisterShader().size() != 0){
    shaders[path_to_asset] = std::move(shader);
    return shaders[path_to_asset];
  };
  
  return shaders["default_fallback"];
};
