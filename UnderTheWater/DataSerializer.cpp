#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);






UW::DataSerializer &UW::DataSerializer::get(){
  static DataSerializer instance;
  return instance;
};



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
    Logger::get().erro("DataSerializer", "Filesystem error while creating directories - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("DataSerializer", "Failed to open file for saving - " + UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
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

    Logger::get().info("DataSerializer", "Object saved { " + object.name + " }");
  };
  
  outFile.close();

  Logger::get().info("DataSerializer", "All Objects Had Been Saved");
};



void UW::DataSerializer::loadAllObjects(std::vector<UW::GameObject> &objects) {
  Logger::get().info("DataSerializer", "Loading all objects...");
  try{
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("DataSerializer", "CMRC - File not found in resources - " + resourcePath);
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
        Logger::get().info("DataSerializer", "Object loaded { " + record.name + " }");
      } else {
      Logger::get().erro("DataSerializer", "File format corrupted at object index " + std::to_string(i));
        break;
      };
    };
    Logger::get().info("DataSerializer", "All objects have been loaded");
  } catch(const std::exception& e){
    Logger::get().erro("DataSerializer", "Exception caught during CMRC GameObject load - " + std::string(e.what()));
  };
};



void UW::DataSerializer::saveAllMaterials(UW::Materials &materials) {
  Logger::get().info("DataSerializer", "Saving all materials...");
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("DataSerializer", "Filesystem error while creating directories - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("DataSerializer", "Failed to open file for saving - " + UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME);
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

    Logger::get().info("DataSerializer", "Material saved { " + el.first + " }");
  };
  
  outFile.close();
  Logger::get().info("DataSerializer", "All Materials Had Been Saved");
};



void UW::DataSerializer::loadAllMaterials(UW::Materials &materials) {
  Logger::get().info("DataSerializer", "Loading all materials...");
  try{
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::MATERIALS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("DataSerializer", "CMRC - File not found in resources - " + resourcePath);
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
        Logger::get().info("DataSerializer", "Material loaded { " + record.name + " }");
      } else {
        Logger::get().erro("DataSerializer", "File format corrupted at material index " + std::to_string(i));
        break;
      };
    };
    Logger::get().info("DataSerializer", "All materials have been loaded");

  } catch(const std::exception& e){
    Logger::get().erro("DataSerializer", "Exception caught during CMRC GameObject load - " + std::string(e.what()));
  }
};



void UW::DataSerializer::saveAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  Logger::get().info("DataSerializer", "Saving all lights...");
  try {
    std::filesystem::path p(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("DataSerializer", "Filesystem error while creating directories - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME, std::ios::binary);
  if (!outFile.is_open()) {
    Logger::get().erro("DataSerializer", "Failed to open file for saving - " + UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME);
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
  Logger::get().info("DataSerializer", "All lights have been saved");
};



void UW::DataSerializer::loadAllLights(std::unordered_map<std::string, UW::Lights> &lights) {
  Logger::get().info("DataSerializer", "Loading all lights...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string resourcePath = UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME;
    
    if (!fs.exists(resourcePath)) {
      Logger::get().erro("DataSerializer", "CMRC - File not found in resources - " + resourcePath);
      return;
    };

    auto embeddedFile = fs.open(resourcePath);
    std::string dataStr(embeddedFile.begin(), embeddedFile.end());
    std::stringstream inFile(dataStr);

    unsigned int lightCount = 0;
    inFile.read(reinterpret_cast<char*>(&lightCount), sizeof(lightCount));

    lights.clear();

    for (unsigned int i = 0; i < lightCount; ++i) {
      UW::LightsRecord record;
      if (inFile >> record) {
        UW::Light light(record.position, record.color, record.strength);
        lights[record.name].emplace_back(light);
      } else {
        Logger::get().erro("DataSerializer", "File format corrupted at light index " + std::to_string(i));
        break;
      };
    };

    for (auto& [name, lightGroup] : lights) lightGroup.compile();

    Logger::get().info("DataSerializer", "All lights have been loaded");
  } catch(const std::exception& e) {
    Logger::get().erro("DataSerializer", "Exception caught during CMRC Lights load - " + std::string(e.what()));
  }
}



void UW::DataSerializer::saveMesh(const std::string &name, const CW::Renderer::Mesh &mesh){
  Logger::get().info("DataSerializer", "Saving mesh: " + name);
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
  Logger::get().info("DataSerializer", "Mesh saved: " + name);
};



void UW::DataSerializer::loadMesh(const std::string& path_to_mesh, std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  Logger::get().info("DataSerializer", "Loading mesh: " + path_to_mesh);
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
    Logger::get().info("DataSerializer", "Mesh loaded: " + record.name);
  }
  catch (const std::exception& e) {
    Logger::get().erro("DataSerializer", "[MeshLoad] CMRC EXCEPTION: " + std::string(e.what()));
  };
};



void UW::DataSerializer::saveAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  Logger::get().info("DataSerializer", "Saving all meshes...");
  for (const auto& [mesh_name, mesh_instance] : meshes){
    saveMesh(mesh_name, mesh_instance);
  };
  Logger::get().info("DataSerializer", "All meshes have been saved");
};



void UW::DataSerializer::loadAllMeshes(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  Logger::get().info("DataSerializer", "Loading all meshes...");
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string meshes_root = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::MESHES_FOLDER;

    if (!fs.exists(meshes_root)) return;

    std::vector<std::string> mesh_files;
    scanCmrcDirectory(fs, meshes_root, "\.msh$", mesh_files);

    for (const auto& file_path : mesh_files){
      loadMesh(file_path, meshes);
    };
    Logger::get().info("DataSerializer", "All meshes have been loaded");
  }
  catch (const std::exception& e) {
    Logger::get().erro("DataSerializer", "[MeshLoad] CMRC EXCEPTION: " + std::string(e.what()));
  };
};



void UW::DataSerializer::saveShaders(const std::string &shader_name, GLuint type){
  Logger::get().info("DataSerializer", "Saving shader: " + shader_name + " type=" + std::to_string(type));
  std::string local_path = UW::Config::GAME_DATA_FOLDER + UW::Config::ASSETS_FOLDER + UW::Config::SHADERS_FOLDER + shader_name + "/" + UW::Config::SHADER_TYPE_TO_NAME[type];
  std::string source = Resources::get().getShader(shader_name).getRegisterShader().at(type).getSource();
  
  try {
    std::filesystem::path p(local_path);
    if (p.has_parent_path())
      std::filesystem::create_directories(p.parent_path());
  } catch (const std::filesystem::filesystem_error& e) {
    Logger::get().erro("DataSerializer", "Filesystem error while creating directories - " + std::string(e.what()));
    return;
  };

  std::ofstream outFile(local_path);
  if (!outFile.is_open()) {
    Logger::get().erro("DataSerializer", "Failed to open file for saving - " + local_path);
    return;
  };

  outFile << source << "\n";
  
  outFile.close();
  Logger::get().info("DataSerializer", "Shader saved: " + shader_name);
};



void UW::DataSerializer::loadShader(const std::string& shader_name){
  Logger::get().info("DataSerializer", "Loading shader: " + shader_name);
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
      Logger::get().warn("DataSerializer", "[LoadShader] CMRC Exception: " + std::string(e.what()));
    };

    // if (std::filesystem::exists(local_path + "/" + shader_name.first) && !std::filesystem::is_directory(local_path + "/" + shader_name.first)) {
    //   std::ifstream file(local_path + "/" + shader_name.first, std::ios::binary | std::ios::ate);
    //   if (file.is_open()) {
    //     std::streamsize size = file.tellg();
    //     file.seekg(0, std::ios::beg);
        
    //     std::vector<char> buffer(size);
    //     if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    //       const GLuint type = shader_name.second;
    //       std::string data_ptr = "";
    //       for(const char el : buffer) data_ptr += el;
    //       shader.setShader(data_ptr, type);
    //     };
    //   };
    // };
  };

  if(shader.getRegisterShader().size() != 0){
    Resources::get().shaders[shader_name] = std::move(shader);
    Logger::get().info("DataSerializer", "Shader loaded: " + shader_name);
  } else {
    Logger::get().info("DataSerializer", "No shader source found for: " + shader_name);
  };
};






void UW::DataSerializer::saveAll(std::vector<UW::GameObject> &objects){
  Logger::get().info("DataSerializer", "Saving all game data...");
  saveAllObjects(objects);
  saveAllMaterials(Resources::get().materials);
  saveAllLights(Resources::get().lights);
  saveAllMeshes(Resources::get().meshes);
  Logger::get().info("DataSerializer", "All game data has been saved");
};



void UW::DataSerializer::loadAll(std::vector<UW::GameObject> &objects){
  Logger::get().info("DataSerializer", "Loading all game data...");
  loadAllMeshes(Resources::get().meshes);
  loadAllLights(Resources::get().lights);
  loadAllMaterials(Resources::get().materials);
  loadAllObjects(objects);
  Logger::get().info("DataSerializer", "All game data has been loaded");
};
