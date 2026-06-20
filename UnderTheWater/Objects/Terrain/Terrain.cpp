// Help me I'am Under The Water
// Copyright 2025 Daynlight
// Licensed under the Apache License, Version 2.0.
// See LICENSE file for details.



#include "Terrain.h"



UW::Terrain::Terrain(){
  generateChunks();
  mesh_id = Resources::get().meshes.get_id("terrain_chunk");
  onLoad();
};



UW::Terrain::~Terrain(){
  onDestroy();
};



void UW::Terrain::onLoad(){

};



void UW::Terrain::onDestroy(){

};



void UW::Terrain::onUpdate(float delta_time){
  
};



void UW::Terrain::onFixedUpdate(float fixed_delta_time){

};



void UW::Terrain::render(CW::Renderer::Renderer* renderer, Camera& culling_camera, Camera& render_camera, CW::Renderer::Uniform& shadows_uniform){
  if(Resources::get().meshes.validateVersion(mesh_version)){
    mesh_version = Resources::get().meshes.getLatestsVersion();
    mesh_id = Resources::get().meshes.get_id("terrain_chunk");
  };

  uniform["projection"]->set(render_camera.transformation(renderer));
  uniform["view"]->set(glm::mat4(1.0f));
  uniform["cameraPosition"]->set<glm::vec3>(culling_camera.position);
  uniform["lightCount"]->set<int>(Resources::get().lights.size());

  uniform["tessBound"]->set<glm::vec2>(UW::Config::TESS_BOUND);
  uniform["mapSize"]->set<glm::vec2>(map_size);
  uniform["maxHeight"]->set<float>(UW::Config::MAX_HEIGHT);
  uniform["distanceCoefficient"]->set<float>(UW::Config::TESS_DISTANCE_COFF);
  uniform["uTexture"]->set<int>(0);
  uniform["material_id"]-> set<int>(Resources::get().materials.translate_material("terrain"));

  Resources::get().getShader("Terrain").getUniforms().emplace_back(&uniform);
  Resources::get().getShader("Terrain").getUniforms().emplace_back(&shadows_uniform);

  Resources::get().getTexture("Terrain/heightmap.png").bind(0);
  glPatchParameteri(GL_PATCH_VERTICES, 4);
  
  for (auto& c : chunks){
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(c.x * UW::Config::CHUNK_SIZE, 0.0f, c.y * UW::Config::CHUNK_SIZE));
    model = glm::scale(model, glm::vec3(UW::Config::CHUNK_SIZE));
    
    if(isVisible(culling_camera.transformation(renderer), model, Resources::get().meshes[mesh_id])){
      uniform["model"]->set<glm::mat4>(model);
      
      Resources::get().getShader("Terrain").bind();
      Resources::get().meshes[mesh_id].render(GL_PATCHES);
    };
  };

  Resources::get().getShader("Terrain").unbind();
  Resources::get().getTexture("Terrain/heightmap.png").unbind();

  Resources::get().getShader("Terrain").getUniforms().clear();
};



void UW::Terrain::generateChunks(){
  chunks.clear();
  chunks.reserve((2 * UW::Config::CHUNK_RADIUS + 1) * (2 * UW::Config::CHUNK_RADIUS + 1));
  
  int radius = UW::Config::CHUNK_RADIUS;
  
  for (int x = -radius; x <= radius; x++){
    for (int z = -radius; z <= radius; z++){
      glm::vec2 position = glm::vec2(x, z);
      chunks.emplace_back(position);
    };
  };

  map_size = glm::vec2(UW::Config::CHUNK_SIZE * (UW::Config::CHUNK_RADIUS * 2.0f + 1.0f));
};



bool UW::Terrain::isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh){
  const float epsilonHeight = 1.0f;

  glm::vec3 localMin(0.0f, -epsilonHeight, 0.0f);
  glm::vec3 localMax(1.0f,  epsilonHeight, 1.0f);

  glm::vec3 corners[8] = {
    glm::vec3(model * glm::vec4(localMin.x, localMin.y, localMin.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMax.x, localMin.y, localMin.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMin.x, localMax.y, localMin.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMax.x, localMax.y, localMin.z, 1.0f)),

    glm::vec3(model * glm::vec4(localMin.x, localMin.y, localMax.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMax.x, localMin.y, localMax.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMin.x, localMax.y, localMax.z, 1.0f)),
    glm::vec3(model * glm::vec4(localMax.x, localMax.y, localMax.z, 1.0f))
  };

  glm::vec3 aabbMin = corners[0];
  glm::vec3 aabbMax = corners[0];

  for (int i = 1; i < 8; i++){
    aabbMin = glm::min(aabbMin, corners[i]);
    aabbMax = glm::max(aabbMax, corners[i]);
  };

  glm::mat4 m = culling_camera_transform;

  glm::vec4 planes[6];

  // Left
  planes[0] = glm::vec4(
    m[0][3] + m[0][0],
    m[1][3] + m[1][0],
    m[2][3] + m[2][0],
    m[3][3] + m[3][0]);

  // Right
  planes[1] = glm::vec4(
    m[0][3] - m[0][0],
    m[1][3] - m[1][0],
    m[2][3] - m[2][0],
    m[3][3] - m[3][0]);

  // Bottom
  planes[2] = glm::vec4(
    m[0][3] + m[0][1],
    m[1][3] + m[1][1],
    m[2][3] + m[2][1],
    m[3][3] + m[3][1]);

  // Top
  planes[3] = glm::vec4(
    m[0][3] - m[0][1],
    m[1][3] - m[1][1],
    m[2][3] - m[2][1],
    m[3][3] - m[3][1]);

  // Near
  planes[4] = glm::vec4(
    m[0][3] + m[0][2],
    m[1][3] + m[1][2],
    m[2][3] + m[2][2],
    m[3][3] + m[3][2]);

  // Far
  planes[5] = glm::vec4(
    m[0][3] - m[0][2],
    m[1][3] - m[1][2],
    m[2][3] - m[2][2],
    m[3][3] - m[3][2]);

  for (int i = 0; i < 6; i++){
    float length = glm::length(glm::vec3(planes[i]));
    if (length > 0.0f) planes[i] /= length;
  };

  for (int i = 0; i < 6; i++){
    glm::vec3 normal = glm::vec3(planes[i]);

    glm::vec3 positiveVertex;

    positiveVertex.x = (normal.x >= 0.0f) ? aabbMax.x : aabbMin.x;
    positiveVertex.y = (normal.y >= 0.0f) ? aabbMax.y : aabbMin.y;
    positiveVertex.z = (normal.z >= 0.0f) ? aabbMax.z : aabbMin.z;

    float distance = glm::dot(normal, positiveVertex) + planes[i].w;

    if (distance < 0.0f) return false;
  };

  return true;
};
