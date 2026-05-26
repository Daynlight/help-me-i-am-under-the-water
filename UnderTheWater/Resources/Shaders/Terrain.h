#pragma once
#include "Renderer.h"

#include <string>



namespace TerrainShader{
inline std::string vertex = R"(#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;

void main(){
  gl_Position = model * vec4(aPos, 1.0);
}
)";



inline std::string fragment = R"(#version 430 core

out vec4 FragColor;

in vec3 FragPosition;
in vec3 Normal;

)"
+ CW::PBRShaderBRDFUniforms +
R"(

uniform int lightCount;
uniform vec3 cameraPosition;

struct Light {
  vec3 position;
  vec3 color;
  float strength;
};

layout(std430, binding = 0) buffer LightsBuffer {
  Light lights[];
};

)"
+ CW::PBRShaderBRDFInclude +
R"(

void main(){
  vec3 color = vec3(0.0);

  for(int i = 0; i < lightCount; i++){
    color += BRDF(
      normalize(Normal),
      FragPosition,
      cameraPosition,
      lights[i].position,
      lights[i].color
    ) * lights[i].strength;
  };

  FragColor = vec4(color, 1.0);
}
)";



inline std::string tessellation = R"(#version 430 core

layout(vertices = 4) out;

uniform vec3 cameraPosition;
uniform vec2 tessBound;
uniform float distanceCoefficient; 

float computeTessLevel(vec3 p0, vec3 p1, vec3 p2, vec3 p3){
  vec3 center = (p0 + p1 + p2 + p3) * 0.25;

  vec3 diff = cameraPosition - center;
  float dist = dot(diff, diff);
  
  float maxTess = tessBound.y;
  float minTess = tessBound.x;

  float t = exp(-dist * distanceCoefficient);

  return mix(minTess, maxTess, t);
}

void main(){
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  if (gl_InvocationID == 0){
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    float tessLevel = computeTessLevel(p0, p1, p2, p3);

    gl_TessLevelOuter[0] = tessLevel;
    gl_TessLevelOuter[1] = tessLevel;
    gl_TessLevelOuter[2] = tessLevel;
    gl_TessLevelOuter[3] = tessLevel;

    gl_TessLevelInner[0] = tessLevel;
    gl_TessLevelInner[1] = tessLevel;
  }
}
)";



inline std::string tessellation_evaluation = R"(#version 430 core

layout(quads, fractional_even_spacing, ccw) in;

out vec3 FragPosition;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform sampler2D uTexture;
uniform vec2 mapSize;
uniform float maxHeight;

float terrainHeight(vec2 p){
  vec2 worldMin = vec2(-mapSize * 0.5);
  vec2 uv = (p - worldMin) / mapSize;

  vec2 texelSize = 1.0 / textureSize(uTexture, 0);
  
  float texelOffset = 1.5; 
  vec2 offset = texelSize * texelOffset;

  float center = texture(uTexture, uv).r;
  float left   = texture(uTexture, uv + vec2(-offset.x, 0.0)).r;
  float right  = texture(uTexture, uv + vec2(offset.x, 0.0)).r;
  float top    = texture(uTexture, uv + vec2(0.0, offset.y)).r;
  float bottom = texture(uTexture, uv + vec2(0.0, -offset.y)).r;

  float height = (center + left + right + top + bottom) / 5.0;

  return (height - 0.5f) * maxHeight;
}

vec3 calcNormal(vec2 p, float hCenter){
  float eps = 1.0;

  float hR = terrainHeight(p + vec2(eps, 0.0));
  float hU = terrainHeight(p + vec2(0.0, eps));

  float dHdX = hR - hCenter;
  float dHdZ = hU - hCenter;

  return normalize(vec3(-dHdX, eps, -dHdZ));
}

void main(){
  vec3 p0 = gl_in[0].gl_Position.xyz;
  vec3 p1 = gl_in[1].gl_Position.xyz;
  vec3 p2 = gl_in[2].gl_Position.xyz;
  vec3 p3 = gl_in[3].gl_Position.xyz;

  vec2 uv = gl_TessCoord.xy;

  vec3 a = mix(p0, p1, uv.x);
  vec3 b = mix(p3, p2, uv.x);
  vec3 pos = mix(a, b, uv.y);

  float hCenter = terrainHeight(pos.xz);
  pos.y = hCenter;

  FragPosition = pos;
  Normal  = calcNormal(pos.xz, hCenter);

  gl_Position = projection * view * vec4(pos, 1.0);
}
)";
};
