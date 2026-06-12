#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);






void UW::DataSerializer::scanCmrcDirectory(
  const cmrc::embedded_filesystem& fs,
  const std::string& current_path,
  const std::string& pattern_str,
  std::vector<std::string>& out_mesh_files){
  std::regex pattern(pattern_str);

  for (const auto& entry : fs.iterate_directory(current_path)) {
    std::string entry_path = current_path + (current_path.back() == '/' ? "" : "/") + entry.filename();
    
    if (entry.is_directory()) scanCmrcDirectory(fs, entry_path, pattern_str, out_mesh_files);
    else if (entry.is_file() && std::regex_search(entry.filename(), pattern)) out_mesh_files.emplace_back(entry_path);
  };
};



void UW::DataSerializer::saveAllObjects(std::vector<UW::GameObject> &objects) {
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME << std::endl;
    return;
  };

  size_t obj_size = objects.size();
  outFile.write(reinterpret_cast<const char*>(&obj_size), sizeof(obj_size));

  for (const auto& object : objects) {
    UW::GameObjectRecord record; 
    
    record.name = object.name;
    record.mesh = object.mesh;
    record.shader = object.shader;
    record.position = object.position;
    record.rotation = object.rotation;
    record.scale = object.scale;
    record.textures = object.textures;
    record.materials = object.materials;

    outFile << record;
  };
  
  outFile.close();
};



void UW::DataSerializer::loadAllObjects(std::vector<UW::GameObject> &objects) {
  try{
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      std::cerr << "CMRC Error: File not found in resources: " << resourcePath << std::endl;
      return;
    }

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);
    
    objects.clear();

    size_t objectCount = 0;
    inFile.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));


    for (size_t i = 0; i < objectCount; ++i) {
      UW::GameObjectRecord record;
      if (inFile >> record) {
        GameObject object(record.name, record.mesh, record.shader);
        object.position = record.position;
        object.rotation = record.rotation;
        object.scale = record.scale;
        object.textures = std::move(record.textures);
        object.materials = std::move(record.materials);

        objects.push_back(std::move(object));
      } else {
        std::cerr << "Error: File format corrupted at object index " << i << std::endl;
        break;
      };
    };
  } catch(const std::exception& e){
    std::cerr << "Exception caught during CMRC GameObject load: " << e.what() << std::endl;
  };
};



void UW::DataSerializer::saveAllMaterials(UW::Materials &materials) {
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME << std::endl;
    return;
  };

  size_t mat_size = materials.size();
  outFile.write(reinterpret_cast<const char*>(&mat_size), sizeof(mat_size));

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



void UW::DataSerializer::loadAllMaterials(UW::Materials &materials) {
  try{
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      std::cerr << "CMRC Error: File not found in resources: " << resourcePath << std::endl;
      return;
    };

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);

    materials.clear();

    size_t materialCount = 0;
    inFile.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

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

  } catch(const std::exception& e){
    std::cerr << "Exception caught during CMRC GameObject load: " << e.what() << std::endl;
  }
};



void UW::DataSerializer::saveAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME << std::endl;
    return;
  };

  unsigned int size = 0;
  for (const auto& el : lights) size += el.second.size();

  outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));

  for (const auto& el : lights) {
    const UW::Lights& lights_data = el.second;
    for(int i = 0; i < lights_data.size(); i++){
      UW::Light light = lights_data.get(i);
      UW::LightsRecord record;
       
      record.name = el.first;
      record.position = light.position;
      record.color = light.color;
      record.strength = light.strength;

      outFile << record;
    }
  };
  
  outFile.close();
};



void UW::DataSerializer::loadAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  try{
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      std::cerr << "CMRC Error: File not found in resources: " << resourcePath << std::endl;
      return;
    };

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);

    size_t lightCount = 0;
    inFile.read(reinterpret_cast<char*>(&lightCount), sizeof(lightCount));

    for (size_t i = 0; i < lightCount; ++i) {
      UW::LightsRecord record;
      if (inFile >> record) {
        UW::Light light(record.position, record.color, record.strength);
        lights[record.name].emplace_back(light);
        lights[record.name].compile();
      } else {
        std::cerr << "Error: File format corrupted at object index " << i << std::endl;
        break;
      };
    };
  } catch(const std::exception& e){
    std::cerr << "Exception caught during CMRC GameObject load: " << e.what() << std::endl;
  }
};



void UW::DataSerializer::saveMesh(const std::string &name, const CW::Renderer::Mesh &mesh){
  std::string folder_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::MESHES_FOLDER;
  std::string file_path   = folder_path + name + UW::Config::MESH_EXTENSION;

  std::filesystem::create_directories(folder_path);

  std::ofstream outFile(file_path, std::ios::binary);
  if (!outFile.is_open()) return;

  UW::MeshRecord record;
  record.name = name;
  record.indices = mesh.getIndices();

  auto reg = mesh.getDataRegister();

  record.mesh_data.clear();
  record.mesh_data.reserve(reg.size());

  for (const auto& [location, mesh_data_instance] : reg){
    UW::MeshRecord::MeshDataRecord e;
    e.key             = location;
    e.dimension       = mesh_data_instance.getDimension();
    e.size_of_element = mesh_data_instance.getSizeOfElement();
    e.size            = mesh_data_instance.getSize();
    e.type            = mesh_data_instance.getType();

    size_t byte_count = e.size;

    const uint8_t* raw = reinterpret_cast<const uint8_t*>(mesh_data_instance.getRawData());
    e.data.assign(raw, raw + byte_count);

    record.mesh_data.emplace_back(std::move(e));
  };

  outFile << record;
};



void UW::DataSerializer::loadMesh(const std::string& path_to_mesh, std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  try{
    auto fs = cmrc::GameData::get_filesystem();
    auto embedded_file = fs.open(path_to_mesh);
    std::string data_str(embedded_file.begin(), embedded_file.end());
    std::stringstream inFile(data_str);

    UW::MeshRecord record;
    if (!(inFile >> record)) return;

    meshes.erase(record.name);
    
    CW::Renderer::Mesh engine_mesh;
    
    engine_mesh.addIndices(record.indices);

    std::unordered_set<unsigned int> loaded_layouts;

    for (const auto& e : record.mesh_data){
      if (e.data.empty()) continue;

      if (loaded_layouts.count(e.key)) continue;
      loaded_layouts.insert(e.key);

      if (e.key == 0 && e.type == GL_FLOAT) {
        std::vector<float> vertices(e.data.size() / sizeof(float));
        std::memcpy(vertices.data(), e.data.data(), e.data.size());
        engine_mesh.addVertices(vertices, e.dimension, e.key);
      } 
      else {
        UW::Utils::uploadBufferByType(engine_mesh, e.type, e.data, e.dimension, e.key);
      };
    };

    meshes[record.name] = std::move(engine_mesh);
  }
  catch (const std::exception& e) {
    std::cerr << "[MeshLoad] CMRC EXCEPTION: " << e.what() << "\n";
  };
};



void UW::DataSerializer::saveAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  for (const auto& [mesh_name, mesh_instance] : meshes){
    saveMesh(mesh_name, mesh_instance);
  };
};



void UW::DataSerializer::loadAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string meshes_root = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::MESHES_FOLDER;

    if (!fs.exists(meshes_root)) return;

    std::vector<std::string> mesh_files;
    scanCmrcDirectory(fs, meshes_root, "\.msh$", mesh_files);

    for (const auto& file_path : mesh_files){
      loadMesh(file_path, meshes);
    };
  }
  catch (const std::exception& e) {
    std::cerr << "[MeshLoad] CMRC EXCEPTION: " << e.what() << "\n";
  };
};



void UW::DataSerializer::saveShaders(const std::string &path_to_shader, GLuint type){
  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + path_to_shader + "/" + UW::Config::SHADER_TYPE_TO_NAME[type];
  std::string source = Resources::get().getShader(path_to_shader).getRegisterShader().at(type).getSource();
  
  try {
    std::filesystem::path p(local_path);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "Filesystem error while creating directories: " << e.what() << std::endl;
    return;
  };

  std::ofstream outFile(local_path);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << local_path << std::endl;
    return;
  };

  outFile << source << "\n";
  
  outFile.close();
};






void UW::DataSerializer::saveAll(std::vector<UW::GameObject> &objects){
  saveAllObjects(objects);
  saveAllMaterials(Resources::get().materials);
  saveAllLights(Resources::get().lights);
  saveAllMeshes(Resources::get().meshes);
};



void UW::DataSerializer::loadAll(std::vector<UW::GameObject> &objects){
  loadAllMeshes(Resources::get().meshes);
  loadAllLights(Resources::get().lights);
  loadAllMaterials(Resources::get().materials);
  loadAllObjects(objects);
};
