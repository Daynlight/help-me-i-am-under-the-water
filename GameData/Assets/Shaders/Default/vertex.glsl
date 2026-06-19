#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 uvs;
layout(location = 3) in int mat_id;

flat out int material_id;
out vec2 uv;
out vec3 Normal;
out vec3 FragPosition;
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 u_LightSpaceMatrix;

uniform mat4 model;

void main(){
  FragPosLightSpace = u_LightSpaceMatrix * model * vec4(aPos, 1.0);
  vec4 pos = projection * view * model * vec4(aPos, 1.0);
  
  material_id = mat_id;
  uv = uvs;
  Normal = normals;
  FragPosition = pos.xyz;

  gl_Position = pos;
}

