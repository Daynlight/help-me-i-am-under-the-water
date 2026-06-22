// Help me I'am Under The Water
// Copyright 2026 Daynlight
// Licensed under the GNU General, Version 3.0.
// See LICENSE file for details.



#include "Lights.h"



UW::Light::Light(glm::vec3 position, glm::vec3 color, float strength)
  :position(position), color(color), strength(strength) {};



UW::Lights::Lights(std::initializer_list<Light> lights)
  : lights(lights) {
  compile();
};



UW::Lights::~Lights(){
  destroy();
};



void UW::Lights::compile(){
  buffer.create();
  buffer.set<Light>(lights);
  is_compiled = true;
};



void UW::Lights::destroy(){
  buffer.destroy();
  is_compiled = false;
};



void UW::Lights::bind(GLuint socket){
  if(!is_compiled) compile();

  buffer.bind(socket);
};



void UW::Lights::unbind(){
  if(!is_compiled) return;

  buffer.unbind();
};



UW::Light& UW::Lights::operator[](unsigned int index){
  is_compiled = false;
  return lights[index];
};



UW::Light UW::Lights::get(unsigned int index) const{
  return lights[index];
};



void UW::Lights::clear(){
  is_compiled = false;
  lights.clear();
};



void UW::Lights::erase(unsigned int index){
  is_compiled = false;
  lights.erase(lights.begin() + index);
};



unsigned int UW::Lights::size() const {
  return lights.size();
};



void UW::Lights::emplace_back(Light light){
  is_compiled = false;
  lights.emplace_back(light);
};



void UW::Lights::emplace_back(std::initializer_list<Light> lights){
  is_compiled = false;
  for (Light el : lights) this->lights.emplace_back(el);
};
