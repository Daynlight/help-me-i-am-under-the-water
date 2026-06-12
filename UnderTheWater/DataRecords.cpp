#include "DataRecords.h"



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
