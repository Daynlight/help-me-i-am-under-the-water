#include "Resources.h"
#include "DataSerializer/DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



UW::Resources& UW::Resources::get(){
  static Resources instance; 
  return instance;
};



UW::Resources::Resources(){
  initMeshes();
  initLights();
};



UW::Resources::~Resources(){
  destroy();
};



void UW::Resources::destroy(){
  meshes.clear();
  textures.clear();
  shaders.clear();
  materials.destroy();
  lights.destroy();
  lights.clear();
};



CW::Renderer::Texture &UW::Resources::getTexture(const std::string &path_to_asset){
  auto it = textures.find(path_to_asset);
  
  if (it != textures.end()) {
    return it->second;
  }

  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::TEXTURES_FOLDER + path_to_asset;

  try {
    auto fs = cmrc::GameData::get_filesystem();
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

  return textures[UW::Config::DEFAULT_TEXTURE]; 
};



CW::Renderer::Shader &UW::Resources::getShader(const std::string &path_to_asset){
  auto it = shaders.find(path_to_asset);
  
  if (it != shaders.end()) {
    return it->second;
  }

  DataSerializer::get().loadShader(path_to_asset);
  
  auto ita = shaders.find(path_to_asset);
  
  if (ita != shaders.end()) {
    return ita->second;
  };
  
  return shaders[UW::Config::DEFAULT_SHADER];
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

  CW::Renderer::Mesh terrain_mesh;
  terrain_mesh.addVertices(vertices, 3);
  terrain_mesh.addIndices({0,1,2,3});
  meshes.emplace_back("terrain_chunk", std::move(terrain_mesh));



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

  CW::Renderer::Mesh sky_box_mesh;
  sky_box_mesh.addVertices(vertices, 3, 0);
  sky_box_mesh.addIndices(indices);
  meshes.emplace_back("sky_box", std::move(sky_box_mesh));



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
  CW::Renderer::Mesh default_mesh;
  default_mesh.addVertices(vertices, 3, 0);
  default_mesh.setData<GLfloat>(normals, 3, 1);
  default_mesh.setData<GLfloat>(uvs, 2, 2);
  default_mesh.setData<GLint>(mat_id, 1, 3);
  default_mesh.addIndices(indices);
  meshes.emplace_back(UW::Config::DEFAULT_MESH, std::move(default_mesh));

};



void UW::Resources::initLights(){
  UW::Light light(glm::vec3(0, 1000, 0), glm::vec3(1.0f, 1.0f,1.0f), 2.0f);
  lights.emplace_back({light});
  lights.compile();
};