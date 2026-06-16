#include "Materials.h"



UW::Material::Material(
  glm::vec3 albedo, 
  float metallic,
  float roughness,
  glm::vec3 emission_color,
  float emission_strength,
  float ambient_occlusion):
  albedo(albedo),
  metallic(metallic),
  roughness(roughness),
  emission_color(emission_color),
  emission_strength(emission_strength),
  ambient_occlusion(ambient_occlusion){};



UW::Materials::~Materials(){
  destroy();
};



void UW::Materials::genVectors(){
  materials.clear();
  material_translate.clear();

  for(const std::pair<std::string, Material>& el : material_reg){
    material_translate[el.first] = materials.size();
    materials.emplace_back(el.second);
  };
};



void UW::Materials::compile(){
  buffer.create();
  genVectors();
  buffer.set<Material>(materials);
  is_compiled = true;
};



void UW::Materials::destroy(){
  buffer.destroy();
  is_compiled = false;
};



void UW::Materials::bind(GLuint socket){
  if(!is_compiled) compile();

  buffer.bind(socket);
};



void UW::Materials::unbind(){
  if(!is_compiled) return;

  buffer.unbind();
};



unsigned int UW::Materials::translate_material(const std::string& name){
  return material_translate[name];
};



UW::Material& UW::Materials::operator[](const std::string& name){
  is_compiled = false;
  return material_reg[name];
};



const std::unordered_map<std::string, UW::Material>& UW::Materials::getMaterialReg(){
  return material_reg;
};



bool UW::Materials::find(const std::string& name){
  auto it = material_reg.find(name);
  if(it == material_reg.end()) return false;
  return true;
};



UW::Material UW::Materials::getMaterial(const std::string& name){
  return material_reg[name];
};



void UW::Materials::clear(){
  is_compiled = false;
  material_reg.clear();
  materials.clear();
};



void UW::Materials::erase(const std::string& name){
  is_compiled = false;
  material_reg.erase(name);
};



unsigned int UW::Materials::size() const {
  return material_reg.size();
};



void UW::Materials::emplace_back(const std::string& name, Material material){
  is_compiled = false;
  material_reg[name] = material;
};



void UW::Materials::emplace_back(std::initializer_list<std::pair<std::string, Material>> materials){
  is_compiled = false;
  for (std::pair<std::string, Material> el : materials) this->material_reg[el.first] = el.second;
};
