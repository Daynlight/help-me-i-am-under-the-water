#pragma once
#include "Renderer.h"

#include <string>



namespace SkyBoxShader {
inline std::string vertex = R"(#version 430 core

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
)";



inline std::string fragment = R"(#version 430 core

out vec4 FragColor;

in vec3 LocalPos;

uniform sampler2D skyboxTex;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v){
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}

void main(){
  vec2 uv = SampleSphericalMap(normalize(LocalPos));
  vec3 color = texture(skyboxTex, uv).rgb;
  FragColor = vec4(color, 1.0);
}
)";
} // namespace SkyBoxShader
