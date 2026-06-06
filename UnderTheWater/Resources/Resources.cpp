#include "Resources.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);



UW::Resources& UW::Resources::get(){
  static Resources instance; 
  return instance;
};



UW::Resources::Resources(){
  initMeshes();
  initTextures();
  initShaders();
  initMaterials();
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



void UW::Resources::initTextures(){
  auto fs = cmrc::assets::get_filesystem();

  // ======================= //
  // ======= Terrain ======= //
  // ======================= //
  {
    auto file = fs.open("Assets/Terrain/Terrain.png");
    const unsigned char* data_ptr = reinterpret_cast<const unsigned char*>(file.begin());

    CW::Renderer::TextureLoader loader(data_ptr, file.size());
    textures["heightmap"].compile(loader.data);
  };


  
  // ====================== //
  // ======= SkyBox ======= //
  // ====================== //
  {
    auto file = fs.open("Assets/Skybox/Skybox.png");
    const unsigned char* data_ptr = reinterpret_cast<const unsigned char*>(file.begin());
    
    CW::Renderer::TextureLoader loader(data_ptr, file.size());
    textures["sky_box"].compile(loader.data);
  };
};



void UW::Resources::initShaders(){
  // ======================= //
  // ======= Terrain ======= //
  // ======================= //
  shaders["terrain"].setVertexShader(TerrainShader::vertex);
  shaders["terrain"].setFragmentShader(TerrainShader::fragment);
  shaders["terrain"].setShader(TerrainShader::tessellation, GL_TESS_CONTROL_SHADER);
  shaders["terrain"].setShader(TerrainShader::tessellation_evaluation, GL_TESS_EVALUATION_SHADER);



  // ===================== //
  // ======= Water ======= //
  // ===================== //
  shaders["water"].setVertexShader(WaterShader::vertex);
  shaders["water"].setFragmentShader(WaterShader::fragment);
  shaders["water"].setShader(WaterShader::tessellation, GL_TESS_CONTROL_SHADER);
  shaders["water"].setShader(WaterShader::tessellation_evaluation, GL_TESS_EVALUATION_SHADER);



  // ====================== //
  // ======= SkyBox ======= //
  // ====================== //
  shaders["sky_box"].setVertexShader(SkyBoxShader::vertex);
  shaders["sky_box"].setFragmentShader(SkyBoxShader::fragment);



  // ============================ //
  // ======= Testing Cube ======= //
  // ============================ //
  shaders["testing"].setVertexShader(TestingCubeShader::vertex);
  shaders["testing"].setFragmentShader(TestingCubeShader::fragment);
};



void UW::Resources::initMaterials(){
  // ======================= //
  // ======= Terrain ======= //
  // ======================= //
  materials.emplace_back("Terrain",
    {glm::vec3(255, 242, 111) / 255.0f,
    0.0f, 0.60f, glm::vec3(0.0f), 0.0f, 1.0f});



  // ===================== //
  // ======= Water ======= //
  // ===================== //
  materials.emplace_back("Water",
    {glm::vec3(52, 114, 245) / 255.0f,
    0.0f, 0.44f, glm::vec3(0.0f), 0.0f, 1.0f});
};



void UW::Resources::initLights(){
  // ====================== //
  // ======= Static ======= //
  // ====================== //
  UW::Light light = UW::Light({4347.0f, 1095.0f, 2547.0f}, {1.0f, 0.7f, 0.5f}, 2.0f);
  lights["static"].emplace_back(light);
  lights["static"].compile();
};
