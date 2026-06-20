#include "GameObject.h"



UW::GameObject::GameObject(const std::string& name, const std::string& mesh, const std::string& shader, const std::vector<std::string>& materials, const std::vector<std::string>& textures, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
  :name(name), mesh(mesh), shader(shader), materials(materials), textures(textures), position(position), rotation(rotation), scale(scale) {
    mesh_id = Resources::get().meshes.get_id(mesh);
  };



UW::GameObject::~GameObject(){
};



void UW::GameObject::render(CW::Renderer::Renderer *renderer, Camera &culling_camera, Camera &render_camera, CW::Renderer::Uniform& shadows_uniform){
  if(Resources::get().meshes.validateVersion(mesh_version)){
    mesh_version = Resources::get().meshes.getLatestsVersion();
    mesh_id = Resources::get().meshes.get_id(this->mesh);
  };

  uniform["projection"]->set<glm::mat4>(render_camera.projection(renderer));
  uniform["view"]->set<glm::mat4>(render_camera.view(renderer));
  
  uniform["cameraPosition"]->set<glm::vec3>(culling_camera.position);
  uniform["lightCount"]->set<int>(Resources::get().lights.size());

  glm::vec3 pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 rotationMat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
  glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), this->scale);
  glm::mat4 preRotate = glm::translate(glm::mat4(1.0f), -pivotOffset);
  glm::mat4 postRotate = glm::translate(glm::mat4(1.0f), pivotOffset);
  glm::mat4 model = translationMat * postRotate * rotationMat * preRotate * scaleMat;

  if(isVisible(culling_camera.transformation(renderer), model, Resources::get().meshes[mesh_id])){
    uniform["model"]->set<glm::mat4>(model);

    for(unsigned int i = 0; i < textures.size(); i++){
      Resources::get().getTexture(this->textures[i]).bind(i);
      uniform[this->textures[i]]->set<int>(i);
    };
    
    Resources::get().getShader(this->shader).getUniforms().emplace_back(&uniform);
    Resources::get().getShader(this->shader).getUniforms().emplace_back(&shadows_uniform);
    
    Resources::get().getShader(this->shader).bind();
    
    std::vector<int> translation;
    for(std::string el : materials){
      translation.emplace_back(Resources::get().materials.translate_material(el));
    };

    GLint loc = glGetUniformLocation(Resources::get().getShader(shader).getShaderProgram(), "mat_translate");
    glUniform1iv(loc, translation.size(), translation.data());
    
    Resources::get().meshes[mesh_id].render();
    
    Resources::get().getShader(this->shader).unbind();

    for(unsigned int i = 0; i < textures.size(); i++) 
      Resources::get().getTexture(this->textures[i]).unbind();

    Resources::get().getShader(this->shader).getUniforms().clear();
  };
};



void UW::GameObject::onUpdate(float delta_time){
};



void UW::GameObject::onFixedUpdate(float fixed_delta_time){
};



bool UW::GameObject::isVisible(glm::mat4 culling_camera_transform, glm::mat4 model, const CW::Renderer::Mesh& mesh){
  auto cullingBox = mesh.getCullingBox();
  glm::vec3 localMin = glm::vec3(cullingBox[0][0], cullingBox[0][1], cullingBox[0][2]); 
  glm::vec3 localMax = glm::vec3(cullingBox[1][0], cullingBox[1][1], cullingBox[1][2]);

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
  planes[0] = glm::vec4(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0], m[3][3] + m[3][0]);
  // Right
  planes[1] = glm::vec4(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0], m[3][3] - m[3][0]);
  // Bottom
  planes[2] = glm::vec4(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1], m[3][3] + m[3][1]);
  // Top
  planes[3] = glm::vec4(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1], m[3][3] - m[3][1]);
  // Near
  planes[4] = glm::vec4(m[0][3] + m[0][2], m[1][3] + m[1][2], m[2][3] + m[2][2], m[3][3] + m[3][2]);
  // Far
  planes[5] = glm::vec4(m[0][3] - m[0][2], m[1][3] - m[1][2], m[2][3] - m[2][2], m[3][3] - m[3][2]);

  for (int i = 0; i < 6; i++){
    float length = glm::length(glm::vec3(planes[i]));
    if (length > 0.0f){
      planes[i] /= length;
    };
  };

  for (int i = 0; i < 6; i++){
    glm::vec3 normal = glm::vec3(planes[i]);
    glm::vec3 positiveVertex;

    positiveVertex.x = (normal.x >= 0.0f) ? aabbMax.x : aabbMin.x;
    positiveVertex.y = (normal.y >= 0.0f) ? aabbMax.y : aabbMin.y;
    positiveVertex.z = (normal.z >= 0.0f) ? aabbMax.z : aabbMin.z;

    float distance = glm::dot(normal, positiveVertex) + planes[i].w;

    if (distance < 0.0f){
      return false; 
    };
  };

  return true;
};