#version 430 core

layout(location = 0) in vec3 aPos;

out vec3 LocalPos;

uniform mat4 projection;
uniform mat4 view;

void main(){
  LocalPos = aPos;

  mat4 staticView = mat4(mat3(view));
  vec4 pos = projection * staticView * vec4(aPos, 1.0);

  gl_Position = pos.xyww;
}