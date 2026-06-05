#pragma once

#include <string>

#include "BRDF.h"


namespace TestingCubeShader{
inline std::string vertex = R"(#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 uvs;
layout(location = 3) in int mat_id;

flat out int material_id;
out vec2 uv;
out vec3 Normal;
out vec3 FragPosition;

uniform mat4 projection;
uniform mat4 view;

uniform mat4 model;

void main(){
  vec4 pos = projection * view * model * vec4(aPos, 1.0);
  
  material_id = mat_id;
  uv = uvs;
  Normal = normals;
  FragPosition = pos.xyz;

  gl_Position = pos;
}
)";

inline std::string fragment = R"(#version 430 core

out vec4 FragColor;

in vec3 FragPosition;
flat in int material_id;
in vec3 Normal;
in vec2 uv;

uniform int lightCount;
uniform vec3 cameraPosition;

struct Light {
  vec3 position;
  vec3 color;
  float strength;
};

struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  vec3 emission_color;
  float emission_strength;
  float ambient_occlusion;
};

layout(std430, binding = 0) buffer LightsBuffer {
  Light lights[];
};

layout(std430, binding = 1) buffer MaterialsBuffer {
  Material material[];
};


)" + BRDF_Include + R"(

vec4 sampleMyTexture(sampler2D tex, vec2 uv) {
  return texture(tex, uv);
}

uniform sampler2D sky_box;

void main(){
  vec3 color = texture(sky_box, uv).rgb;

  for(int i = 0; i < lightCount; i++){
    color += BRDF(
      normalize(Normal),
      FragPosition,
      cameraPosition,
      lights[i].position,
      lights[i].color,
      material[material_id].albedo,
      material[material_id].metallic,
      material[material_id].roughness,
      material[material_id].emission_color,
      material[material_id].emission_strength,
      material[material_id].ambient_occlusion
    ) * lights[i].strength;
  };

  FragColor = vec4(color, 1.0);
}
)";
};