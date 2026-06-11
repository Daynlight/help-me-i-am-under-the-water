#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);






void scanCmrchDirectory(
  const cmrc::embedded_filesystem& fs,
  const std::string& current_path,
  std::vector<std::string>& out_mesh_files)
{
  for (const auto& entry : fs.iterate_directory(current_path)) {
    std::string entry_path = current_path + (current_path.back() == '/' ? "" : "/") + entry.filename();
    
    if (entry.is_directory()) scanCmrchDirectory(fs, entry_path, out_mesh_files);
    else if (entry.is_file() && entry.filename() == "mesh.msh") out_mesh_files.push_back(entry_path);
  };
};



std::ostream& UW::operator<<(std::ostream& os, const UW::GameObjectRecord& record) {
  size_t name_sz = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_sz), sizeof(name_sz));
  if (name_sz > 0) os.write(record.name.data(), name_sz);

  size_t mesh_sz = record.mesh.size();
  os.write(reinterpret_cast<const char*>(&mesh_sz), sizeof(mesh_sz));
  if (mesh_sz > 0) os.write(record.mesh.data(), mesh_sz);

  size_t shader_sz = record.shader.size();
  os.write(reinterpret_cast<const char*>(&shader_sz), sizeof(shader_sz));
  if (shader_sz > 0) os.write(record.shader.data(), shader_sz);

  os.write(reinterpret_cast<const char*>(&record.position), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.rotation), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.scale), sizeof(glm::vec3));

  size_t tex_count = record.textures.size();
  os.write(reinterpret_cast<const char*>(&tex_count), sizeof(tex_count));
  for (const auto& tex : record.textures) {
    size_t tex_sz = tex.size();
    os.write(reinterpret_cast<const char*>(&tex_sz), sizeof(tex_sz));
    if (tex_sz > 0) os.write(tex.data(), tex_sz);
  };

  size_t mat_count = record.materials.size();
  os.write(reinterpret_cast<const char*>(&mat_count), sizeof(mat_count));
  for (const auto& mat : record.materials) {
    size_t mat_sz = mat.size();
    os.write(reinterpret_cast<const char*>(&mat_sz), sizeof(mat_sz));
    if (mat_sz > 0) os.write(mat.data(), mat_sz);
  };

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::GameObjectRecord& record) {
  size_t name_sz = 0;
  if (!is.read(reinterpret_cast<char*>(&name_sz), sizeof(name_sz))) return is;
  record.name.resize(name_sz);
  if (name_sz > 0) is.read(&record.name[0], name_sz);

  size_t mesh_sz = 0;
  is.read(reinterpret_cast<char*>(&mesh_sz), sizeof(mesh_sz));
  record.mesh.resize(mesh_sz);
  if (mesh_sz > 0) is.read(&record.mesh[0], mesh_sz);

  size_t shader_sz = 0;
  is.read(reinterpret_cast<char*>(&shader_sz), sizeof(shader_sz));
  record.shader.resize(shader_sz);
  if (shader_sz > 0) is.read(&record.shader[0], shader_sz);

  is.read(reinterpret_cast<char*>(&record.position), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.rotation), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.scale), sizeof(glm::vec3));

  size_t tex_count = 0;
  is.read(reinterpret_cast<char*>(&tex_count), sizeof(tex_count));
  
  if (tex_count > 10000) { 
    is.setstate(std::ios::failbit);
    return is;
  };

  record.textures.resize(tex_count);
  for (auto& tex : record.textures) {
    size_t tex_sz = 0;
    is.read(reinterpret_cast<char*>(&tex_sz), sizeof(tex_sz));
    tex.resize(tex_sz);
    if (tex_sz > 0) is.read(&tex[0], tex_sz);
  }

  size_t mat_count = 0;
  is.read(reinterpret_cast<char*>(&mat_count), sizeof(mat_count));
  
  if (mat_count > 10000) {
    is.setstate(std::ios::failbit);
    return is;
  };

  record.materials.resize(mat_count);
  for (auto& mat : record.materials) {
    size_t mat_sz = 0;
    is.read(reinterpret_cast<char*>(&mat_sz), sizeof(mat_sz));
    mat.resize(mat_sz);
    if (mat_sz > 0) is.read(&mat[0], mat_sz);
  };

  return is;
};



std::ostream& UW::operator<<(std::ostream& os, const UW::MaterialsRecord& record) {
  size_t name_sz = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_sz), sizeof(size_t));
  if(name_sz > 0) os.write(reinterpret_cast<const char*>(record.name.data()), name_sz);

  os.write(reinterpret_cast<const char*>(&record.albedo), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.metallic), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.roughness), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.emission_color), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.emission_strength), sizeof(float));
  os.write(reinterpret_cast<const char*>(&record.ambient_occlusion), sizeof(float));

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::MaterialsRecord& record) {
  size_t name_sz = 0;
  if (!is.read(reinterpret_cast<char*>(&name_sz), sizeof(name_sz))) return is;
  record.name.resize(name_sz);
  if (name_sz > 0) is.read(&record.name[0], name_sz);

  is.read(reinterpret_cast<char*>(&record.albedo), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.metallic), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.roughness), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.emission_color), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.emission_strength), sizeof(float));
  is.read(reinterpret_cast<char*>(&record.ambient_occlusion), sizeof(float));

  return is;
};


std::ostream& UW::operator<<(std::ostream& os, const UW::LightsRecord& record) {
  size_t name_sz = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_sz), sizeof(size_t));
  if(name_sz > 0) os.write(reinterpret_cast<const char*>(record.name.data()), name_sz);

  os.write(reinterpret_cast<const char*>(&record.position), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.color), sizeof(glm::vec3));
  os.write(reinterpret_cast<const char*>(&record.strength), sizeof(float));

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::LightsRecord& record) {
  size_t name_sz = 0;
  if (!is.read(reinterpret_cast<char*>(&name_sz), sizeof(name_sz))) return is;
  record.name.resize(name_sz);
  if (name_sz > 0) is.read(&record.name[0], name_sz);

  is.read(reinterpret_cast<char*>(&record.position), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.color), sizeof(glm::vec3));
  is.read(reinterpret_cast<char*>(&record.strength), sizeof(float));

  return is;
};



std::ostream& UW::operator<<(std::ostream& os, const UW::MeshRecord& record){
  size_t name_size = record.name.size();
  os.write(reinterpret_cast<const char*>(&name_size), sizeof(name_size));
  if (name_size > 0) os.write(record.name.data(), name_size);

  size_t index_count = record.indices.size();
  os.write(reinterpret_cast<const char*>(&index_count), sizeof(index_count));
  if (index_count > 0) os.write(reinterpret_cast<const char*>(record.indices.data()), index_count * sizeof(record.indices[0]));

  size_t buffer_count = record.mesh_data.size();
  os.write(reinterpret_cast<const char*>(&buffer_count), sizeof(buffer_count));

  for (const auto& e : record.mesh_data){
    os.write(reinterpret_cast<const char*>(&e.key), sizeof(e.key));
    os.write(reinterpret_cast<const char*>(&e.dimension), sizeof(e.dimension));
    os.write(reinterpret_cast<const char*>(&e.size_of_element), sizeof(e.size_of_element));
    os.write(reinterpret_cast<const char*>(&e.size), sizeof(e.size));
    os.write(reinterpret_cast<const char*>(&e.type), sizeof(e.type));

    size_t byte_count = e.data.size();
    os.write(reinterpret_cast<const char*>(&byte_count), sizeof(byte_count));
    if (byte_count > 0) os.write(reinterpret_cast<const char*>(e.data.data()), byte_count);
  };

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::MeshRecord& record){
  size_t name_size = 0;
  if (!is.read(reinterpret_cast<char*>(&name_size), sizeof(name_size))) return is;
  
  record.name.resize(name_size);
  if (name_size > 0) is.read(&record.name[0], name_size);

  size_t index_count = 0;
  is.read(reinterpret_cast<char*>(&index_count), sizeof(index_count));
  
  record.indices.resize(index_count);
  if (index_count > 0) is.read(reinterpret_cast<char*>(record.indices.data()), index_count * sizeof(record.indices[0]));

  size_t buffer_count = 0;
  is.read(reinterpret_cast<char*>(&buffer_count), sizeof(buffer_count));
  
  record.mesh_data.resize(buffer_count);

  for (auto& e : record.mesh_data){
    is.read(reinterpret_cast<char*>(&e.key), sizeof(e.key));
    is.read(reinterpret_cast<char*>(&e.dimension), sizeof(e.dimension));
    is.read(reinterpret_cast<char*>(&e.size_of_element), sizeof(e.size_of_element));
    is.read(reinterpret_cast<char*>(&e.size), sizeof(e.size));
    is.read(reinterpret_cast<char*>(&e.type), sizeof(e.type));

    size_t byte_count = 0;
    is.read(reinterpret_cast<char*>(&byte_count), sizeof(byte_count));
    
    e.data.resize(byte_count);
    if (byte_count > 0) is.read(reinterpret_cast<char*>(e.data.data()), byte_count);
  };

  return is;
};






void UW::DataSerializer::save(std::vector<UW::GameObject> &objects) {
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



void UW::DataSerializer::load(std::vector<UW::GameObject> &objects) {
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


void UW::DataSerializer::save(UW::Materials &materials) {
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



void UW::DataSerializer::load(UW::Materials &materials) {
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



void UW::DataSerializer::save(std::unordered_map<std::string, UW::Lights> &lights) {
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



void UW::DataSerializer::load(std::unordered_map<std::string, UW::Lights> &lights) {
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



void UW::DataSerializer::save(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  for (const auto& [mesh_name, mesh_instance] : meshes){
    std::string folder_path = UW::Config::GAME_DATA_FOLDER + "Assets/Meshes/" + mesh_name + "/";
    std::string file_path   = folder_path + "mesh.msh";

    std::filesystem::create_directories(folder_path);

    std::ofstream outFile(file_path, std::ios::binary);
    if (!outFile.is_open()) continue;

    UW::MeshRecord record;
    record.name = mesh_name;
    record.indices = mesh_instance.getIndices();

    auto reg = mesh_instance.getDataRegister();

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
};



void UW::DataSerializer::load(std::unordered_map<std::string, CW::Renderer::Mesh> &meshes){
  try {
    auto fs = cmrc::GameData::get_filesystem();
    std::string meshes_root = UW::Config::GAME_DATA_FOLDER + "Assets/Meshes";

    if (!fs.exists(meshes_root)) return;

    std::vector<std::string> mesh_files;
    scanCmrchDirectory(fs, meshes_root, mesh_files);

    for (const auto& file_path : mesh_files){
      auto embedded_file = fs.open(file_path);
      std::string data_str(embedded_file.begin(), embedded_file.end());
      std::stringstream inFile(data_str);

      UW::MeshRecord record;
      if (!(inFile >> record)) continue;

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
    };
  }
  catch (const std::exception& e) {
    std::cerr << "[MeshLoad] CMRC EXCEPTION: " << e.what() << "\n";
  };
};



void UW::DataSerializer::save(const std::string &path_to_shader, GLuint type){
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

