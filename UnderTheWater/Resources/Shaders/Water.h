#pragma once
#include "Renderer.h"

#include <string>



namespace WaterShader{
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
in vec3 Normal; // Passed from Evaluation Shader

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
  vec3 N = normalize(Normal);
  vec3 V = normalize(cameraPosition - FragPosition);

  float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
  fresnel = mix(0.05, 1.0, fresnel);

  vec3 color = albedo * 0.1f;

  for(int i = 0; i < lightCount; i++){
    vec3 L = normalize(lights[i].position - FragPosition);
    
    color += BRDF(
      N,
      FragPosition,
      cameraPosition,
      lights[i].position,
      lights[i].color
    ) * lights[i].strength;
  };

  float alpha = clamp(0.2 + fresnel, 0.0, 1.0);
  
  FragColor = vec4(color, alpha);
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
uniform vec2 mapSize;
uniform float waterHeight;
uniform float time;

struct Wave {
  vec2 dir;
  float amp;
  float freq;
  float speed;
};

Wave getWave(int i) {
  float seed = float(i) * 12.9898;
  return Wave(
    normalize(vec2(sin(seed), cos(seed * 0.5))),
    0.9 / float(i + 1),
    0.001 + float(i) * 0.7,
    0.5 + fract(seed) * 0.4
  );
}

float terrainHeight(vec2 p, float timeScale) {
  float h = waterHeight;
  for(int i = 0; i < 20; i++) {
    Wave w = getWave(i);
    float angle = dot(p, w.dir) * w.freq + timeScale * w.speed;
    h += w.amp * sin(angle);
  }
  return h;
}

vec3 getGerstnerNormal(vec2 p, float timeScale) {
  vec3 tangent = vec3(1.0, 0.0, 0.0);
  vec3 binormal = vec3(0.0, 0.0, 1.0);
  
  for(int i = 0; i < 20; i++) {
    Wave w = getWave(i);
    float phase = dot(p, w.dir) * w.freq + timeScale * w.speed;
    float wa = w.freq * w.amp;
    
    tangent += vec3(
      -w.dir.x * w.dir.x * wa * cos(phase),
      -w.dir.x * w.dir.y * wa * cos(phase),
      w.dir.x * wa * cos(phase)
    );
    binormal += vec3(
      -w.dir.x * w.dir.y * wa * cos(phase),
      -w.dir.y * w.dir.y * wa * cos(phase),
      w.dir.y * wa * cos(phase)
    );
  }
  
  return normalize(cross(vec3(0.0, 1.0, 0.0) + binormal, vec3(1.0, 0.0, 0.0) + tangent));
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

  float timeScale = time * 0.5;

  float hCenter = terrainHeight(pos.xz, timeScale);
  pos.y = hCenter;

  FragPosition = pos;
  Normal = getGerstnerNormal(pos.xz, timeScale);

  gl_Position = projection * view * vec4(pos, 1.0);
}
)";
};
