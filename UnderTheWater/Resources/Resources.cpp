#include "Resources.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(assets);



std::ostream& UW::operator<<(std::ostream& os, const UW::MaterialsRecord& record) {
  os << record.name << "\n" 
     << record.albedo.x << " " << record.albedo.y << " " << record.albedo.z << "\n"
     << record.metallic << "\n"
     << record.roughness << "\n" 
     << record.emission_color.x << " " << record.emission_color.y << " " << record.emission_color.z << "\n"
     << record.emission_strength << "\n"
     << record.ambient_occlusion << "\n";

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::MaterialsRecord& record) {
  if (!std::getline(is, record.name)) return is;

  if (!(is >> record.albedo.x >> record.albedo.y >> record.albedo.z
           >> record.metallic
           >> record.roughness
           >> record.emission_color.x >> record.emission_color.y >> record.emission_color.z
           >> record.emission_strength
           >> record.ambient_occlusion)) {
    return is;
  }
  
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  return is;
};



void UW::Resources::save() {
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME << std::endl;
    return;
  };

  outFile << materials.size() << "\n";

  for (auto& el : materials.getMaterialReg()) {
    UW::MaterialsRecord record; 
    UW::Material material = el.second;

    record.name = el.first;
    record.albedo = material.albedo;
    record.metallic = material.metallic;
    record.roughness = material.roughness;
    record.emission_color = material.emission_color;
    record.emission_strength = material.emission_strength;
    record.ambient_occlusion = material.ambient_occlusion;

    outFile << record;
  };
  
  outFile.close();
};



void UW::Resources::load() {
  std::ifstream inFile(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
  if (!inFile.is_open()) {
    std::cerr << "Failed to open file for loading: " << UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME << std::endl;
    return;
  };

  materials.clear();

  size_t materialCount = 0;
  if (!(inFile >> materialCount)) return;
  inFile.ignore();

  for (size_t i = 0; i < materialCount; ++i) {
    UW::MaterialsRecord record;
    if (inFile >> record) {
      Material material;
      material.albedo = record.albedo;
      material.metallic = record.metallic;
      material.roughness = record.roughness;
      material.emission_color = record.emission_color;
      material.emission_strength = record.emission_strength;
      material.ambient_occlusion = record.ambient_occlusion;

      materials.emplace_back(record.name, std::move(material));
    } else {
      std::cerr << "Error: File format corrupted at object index " << i << std::endl;
      break;
    };
  };

  inFile.close();
};







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
  materials.emplace_back("terrain",
    {glm::vec3(255, 242, 111) / 255.0f,
    0.0f, 0.60f, glm::vec3(0.0f), 0.0f, 1.0f});



  // ===================== //
  // ======= Water ======= //
  // ===================== //
  materials.emplace_back("water",
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
