#include "DataSerializer.h"

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(GameData);



std::ostream& UW::operator<<(std::ostream& os, const UW::GameObjectRecord& record) {
  os << record.name << "\n" 
     << record.mesh << "\n" 
     << record.shader << "\n"
     << record.position.x << " " << record.position.y << " " << record.position.z << "\n"
     << record.rotation.x << " " << record.rotation.y << " " << record.rotation.z << "\n"
     << record.scale.x << " " << record.scale.y << " " << record.scale.z << "\n";

  os << record.textures.size() << "\n";
  for (const auto& tex : record.textures) os << tex << "\n";

  os << record.materials.size() << "\n";
  for (const auto& mat : record.materials) os << mat << "\n";

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::GameObjectRecord& record) {
  if (!std::getline(is, record.name) || 
      !std::getline(is, record.mesh) || 
      !std::getline(is, record.shader)) return is;

  if (!(is >> record.position.x >> record.position.y >> record.position.z >>
              record.rotation.x >> record.rotation.y >> record.rotation.z >>
              record.scale.x >> record.scale.y >> record.scale.z)) return is;

  size_t texSize = 0;
  if (!(is >> texSize)) return is;
  is.ignore();
  
  if (texSize > 10000) { 
    is.setstate(std::ios::failbit);
    return is;
  };

  record.textures.resize(texSize);
  for (size_t i = 0; i < texSize; ++i) std::getline(is, record.textures[i]);

  size_t matSize = 0;
  if (!(is >> matSize)) return is;
  is.ignore();
  
  if (matSize > 10000) {
    is.setstate(std::ios::failbit);
    return is;
  };

  record.materials.resize(matSize);
  for (size_t i = 0; i < matSize; ++i) std::getline(is, record.materials[i]);

  return is;
};



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


std::ostream& UW::operator<<(std::ostream& os, const UW::LightsRecord& record) {
  os << record.name << "\n" 
     << record.position.x << " " << record.position.y << " " << record.position.z << "\n"
     << record.color.x << " " << record.color.y << " " << record.color.z << "\n"
     << record.strength << "\n";

  return os;
};



std::istream& UW::operator>>(std::istream& is, UW::LightsRecord& record) {
  if (!std::getline(is, record.name)) return is;

  if (!(is >> record.position.x >> record.position.y >> record.position.z
           >> record.color.x >> record.color.y >> record.color.z
           >> record.strength)) {
    return is;
  }
  
  is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::OBJECTS_FILENAME << std::endl;
    return;
  };

  outFile << objects.size() << "\n";

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
    if (!(inFile >> objectCount)) return;
    inFile.ignore();

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

  std::ofstream outFile(UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME);
  if (!outFile.is_open()) {
    std::cerr << "Failed to open file for saving: " << UW::Config::GAME_DATA_FOLDER + UW::Config::LIGHTS_FILENAME << std::endl;
    return;
  };

  unsigned int size = 0;
  for (const auto& el : lights) size += el.second.size();

  outFile << size << "\n";

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

    lights.clear();

    size_t lightCount = 0;
    if (!(inFile >> lightCount)) return;
    inFile.ignore();

    for (size_t i = 0; i < lightCount; ++i) {
      UW::LightsRecord record;
      if (inFile >> record) {
        UW::Light light(record.position, record.color, record.strength);
        lights[record.name].emplace_back(light);
      } else {
        std::cerr << "Error: File format corrupted at object index " << i << std::endl;
        break;
      };
    };
  } catch(const std::exception& e){
    std::cerr << "Exception caught during CMRC GameObject load: " << e.what() << std::endl;
  }
};







void UW::DataSerializer::save(const std::string &path_to_asset, GLuint type){
  std::string local_path = "Assets/" + path_to_asset + "/" + UW::Config::SHADER_TYPE_TO_NAME[type];
  std::string source = Resources::get().getShader(path_to_asset).getRegisterShader().at(type).getSource();
  
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

