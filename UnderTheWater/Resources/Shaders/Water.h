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

uniform int lightCount;
uniform vec3 cameraPosition;
uniform int material_id;


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





const float PI = 3.14159265359;

// ---------------- BRDF FUNCTIONS ----------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness * roughness;
  float a2 = a * a;

  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / max(denom, 0.0001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;

  return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  return GeometrySchlickGGX(NdotV, roughness) *
          GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 BRDF_PBR(
  vec3 N,
  vec3 V,
  vec3 L,
  vec3 lightColor,
  vec3 albedo,
  float metallic,
  float roughness
)
{
  vec3 H = normalize(V + L);

  float NdotL = max(dot(N, L), 0.0);
  float NdotV = max(dot(N, V), 0.0);
  float NdotH = max(dot(N, H), 0.0);

  // ---------------- Fresnel ----------------
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  vec3 F = FresnelSchlick(max(dot(V, H), 0.0), F0);

  // ---------------- GGX ----------------
  float a = roughness * roughness;
  float a2 = a * a;

  float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
  float D = a2 / (PI * denom * denom + 0.0001);

  float k = (roughness + 1.0);
  k = (k * k) / 8.0;

  float G1 = NdotL / (NdotL * (1.0 - k) + k);
  float G2 = NdotV / (NdotV * (1.0 - k) + k);
  float G = G1 * G2;

  // ---------------- Specular ----------------
  vec3 spec = (D * G * F) / max(4.0 * NdotL * NdotV, 0.001);

  // ---------------- Diffuse (Blender-style energy conservation) ----------------
  vec3 kS = F;
  vec3 kD = (1.0 - kS) * (1.0 - metallic);

  vec3 diffuse = kD * albedo / PI;

  return (diffuse + spec) * lightColor * NdotL;
}

vec3 BRDF(
  vec3 Normal,
  vec3 FragPos,
  vec3 cameraPos,
  vec3 lightPos,
  vec3 lightColor,
  vec3 albedo,
  float metallic,
  float roughness,
  vec3 emission_color,
  float emission_strength,
  float ambient_occlusion
)
{
  vec3 N = normalize(Normal);
  vec3 V = normalize(cameraPos - FragPos);
  vec3 L = normalize(lightPos - FragPos);

  vec3 lighting = BRDF_PBR(N, V, L, lightColor, albedo, metallic, roughness);

  vec3 ambient = 0.03 * albedo * ambient_occlusion;

  vec3 emissiveColor = emission_color * emission_strength;

  return lighting + ambient + emissiveColor;
};




void main(){
  vec3 N = normalize(Normal);
  vec3 V = normalize(cameraPosition - FragPosition);

  float fresnel = pow(1.0 - max(dot(N, V), 0.0), 5.0);
  fresnel = mix(0.05, 1.0, fresnel);

  vec3 color = material[material_id].albedo * 0.1f;

  for(int i = 0; i < lightCount; i++){
    vec3 L = normalize(lights[i].position - FragPosition);
    
    color += BRDF(
      N,
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
